//
// Created by roman on 29.05.18.
//

#include <iostream>
#include <cstring>
#include <bitset>
#include "../huffman_algorithm/huffman_coding.h"
#include "class_helper/class_helper.h"
#include <cmath>
//extern void fill_frequency(std::string filename, huffman_coding &huff);

const std::string help_message
    ("Use -comp filename.* — to compress file, result writed into file.*.huff\nUse -decomp filename.*.huff — to decompress file, result writed into encoded_filename.*\n");
const std::string default_message("Use -help for information\n");

int main(int argc, char *argv[]) {
//  std::ofstream ofs("abc.txt");
//  for (int i = 0; i < 20; ++i) {
//      for (int k = 0; k < pow(2, i); ++k) {
//        ofs << char(i*8);
//      }
//  }
//  ofs.close();

  switch (argc) {
    case 2: {
      if (strcmp(argv[1], "-help") == 0) {
        std::cout << help_message;
      } else {
        std::cout << default_message;
      }
      break;
    }
    case 3: {
      if (strcmp(argv[1], "-comp") == 0) {
        try {
          std::cout << "compressing...\n";
          huffman_coding huff;
          class_helper helper;
          helper.compress(std::string(argv[2]), huff);
        } catch (std::string e) {
          std::cout << "error, while compressing: " + e + "\n";
        }
      } else if (strcmp(argv[1], "-decomp") == 0) {
        try {
          std::cout << "decompressing...\n";
          huffman_coding huff;
          class_helper helper;
          helper.decompress(std::string(argv[2]), huff);
        } catch (std::string e) {
          std::cout << "error, while decompressing: " + e + "\n";
        }
      } else {
        std::cout << default_message;
      }
      break;
    }
    default: {
      std::cout << default_message;
    };
  }
  return 0;
}
