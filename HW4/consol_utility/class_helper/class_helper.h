//
// Created by roman on 05.06.18.
//

#ifndef HUFFMAN_CLASS_HELPER_H
#define HUFFMAN_CLASS_HELPER_H

#include "../../huffman_algorithm/huffman_coding.h"
#include "bit_storage/bit_storage.h"
const size_t buffer_size = 1 << 15;

struct class_helper {
 public:
  class_helper() = default;
 private:
//  size_t number_length(uint64_t number);
  void fill_frequency(const std::string &filename, huffman_coding &huff);
  void write_tree(std::ofstream &ofs, std::vector<node> &tree);
  void dfs_for_symbols_and_structure(std::vector<node> &tree,
                                     size_t root,
                                     std::string &symbols,
                                     std::string &structure);
  uint64_t calculate_file_length(uint64_t *frequency, size_t size, std::vector<std::pair<uint64_t, size_t>> &codes);
  void write_coded_file(std::ofstream &ofs, std::string filename, std::vector<std::pair<uint64_t, size_t>> &codes);
  size_t extract_tree_size(std::ifstream &ifs);
  void extract_symbols(std::ifstream &ifs, std::string &symbols, size_t symbols_count);
  void extract_tree_structure(std::ifstream &ifs, std::string &tree_structure, size_t tree_size);
  void restore_tree(std::string &structure, std::string &symbols, std::vector<node> &tree, size_t tree_size);
  size_t extract_file_length(std::ifstream &ifs);
  void decode(size_t file_length, std::ifstream &ifs, std::vector<node> &tree, std::string input_filename);
 public:
  void decompress(std::string filepath, huffman_coding &huff);
  void compress(std::string filepath, huffman_coding &huff);
 private:
  char buffer[buffer_size] = {};
 public:
};
#endif //HUFFMAN_CLASS_HELPER_H
