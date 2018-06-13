//
// Created by roman on 04.06.18.
//

#include <gmpxx.h>
#include <bitset>
#include <iostream>
#include "class_helper.h"

void class_helper::compress(std::string filepath, huffman_coding &huff) {
  ptrdiff_t i = filepath.size() - 1;
  while (i >= 0 && filepath[i] != '/') {
    i--;
  }
  i++;
  std::string filename(filepath.substr(static_cast<unsigned long>(i), filepath.size()));
  fill_frequency(filepath, huff);
  huff.build_tree();
//
  std::ofstream ofs(filename + ".huff", std::ios_base::binary);
  write_tree(ofs, huff.get_tree());
  if (huff.get_tree().size() == 0) {
    ofs.close();
    return;
  }
  std::vector<std::pair<uint64_t, size_t>> codes(huff.get_codes());
  uint64_t length = calculate_file_length(huff.get_frequency(), huff.get_alphabet_size(), codes);
  ofs.write(reinterpret_cast<char *>(&length), sizeof(length));
  write_coded_file(ofs, filepath, codes);

  ofs.close();
}

void class_helper::fill_frequency(const std::string &filename, huffman_coding &huff) {
  std::ifstream ifs(filename, std::ifstream::binary);
  if (!ifs.good()) {
    throw std::string("invalid file");
  }
  uint64_t new_frequency[alphabet_size] = {};
  ifs.read(buffer, buffer_size);
  while (ifs.gcount() != 0) {
    for (ptrdiff_t i = 0; i < ifs.gcount(); ++i) {
      size_t a = static_cast<size_t>(static_cast<unsigned char>(buffer[i]));
      new_frequency[a]++;
    }
    ifs.read(buffer, buffer_size);
  }
  huff.set_frequency(new_frequency);
  ifs.close();
}
void class_helper::write_tree(std::ofstream &ofs, std::vector<node> &tree) {
  size_t size = tree.size();
  for (size_t i = 0; i < sizeof(size_t); ++i) {
    ofs << *(reinterpret_cast<char *>(&size) + i);
  }
  if (size == 0) {
    return;
  }
  std::string symbols;
  std::string structure;
  dfs_for_symbols_and_structure(tree, tree.size() - 1, symbols, structure);
  ofs.write(symbols.data(), symbols.length());
  ofs << structure;
}

void class_helper::dfs_for_symbols_and_structure(std::vector<node> &tree,
                                                 size_t root,
                                                 std::string &symbols,
                                                 std::string &structure) {
  if (tree[root].symbol != std::numeric_limits<size_t>::max()) {
    symbols.push_back(static_cast<char>(tree[root].symbol));
    structure.push_back('U');
    return;
  }
  structure.push_back('L');
  dfs_for_symbols_and_structure(tree, tree[root].left, symbols, structure);
  structure.push_back('R');
  dfs_for_symbols_and_structure(tree, tree[root].right, symbols, structure);
  structure.push_back('U');
}
//size_t class_helper::number_length(uint64_t number) {
//  size_t answer = 1;
//  while (number > 1) {
//    number >>= 1;
//    answer++;
//  }
//  return answer;
//}
uint64_t class_helper::calculate_file_length(uint64_t *frequency,
                                             size_t size,
                                             std::vector<std::pair<uint64_t, size_t>> &codes) {
  uint64_t length = 0;
  for (size_t i = 0; i < size; ++i) {
    length += frequency[i] * codes[i].second;
  }
  return length;
}
void class_helper::write_coded_file(std::ofstream &ofs,
                                    std::string filename,
                                    std::vector<std::pair<uint64_t, size_t>> &codes) {
  std::ifstream ifs(filename, std::ios_base::binary);
  ifs.read(buffer, buffer_size);
  uint64_t crutch;
  bit_storage bit;
  size_t counter = 0;
  while (ifs.gcount() != 0) {
    counter++;
    auto count = static_cast<size_t>(ifs.gcount());
    for (size_t i = 0; i < count; ++i) {
      bit.append(codes[static_cast<size_t>(static_cast<unsigned char>(buffer[i]))]);
    }
//    for (int j = 0; j < bit.length(); ++j) {
//      std::cout<<bit[j];
//    }
    ifs.read(buffer, buffer_size);
    if (ifs.gcount() == 0) {
      std::pair<char *, size_t> p(bit.to_char_array());
      ofs.write(p.first, p.second + (bit.get_last_block_length() - 1) / 8 + 1);
    } else {
      std::pair<char *, size_t> p(bit.to_char_array());
      ofs.write(p.first, p.second);
      crutch = bit.back();
      size_t crutch_length = bit.get_last_block_length();
      bit = bit_storage(crutch, crutch_length);
//      bit.append(std::pair<uint64_t, size_t>(crutch, crutch_length));
//      size_t t = crutch_length - bit.length();
//      for (size_t i = 0; i < t; ++i) {
//        bit.append(std::pair<uint64_t, size_t>(0, 1));
//      }
    }
  }
}

