//
// Created by roman on 10.06.18.
//

#include <gtest/gtest.h>
#include <fstream>
#include <consol_utility/class_helper/class_helper.h>

bool compare_two_files(std::string filepath1, std::string filepath2) {
  std::ifstream ifs1(filepath1, std::ios_base::binary);
  std::ifstream ifs2(filepath2, std::ios_base::binary);

  char buffer1[buffer_size];
  char buffer2[buffer_size];

  if (!ifs1 || !ifs2) {
    return false;
  }
  do {
    ifs1.read(buffer1, buffer_size);
    ifs2.read(buffer2, buffer_size);
    if (ifs1.gcount() != ifs2.gcount()) return false;
    for (ptrdiff_t i = 0; i < ifs1.gcount(); ++i) {
      if (buffer1[i] != buffer2[i]) {
        return false;
      }
    }

  }while (ifs1.gcount() > 0 || ifs2.gcount() > 0);

  return true;
}

TEST(correctness, empty)
{
  class_helper helper;
  huffman_coding huff;
  helper.compress("../gtest/empty_test.txt", huff);
  helper.decompress("empty_test.txt.huff", huff);
  EXPECT_EQ(compare_two_files("../gtest/empty_test.txt", "decoded_empty_test.txt"), true);
}

TEST(correctness, so_many_0_test)
{
 class_helper helper;
 huffman_coding huff;
 helper.compress("../gtest/so_many_0_test.txt", huff);
 helper.decompress("so_many_0_test.txt.huff", huff);
 EXPECT_EQ(compare_two_files("../gtest/so_many_0_test.txt", "decoded_so_many_0_test.txt"), true);
}

TEST(correctness, one_mb)
{
 class_helper helper;
 huffman_coding huff;
 helper.compress("../gtest/test1Mb.txt", huff);
 helper.decompress("test1Mb.txt.huff", huff);
 EXPECT_EQ(compare_two_files("../gtest/test1Mb.txt", "decoded_test1Mb.txt"), true);
}

TEST(correctness, compress_eighty_mb)
{
 class_helper helper;
 huffman_coding huff;
 helper.compress("../gtest/test80Mb.txt", huff);
}

//TEST(correctness, eighty_mb)
//{
//  class_helper helper;
//  huffman_coding huff;
//  helper.compress("../gtest/test80Mb.txt", huff);
//  helper.decompress("test80Mb.txt.huff", huff);
//  EXPECT_EQ(compare_two_files("../gtest/test80Mb.txt", "decoded_test80Mb.txt"), true);
//}

TEST(correctness, jpg_test)
{
  class_helper helper;
  huffman_coding huff;
  helper.compress("../gtest/jpg_test.jpg", huff);
  helper.decompress("jpg_test.jpg.huff", huff);
 EXPECT_EQ(compare_two_files("../gtest/jpg_test.jpg", "decoded_jpg_test.jpg"), true);
}
