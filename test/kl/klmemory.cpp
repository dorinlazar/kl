#include <kl/memory.hpp>
#include <gtest/gtest.h>

static int allocation_count = 0;

class A {
public:
  A() { allocation_count++; }
  A(int allocation_no) { allocation_count = allocation_no; }
  ~A() { allocation_count--; }
};

TEST(klmem, simple_unique_pointer_tests) {
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
    auto ptr_multi = kl::make_array_ptr<A>(100);
    ASSERT_EQ(allocation_count, 100);
    ptr_multi = kl::make_array_ptr<A>(3);
    ASSERT_EQ(allocation_count, 3);
  }
  ASSERT_EQ(allocation_count, 0);
}

static int deleter_count = 0;

template <typename T>
class CustomDeleter {
public:
  CustomDeleter() noexcept = default;
  void operator()(T* ptr) {
    delete ptr;
    deleter_count++;
  }
};

TEST(klmem, test_unique_ptr_deleter_not_called_twice) {
  {
    auto ptr = kl::UniquePtr<A, CustomDeleter<A>>(new A);
    ASSERT_EQ(deleter_count, 0);
  }
  ASSERT_EQ(deleter_count, 1);
  {
    auto ptr = kl::UniquePtr<A, CustomDeleter<A>>(new A());
    ptr = nullptr;
    ASSERT_EQ(deleter_count, 2);
  }
  ASSERT_EQ(deleter_count, 2);
}