void class_helper::decompress(std::string filename, huffman_coding &huff) {
  if (filename.size() < 6 || filename.substr(filename.size() - 5, filename.size()) != ".huff") {
    throw std::string("invalid file");
  }
  std::ifstream ifs(filename, std::ios_base::binary);
//  ifs.read(buffer, buffer_size);
  if (!ifs.good()) throw std::string("invalid file");
  size_t tree_size = extract_tree_size(ifs);
  if (tree_size == 0) {
    std::ofstream
        ofs(std::string("decoded_").append(filename.substr(0, filename.size() - 5)), std::ios_base::binary);
    ofs.close();
    return;
  }
  std::string symbols;
  extract_symbols(ifs, symbols, (tree_size + 1) / 2);

  std::string tree_structure;
  extract_tree_structure(ifs, tree_structure, tree_size * 2 - 1);

  std::vector<node> tree;
  restore_tree(tree_structure, symbols, tree, tree_size);

  size_t file_length = extract_file_length(ifs);
  decode(file_length, ifs, tree, filename);
}

size_t class_helper::extract_tree_size(std::ifstream &ifs) {
  size_t size = 0;
  auto *arr = new char[sizeof(size_t)];
  ifs.read(arr, sizeof(size_t));
  if (ifs.gcount() != sizeof(size_t)) throw std::string("file is damaged");
  size = *reinterpret_cast<size_t *>(arr);
  delete[] arr;
  return size;
}

void class_helper::extract_symbols(std::ifstream &ifs, std::string &symbols, size_t symbols_count) {
  for (size_t i = 0; i < symbols_count; ++i) {
    char tmp;
    if (ifs.eof()) throw std::string("file is damaged");
    ifs.read(&tmp, 1);
    symbols.push_back(tmp);
  }
}

void class_helper::extract_tree_structure(std::ifstream &ifs, std::string &tree_structure, size_t tree_size) {
  for (size_t i = 0; i < tree_size; ++i) {
    char tmp;
    if (ifs.eof()) throw std::string("file is damaged");
    ifs.read(&tmp, 1);
    tree_structure.push_back(tmp);
  }
}

void class_helper::restore_tree(std::string &structure,
                                std::string &symbols,
                                std::vector<node> &tree,
                                size_t tree_size) {
  size_t ind_in_symb = 0;
  char last_move = 'L';
  std::vector<size_t> parents(tree_size);
  size_t cur_vertex = 0;
  tree.clear();
  tree.push_back(node(0, std::numeric_limits<size_t>::max(), 0, 0));
  for (size_t i = 0; i < structure.length(); ++i) {
    if (structure[i] == 'L') {
      tree[cur_vertex].left = tree.size();
      parents[tree.size()] = cur_vertex;
      cur_vertex = tree.size();
      tree.push_back(node(tree.size(), std::numeric_limits<size_t>::max(), 0, 0));
    }
    if (structure[i] == 'R') {
      tree[cur_vertex].right = tree.size();
      parents[tree.size()] = cur_vertex;
      cur_vertex = tree.size();
      tree.push_back(node(tree.size(), std::numeric_limits<size_t>::max(), 0, 0));
    }
    if (structure[i] == 'U') {
      if (last_move != 'U') {
        tree[cur_vertex].symbol = static_cast<unsigned char>(symbols[ind_in_symb++]);
        cur_vertex = parents[cur_vertex];
      } else {
        cur_vertex = parents[cur_vertex];
      }
    }
    last_move = structure[i];
  }
}
size_t class_helper::extract_file_length(std::ifstream &ifs) {
  size_t length = 0;
  auto *arr = new char[sizeof(size_t)];
  ifs.read(arr, sizeof(arr));
  length = *(reinterpret_cast<size_t *>(arr));
  delete[] arr;
  return length;
}

void class_helper::decode(size_t file_length, std::ifstream &ifs, std::vector<node> &tree, std::string input_filename) {
  std::ofstream
      ofs(std::string("decoded_").append(input_filename.substr(0, input_filename.length() - 5)), std::ios_base::binary);
  ifs.read(buffer, buffer_size);
  size_t cur_ind = 0;
  size_t ind_in_storage = 0;
  size_t ind_in_tree = 0;
  bit_storage bit(buffer, buffer_size);
  if (tree.size() == 1) {
    for (size_t i = 0; i < file_length; ++i) {
      ofs << static_cast<char>(static_cast<unsigned char>(tree[0].symbol));
    }
  } else {
    while (cur_ind < file_length) {
      while (tree[ind_in_tree].symbol == std::numeric_limits<size_t>::max()) {
        if (bit[ind_in_storage]) {
          ind_in_tree = tree[ind_in_tree].right;
        } else {
          ind_in_tree = tree[ind_in_tree].left;
        }
        cur_ind++;
        ind_in_storage++;
        if (ind_in_storage >= buffer_size * 8) {
          ifs.read(buffer, buffer_size);
          if (ifs.gcount() == 0 && cur_ind < file_length) {
            throw std::string("file is damaged");
          }
          bit = bit_storage(buffer, buffer_size);
          ind_in_storage = 0;
        }
//        if (cur_ind >= file_length && ind_in_storage < bit.length() - 1) {
//          int a = bit.length();
//          throw std::string("file is damaged");
//        }
      }
      ofs << static_cast<char>(static_cast<unsigned char>(tree[ind_in_tree].symbol));
      ind_in_tree = 0;
    }
  }
  ofs.close();
}


