//
// Created by roman on 09.05.18.
//

#ifndef VECTOR_OPTIMIZATIONS_OPTIMIZED_VECTOR_H
#define VECTOR_OPTIMIZATIONS_OPTIMIZED_VECTOR_H

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class op_vector {
 public:
  op_vector(const op_vector &) noexcept;
  op_vector &operator=(const op_vector &) noexcept;
  op_vector();
  explicit op_vector(size_t size);
 private:
  size_t _size;
  bool is_small;
  static const size_t _small_object_size = 1;
  size_t _capacity;
  uint32_t small_object_data[_small_object_size];
  shared_ptr<vector<uint32_t>> _big_object_ptr;
  uint32_t *data_ptr;
 private:
  void ensure_capacity(size_t size);
 public:
  size_t small_object_size() const;
  size_t capacity() const;
  size_t size() const;
  void push_back(uint32_t elem);
  uint32_t &operator[](uint32_t pos);
  uint32_t &operator[](uint32_t pos) const;
  void resize(uint32_t size);
  void make_unique();
  void pop_back();
  const shared_ptr<vector<uint32_t>>& big_object_ptr();
  uint32_t back();
  void inverse_values();
  void swap(op_vector& rhs);
};

#endif //VECTOR_OPTIMIZATIONS_OPTIMIZED_VECTOR_H
