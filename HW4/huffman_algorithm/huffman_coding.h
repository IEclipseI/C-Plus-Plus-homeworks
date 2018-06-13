//
// Created by roman on 29.05.18.
//

#ifndef HUFFMAN_COMPRESSOR_H
#define HUFFMAN_COMPRESSOR_H

#include <fstream>
#include <vector>

static const size_t alphabet_size = 256;

struct node {
  node() = default;
  node(size_t ind, size_t symbol, size_t left, size_t right);
  size_t ind;
  size_t symbol;
  size_t left, right;
};

struct huffman_coding {
 public:
  huffman_coding() = default;
 private:
  void dfs_for_codes(uint64_t code, size_t root, std::vector<std::pair<uint64_t, size_t>> &codes, size_t len);
 public:
  void build_tree();
  void set_frequency(const uint64_t *new_frequency);
  size_t get_alphabet_size();
  std::vector<node> &get_tree();
  std::vector<std::pair<uint64_t, size_t>> get_codes();
  uint64_t *get_frequency();
 private:
  uint64_t frequency[alphabet_size] = {};
 public:
  std::vector<node> tree; // root is always last element;
};

#endif //HUFFMAN_COMPRESSOR_H
