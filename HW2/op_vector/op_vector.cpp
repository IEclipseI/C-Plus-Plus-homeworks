//
// Created by roman on 09.05.18.
//

#include "op_vector.h"

using namespace std;

op_vector::op_vector()
    : is_small(true), _size(0), _capacity(_small_object_size) {
  data_ptr = small_object_data;
  for (size_t i = 0; i < _small_object_size; ++i) {
    data_ptr[i] = 0;
  }
}

size_t op_vector::size() const {
  return _size;
}

void op_vector::push_back(uint32_t elem) {
  ensure_capacity(_size + 1);
  data_ptr[_size++] = elem;
}

void op_vector::ensure_capacity(size_t new_size) {
  if (_capacity < new_size) {
    if (is_small) {
      is_small = false;
      shared_ptr<vector<uint32_t>> ptr(new vector<uint32_t>(small_object_data,
                                                            small_object_data +
                                                                sizeof(small_object_data) / sizeof(uint32_t)));
      ptr->resize(_capacity * 2);
      data_ptr = ptr->data();
      _big_object_ptr = ptr;
      _capacity *= 2;
    } else {
      _big_object_ptr->resize(_capacity * 2);
      data_ptr = _big_object_ptr->data();
      _capacity *= 2;
    }
  }
}
size_t op_vector::capacity() const {
  return _capacity;
}
size_t op_vector::small_object_size() const {
  return _small_object_size;
}
uint32_t &op_vector::operator[](uint32_t pos) {
  return data_ptr[pos];
}
op_vector::op_vector(const op_vector &rhs) noexcept :
    _size(rhs._size), is_small(rhs.is_small), _capacity(rhs._capacity),
    _big_object_ptr(rhs._big_object_ptr) {
  if (rhs.is_small) {
    for (size_t i = 0; i < _small_object_size; ++i) {
      small_object_data[i] = rhs.small_object_data[i];
    }
    data_ptr = small_object_data;
  } else {
    data_ptr = rhs.data_ptr;
  }
}
void op_vector::make_unique() {
  if (is_small) {
    for (size_t i = 0; i < _size; ++i) {
      small_object_data[i] = data_ptr[i];
    }
    data_ptr = small_object_data;
  } else {
    if (!_big_object_ptr.unique()) {
      _big_object_ptr = shared_ptr<vector<uint32_t>>(new vector<uint32_t>(*_big_object_ptr));
      data_ptr = _big_object_ptr->data();
    }
  }
}
op_vector &op_vector::operator=(const op_vector &rhs) noexcept {
  _size = rhs.size();
  _capacity = rhs.capacity();
  is_small = rhs.is_small;
  _big_object_ptr = rhs._big_object_ptr;
  if (rhs.is_small) {
    for (size_t i = 0; i < _size; ++i) {
      small_object_data[i] = rhs.small_object_data[i];
    }
    data_ptr = small_object_data;
  } else {
    data_ptr = rhs.data_ptr;
  }
  return *this;
}
void op_vector::pop_back() {
  --_size;
}
const shared_ptr<vector<uint32_t>> &op_vector::big_object_ptr() {
  return _big_object_ptr;
}
uint32_t &op_vector::operator[](uint32_t pos) const {
  return data_ptr[pos];
}
void op_vector::resize(uint32_t size) {
  _size = size;
  ensure_capacity(_size);
}
op_vector::op_vector(size_t size) : _size(size) {
  if (size > _small_object_size) {
    _big_object_ptr = shared_ptr<vector<uint32_t>>(new vector<uint32_t>(size));
    data_ptr = _big_object_ptr->data();
    _capacity = _big_object_ptr->capacity();
    is_small = false;
  } else {
    data_ptr = small_object_data;
    _capacity = 4;
    for (int i = 0; i < _small_object_size; ++i) {
      data_ptr[i] = 0;
    }
    is_small = true;
  }
}
uint32_t op_vector::back() {
  return data_ptr[_size - 1];
}
void op_vector::inverse_values() {
  for (int i = 0; i < _size; ++i) {
    data_ptr[i] = ~data_ptr[i];
  }
}
void op_vector::swap(op_vector &rhs) {
  std::swap(this->_big_object_ptr, rhs._big_object_ptr);
  std::swap(this->is_small, rhs.is_small);
  std::swap(this->_size, rhs._size);
  std::swap(this->_capacity, rhs._capacity);
  std::swap(this->small_object_data, rhs.small_object_data);
  this->data_ptr = is_small?this->small_object_data: this->_big_object_ptr->data();
  rhs.data_ptr = rhs.is_small? rhs.small_object_data: rhs._big_object_ptr->data();
}
