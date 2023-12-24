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
}
