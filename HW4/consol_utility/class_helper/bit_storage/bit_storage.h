//
// Created by roman on 08.06.18.
//

#ifndef UNTITLED4_BIT_STORAGE_H
#define UNTITLED4_BIT_STORAGE_H

#include <vector>
#include <cstdint>
#include <cstddef>

struct bit_storage {
 public:
  bit_storage();
  bit_storage(char*, size_t size);
  bit_storage(uint64_t crutch, size_t crutch_length);
 public:
  void append(std::pair<uint64_t, size_t> symbol);
  size_t length();
  std::pair<char*, size_t> to_char_array();
  uint64_t back();
  size_t get_last_block_length();
  bool operator[](size_t pos);
 private:
  inline uint64_t reverse_symbol(uint64_t symbol, size_t len);
  inline size_t number_length(uint64_t number);
 private:
  size_t free_places_in_block;
 public:
  std::vector<uint64_t> storage;
};


#endif //UNTITLED4_BIT_STORAGE_H
