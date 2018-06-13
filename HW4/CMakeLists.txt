cmake_minimum_required(VERSION 3.9)
project(huffman)

include_directories(${PROJECT_SOURCE_DIR})


set(CMAKE_CXX_STANDARD 14)

add_executable(huffman consol_utility/consol_utility.cpp
        consol_utility/class_helper/class_helper.cpp
        consol_utility/class_helper/class_helper.h
        consol_utility/class_helper/bit_storage/bit_storage.h
        consol_utility/class_helper/bit_storage/bit_storage.cpp)
add_executable(huffman_algo_tests
        gtest/haffman_algo_tests.cpp
        consol_utility/class_helper/class_helper.cpp
        consol_utility/class_helper/class_helper.h
        consol_utility/class_helper/bit_storage/bit_storage.h
        consol_utility/class_helper/bit_storage/bit_storage.cpp
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc)
add_library(huffman_algorithm
        huffman_algorithm/huffman_coding.cpp
        huffman_algorithm/huffman_coding.h
        )

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif ()
target_link_libraries(huffman_algo_tests huffman_algorithm -lgmp -lgmpxx -lpthread)
target_link_libraries(huffman huffman_algorithm)