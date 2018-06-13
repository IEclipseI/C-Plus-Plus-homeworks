//
// Created by roman on 08.06.18.
//

#include "bit_storage.h"

bit_storage::bit_storage() : free_places_in_block(64), storage(std::vector<uint64_t>(1)) {
}

size_t bit_storage::number_length(uint64_t number) {
  size_t answer = 1;
  while (number > 1) {
    number >>= 1;
    answer++;
  }
  return answer;
}

void bit_storage::append(std::pair<uint64_t, size_t> symbol) {
  size_t len = symbol.second;
  uint64_t new_symbol = reverse_symbol(symbol.first, len);
  if (len <= free_places_in_block) {
    storage.back() += (new_symbol << (64 - free_places_in_block));
    free_places_in_block -= len;
  } else {
    if (free_places_in_block != 0) storage.back() += new_symbol << (64 - free_places_in_block);
    storage.push_back(0);
    storage.back() += (new_symbol >> free_places_in_block);
    free_places_in_block = 64 - (len - free_places_in_block);
  }
}

size_t bit_storage::length() {
  return storage.size() * 64 - free_places_in_block;
}

std::pair<char *, size_t> bit_storage::to_char_array() {
  return std::pair<char *, size_t>(reinterpret_cast<char *>(storage.data()),
                                   storage.size() * 8 - 8u);
}

uint64_t bit_storage::back() {
  return storage.back();
}

size_t bit_storage::get_last_block_length() {
  return 64 - free_places_in_block;
}

bit_storage::bit_storage(char *arr, size_t size) : free_places_in_block(0) {
  size /= 8;
  auto *data = reinterpret_cast<uint64_t *>(arr);
  for (size_t i = 0; i < size; ++i) {
    storage.push_back(data[i]);
  }
}
bool bit_storage::operator[](size_t pos) {
  return static_cast<bool>((storage[pos / 64] >> (pos - pos / 64 * 64)) % 2);
}
uint64_t bit_storage::reverse_symbol(uint64_t symbol, size_t len) {
  uint64_t shift = len - 1;
  uint64_t result = 0;
  for (size_t i = 0; i < len; ++i) {
    result += (symbol % 2) << shift--;
    symbol >>= 1;
  }
  return result;
}

bit_storage::bit_storage(uint64_t crutch, size_t crutch_length):free_places_in_block(64 - crutch_length) {
  storage = std::vector<uint64_t>({crutch});
}

