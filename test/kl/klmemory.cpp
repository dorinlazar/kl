#include <kl/memory.hpp>
#include <gtest/gtest.h>

static const int CANARY_VALUE = -5555;
static int object_count = 0;
static int creation_count = 0;
static int deletion_count = 0;

class A {
public:
  A() {
    object_count++;
    creation_count++;
  }
  A(int allocation_no) {
    object_count = allocation_no;
    creation_count++;
  }
  ~A() {
    object_count--;
    deletion_count++;
  }
  int foo() { return CANARY_VALUE; }
};

TEST(klmem, simple_unique_pointer_tests) {
  object_count = 0;
  auto ptr = kl::make_ptr<A>();
  ASSERT_EQ(object_count, 1);
  ptr.reset();
  ASSERT_EQ(object_count, 0);
  deletion_count = 0;
  ptr = kl::make_ptr<A>(120);
  ASSERT_EQ(object_count, 120);
  ASSERT_EQ(ptr->foo(), CANARY_VALUE);
  ASSERT_EQ((*ptr).foo(), CANARY_VALUE);
  ptr = nullptr;
  EXPECT_THROW(ptr->foo(), kl::RuntimeError);
  EXPECT_THROW((*ptr).foo(), kl::RuntimeError);
  ASSERT_EQ(object_count, 119);
  ASSERT_EQ(deletion_count, 1);
  {
    object_count = 0;
    creation_count = 0;
    deletion_count = 0;
    auto ptr2 = kl::make_ptr<A>();
    ASSERT_EQ(object_count, 1);
    ASSERT_EQ(creation_count, 1);
    auto ptr3(std::move(ptr2));
    ASSERT_EQ(object_count, 1);
    ASSERT_EQ(creation_count, 1);
    ASSERT_EQ(deletion_count, 0);
  }
  ASSERT_EQ(deletion_count, 1);
  ASSERT_EQ(object_count, 0);
  ptr = kl::make_ptr<A>();
  delete ptr.release();
  ASSERT_EQ(ptr.get(), nullptr);
  EXPECT_THROW(ptr->foo(), kl::RuntimeError);
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

TEST(klmem, array_tests) {
  object_count = 0;
  auto ptr = kl::make_array_ptr<A>(100);
  ASSERT_EQ(object_count, 100);
  ASSERT_NE(ptr.get(), nullptr);
  ASSERT_EQ(ptr.size(), 100);
  ASSERT_THROW(ptr[100], kl::RuntimeError);
  ASSERT_NO_THROW(ptr[99]);
  ptr.reset();
  ASSERT_EQ(object_count, 0);
  ASSERT_EQ(ptr.get(), nullptr);
  ASSERT_EQ(ptr.size(), 0);
  ASSERT_THROW(ptr[0], kl::RuntimeError);
  ASSERT_NO_THROW(ptr.reset());
  ASSERT_EQ(ptr.size(), 0);
  ptr = kl::make_array_ptr<A>(30);
  ASSERT_EQ(object_count, 30);
  ptr = nullptr;
  ASSERT_EQ(object_count, 0);
  ASSERT_NO_THROW(ptr.reset());
  ASSERT_EQ(object_count, 0);
  try {
    ptr = kl::make_array_ptr<A>(30);
    ASSERT_EQ(object_count, 30);
    auto ptr2 = kl::make_array_ptr<A>(100);
    ASSERT_EQ(object_count, 130);
    throw kl::RuntimeError("Hello");
  } catch (const kl::RuntimeError&) {
  }
  ASSERT_EQ(object_count, 30);
  delete[] (ptr.release());
  ASSERT_EQ(object_count, 0);
  ASSERT_EQ(ptr.size(), 0);
  ASSERT_EQ(ptr.get(), nullptr);

  {
    auto ptr2 = kl::make_array_ptr<A>(100);
    ASSERT_EQ(object_count, 100);
    deletion_count = 0;
    creation_count = 0;
    kl::UniqueArrayPtr<A> ptr3(std::move(ptr2));
    ASSERT_EQ(object_count, 100);
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(creation_count, 0);
    ptr3 = std::move(ptr3); // this is a bad idea!
  }
  ASSERT_EQ(object_count, 0);
  {
    kl::UniqueArrayPtr<A> ptr3(std::move(ptr));
    ASSERT_EQ(object_count, 0);
    ASSERT_EQ(ptr3.get(), nullptr);
    ASSERT_EQ(ptr3.size(), 0);
  }
}
