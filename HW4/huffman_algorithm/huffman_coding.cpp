//
// Created by roman on 29.05.18.
//

#include <iostream>
#include "huffman_coding.h"
#include <algorithm>
#include <deque>

void huffman_coding::build_tree() {
  std::vector<std::pair<node, uint64_t>> nodes;
  tree.clear();
  for (size_t i = 0; i < alphabet_size; ++i) {
    if (frequency[i] != 0) {
      nodes.push_back(std::pair<node, uint64_t>(node(tree.size(), i, 0, 0), frequency[i]));
      tree.push_back(node(tree.size(), i, 0, 0));
    }
  }

  std::sort(nodes.begin(), nodes.end(), [](std::pair<node, uint64_t> &l, std::pair<node, uint64_t> &r) {
    return l.second > r.second;
  });

  while (nodes.size() > 1) {
    std::pair<node, uint64_t>
        tmp(node(tree.size(), std::numeric_limits<size_t>::max(), nodes[nodes.size() - 1].first.ind, nodes[nodes.size() - 2].first.ind),
            nodes[nodes.size() - 1].second + nodes[nodes.size() - 2].second);
    tree.emplace_back(tree.size(), std::numeric_limits<size_t>::max(), nodes[nodes.size() - 1].first.ind, nodes[nodes.size() - 2].first.ind);
    nodes.pop_back();
    nodes.pop_back();
    nodes.push_back(tmp);
    std::sort(nodes.begin(), nodes.end(), [](std::pair<node, uint64_t> &l, std::pair<node, uint64_t> &r) {
      return l.second > r.second;
    });
  }
}

void huffman_coding::set_frequency(const uint64_t *new_frequency) {
  for (size_t i = 0; i < alphabet_size; ++i) {
    frequency[i] = new_frequency[i];
  }
}

size_t huffman_coding::get_alphabet_size() {
  return alphabet_size;
}

std::vector<node> &huffman_coding::get_tree() {
  return tree;
}

std::vector<std::pair<uint64_t , size_t >> huffman_coding::get_codes() {
  std::vector<std::pair<uint64_t, size_t>> codes(alphabet_size);
  if (tree.size() == 1) {
    codes[tree[0].symbol].second = 1;
    return codes;
  }
  dfs_for_codes(0, tree.size() - 1, codes, 0);
  return codes;
}

void huffman_coding::dfs_for_codes(uint64_t code, size_t root, std::vector<std::pair<uint64_t,size_t >> &codes, size_t len) {
  if (tree[root].symbol != std::numeric_limits<size_t>::max()) {
    codes[tree[root].symbol].first = code;
    codes[tree[root].symbol].second = len;
    return;
  }
  dfs_for_codes(code<<1, tree[root].left, codes, len + 1);
  dfs_for_codes((code<<1) + 1, tree[root].right, codes, len + 1);
}

uint64_t *huffman_coding::get_frequency() {
  return frequency;
}

node::node(size_t ind, size_t symbol, size_t left, size_t right)
    : ind(ind), symbol(symbol), left(left), right(right) {
}
