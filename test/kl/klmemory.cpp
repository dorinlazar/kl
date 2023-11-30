#include <kl/memory.hpp>
#include <gtest/gtest.h>

static int allocation_count = 0;

class A {
public:
  A() { allocation_count++; }
  A(int allocation_no) { allocation_count = allocation_no; }
  ~A() { allocation_count--; }
};

TEST(klmem, test_unique_ptr) {
  allocation_count = 0;
  auto ptr = kl::make_ptr<A>();
  ASSERT_EQ(allocation_count, 1);
  ptr.reset();
  ASSERT_EQ(allocation_count, 0);
  ptr = kl::make_ptr<A>(100);
  ASSERT_EQ(allocation_count, 100);
  ptr = nullptr;
  ASSERT_EQ(allocation_count, 99);
  allocation_count = 0;
  {
    auto ptr_multi = kl::make_ptr_multi<A>(100);
    ASSERT_EQ(allocation_count, 100);
    ptr_multi = kl::make_ptr_multi<A>(3);
    ASSERT_EQ(allocation_count, 3);
  }
  ASSERT_EQ(allocation_count, 0);
}
