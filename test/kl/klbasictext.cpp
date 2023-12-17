#include <gtest/gtest.h>
#include "kl/text.hpp"
using namespace kl;

TEST(klbasictext, textref) {
  constexpr TextRefCountedBase a;
  EXPECT_EQ(a.refcount, RefCountedGuard);
  auto item = "  1  Hello world"_tr;
  EXPECT_EQ(item->refcount, RefCountedGuard);
  item->add_ref();
  EXPECT_EQ(item->refcount, RefCountedGuard);
  EXPECT_FALSE(item->remove_ref_and_test());
  EXPECT_EQ(item->refcount, RefCountedGuard);
  auto item2 = "  1  Hello world"_tr;
  EXPECT_EQ(item->refcount, RefCountedGuard);
  EXPECT_EQ(item, item2);
}
