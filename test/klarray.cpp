#include <gtest/gtest.h>
#include <kl/ds/array.hpp>

using namespace kl;

TEST(klarray, construction) {
  Array<int> a;
  EXPECT_EQ(a.size(), 0);
  EXPECT_EQ(a.reserved(), 0);
  Array<int> b(TagBuild{}, 10);
  EXPECT_EQ(b.size(), 10);
  EXPECT_EQ(b.reserved(), 10);
  Array<int> c(TagNoInit{}, 10);
  EXPECT_EQ(c.size(), 10);
  EXPECT_EQ(c.reserved(), 10);
  Array<int> d(TagReserve{}, 10);
  EXPECT_EQ(d.size(), 0);
  EXPECT_EQ(d.reserved(), 10);
  Array<int> e{1, 2, 3, 4, 5};
  EXPECT_EQ(e.size(), 5);
  EXPECT_EQ(e.reserved(), 5);
  EXPECT_EQ(e[3], 4);
}

TEST(klarray, reallocation) {
  Array<int> a;
  EXPECT_EQ(a.size(), 0);
  EXPECT_EQ(a.reserved(), 0);
  a.push_back(1);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(a.reserved(), 8);
  for (auto i = 0; i < 7; i++) {
    a.push_back(i);
    EXPECT_EQ(a.size(), 2 + i);
    EXPECT_EQ(a.reserved(), 8);
  }
  for (auto i = 0; i < 8; i++) {
    a.push_back(i);
    EXPECT_EQ(a.size(), 9 + i);
    EXPECT_EQ(a.reserved(), 16);
  }
  for (auto i = 0; i < 16; i++) {
    a.push_back(i);
    EXPECT_EQ(a.size(), 17 + i);
    EXPECT_EQ(a.reserved(), 32);
  }
  a.push_back(1);
  EXPECT_EQ(a.size(), 33);
  EXPECT_EQ(a.reserved(), 64);
}
