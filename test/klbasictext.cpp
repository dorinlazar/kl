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

TEST(klbasictext, text_construction) {
  auto t = "  1  Hello world"_t;
  EXPECT_EQ(t.size(), 16);
  Text t2 = t;
  EXPECT_EQ(t2.size(), 16);
  Text t3 = std::move(t2);
  EXPECT_EQ(t3.size(), 16);
  Text t4("  1  Hello world");
  EXPECT_EQ(t4.size(), 16);
  Text t5 = "  1  Hello world";
  EXPECT_EQ(t5.size(), 16);
  Text t6 = 'a';
  EXPECT_EQ(t6.size(), 1);
  Text t7(nullptr);
  EXPECT_EQ(t7.size(), 0);
  Text t8 = "  1  Hello world"_tr;
  EXPECT_EQ(t8.size(), 16);
  Text t9 = "  1  Hello world"_t;
  EXPECT_EQ(t9.size(), 16);
  Text t10("  1  Hello world", 5);
  EXPECT_EQ(t10.size(), 5);
}
