#include <gtest/gtest.h>
#include "kl/text.hpp"
using namespace kl;

TEST(klbasictext, textref) {
  constexpr TextRefCountedBase a;
  EXPECT_EQ(a.refcount, RefCountedGuard);
  a.add_ref();
  EXPECT_EQ(a.refcount, RefCountedGuard);
  TextRefCountedBase b;
  b.size = 10;
  EXPECT_EQ(b.refcount, 0);
}
