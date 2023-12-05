#include <kl/memory.hpp>
#include <gtest/gtest.h>

static const int CANARY_VALUE = -5555;
static int object_count = 0;
static int creation_count = 0;
static int deletion_count = 0;
static int deleter_count = 0;

class KLMem : public testing::Test {
public:
  KLMem() {}

protected:
  void SetUp() override {
    object_count = 0;
    creation_count = 0;
    deletion_count = 0;
    deleter_count = 0;
  }
};

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

TEST_F(KLMem, simple_unique_pointer_tests) {
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

template <typename T>
class CustomDeleter {
public:
  CustomDeleter() noexcept = default;
  void operator()(T* ptr) {
    delete ptr;
    deleter_count++;
  }
};

TEST_F(KLMem, test_unique_ptr_deleter_not_called_twice) {
  {
    auto ptr = kl::UniquePointer<A, CustomDeleter<A>>(new A);
    ASSERT_EQ(deleter_count, 0);
  }
  ASSERT_EQ(deleter_count, 1);
  {
    auto ptr = kl::UniquePointer<A, CustomDeleter<A>>(new A());
    ptr = nullptr;
    ASSERT_EQ(deleter_count, 2);
  }
  ASSERT_EQ(deleter_count, 2);
}

TEST_F(KLMem, array_tests) {
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
    kl::UniqueArrayPointer<A> ptr3(std::move(ptr2));
    ASSERT_EQ(object_count, 100);
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(creation_count, 0);
    ptr3 = std::move(ptr3); // this is a bad idea!
  }
  ASSERT_EQ(object_count, 0);
  {
    kl::UniqueArrayPointer<A> ptr3(std::move(ptr));
    ASSERT_EQ(object_count, 0);
    ASSERT_EQ(ptr3.get(), nullptr);
    ASSERT_EQ(ptr3.size(), 0);
  }
}

TEST_F(KLMem, test_pointer) {
  {
    kl::Pointer<A> ptr(100);
    ASSERT_EQ(object_count, 100);
    ASSERT_EQ(creation_count, 1);
    ASSERT_NO_THROW(ptr->foo());
    ASSERT_NO_THROW((*ptr).foo());
  }
  ASSERT_EQ(object_count, 99);
  ASSERT_EQ(deletion_count, 1);
}

template <typename T>
size_t get_reference_count(T* ptr) {
  return *reinterpret_cast<const size_t*>(reinterpret_cast<const uint8_t*>(ptr) - sizeof(size_t));
}

TEST_F(KLMem, test_ref_counted_pointer) {
  {
    auto ptr = kl::make_mutable_shareable<A>(1);
    ASSERT_EQ(object_count, 1);
    ASSERT_EQ(creation_count, 1);
    {
      auto ptr_moved = ptr;
      ASSERT_EQ(get_reference_count<A>(&(*ptr_moved)), 2);
      auto ptr3(std::move(ptr_moved));
      ASSERT_EQ(get_reference_count<A>(&(*ptr3)), 2);
      auto ptr4 = ptr3;
      ASSERT_EQ(get_reference_count<A>(&(*ptr4)), 3);
      ASSERT_EQ(&(*ptr4), &(*ptr3));
      ASSERT_EQ(&(*ptr4), &(*ptr));
      ASSERT_EQ(deletion_count, 0);
    }
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(get_reference_count<A>(&(*ptr)), 1);
  }
  ASSERT_EQ(deletion_count, 1);
  ASSERT_EQ(object_count, 0);
}

TEST_F(KLMem, test_ref_counted_pointer_2) {
  {
    auto ptr = kl::make_mutable_shareable<A>(1);
    ASSERT_EQ(object_count, 1);
    ASSERT_EQ(creation_count, 1);
    {
      kl::ShareableMutablePointer<A> ptr_moved = nullptr;
      ptr_moved = ptr;
      ASSERT_EQ(get_reference_count<A>(&(*ptr_moved)), 2);
      kl::ShareableMutablePointer<A> ptr3 = nullptr;
      EXPECT_THROW((*ptr3).foo(), kl::RuntimeError);
      EXPECT_THROW(ptr3->foo(), kl::RuntimeError);
      ptr3 = std::move(ptr_moved);
      ASSERT_EQ(get_reference_count<A>(&(*ptr3)), 2);
      auto ptr4 = ptr3;
      ASSERT_EQ(get_reference_count<A>(&(*ptr4)), 3);
      ASSERT_EQ(&(*ptr4), &(*ptr3));
      ASSERT_EQ(&(*ptr4), &(*ptr));
      ASSERT_EQ(deletion_count, 0);
      ASSERT_EQ(ptr4->foo(), CANARY_VALUE);
    }
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(get_reference_count<A>(&(*ptr)), 1);
  }
  ASSERT_EQ(deletion_count, 1);
  ASSERT_EQ(object_count, 0);
}

TEST_F(KLMem, test_shareable_pointer_2) {
  {
    kl::ShareablePointer<A> ptr = kl::make_shareable<A>(100);
    ASSERT_EQ(object_count, 100);
    ASSERT_EQ(creation_count, 1);
    {
      kl::ShareablePointer<A> ptr2(ptr);
      ASSERT_EQ(get_reference_count<A>(&(*ptr2)), 2);
      kl::ShareablePointer<A> ptr3 = ptr2;
      ASSERT_EQ(get_reference_count<A>(&(*ptr3)), 3);
      ASSERT_EQ(&(*ptr3), &(*ptr2));
      ASSERT_EQ(&(*ptr3), &(*ptr));
      ASSERT_EQ(deletion_count, 0);
      ASSERT_EQ(ptr3->foo(), CANARY_VALUE);
    }
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(get_reference_count<A>(&(*ptr)), 1);
  }
  ASSERT_EQ(deletion_count, 1);
  ASSERT_EQ(object_count, 99);
}

TEST_F(KLMem, shared_array_tests) {
  kl::SharedArrayPointer<A> ptr(100);
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
  ptr = nullptr;
  ASSERT_EQ(object_count, 0);
  ASSERT_EQ(ptr.size(), 0);
  ASSERT_EQ(ptr.get(), nullptr);

  {
    kl::SharedArrayPointer<A> ptr2(100);
    ASSERT_EQ(object_count, 100);
    deletion_count = 0;
    creation_count = 0;
    kl::SharedArrayPointer<A> ptr3(std::move(ptr2));
    ASSERT_EQ(object_count, 100);
    ASSERT_EQ(deletion_count, 0);
    ASSERT_EQ(creation_count, 0);
    ptr = ptr3;
  }
  ASSERT_EQ(object_count, 100);
  ptr = nullptr;
  {
    kl::SharedArrayPointer<A> ptr3(std::move(ptr));
    ASSERT_EQ(object_count, 0);
    ASSERT_EQ(ptr3.get(), nullptr);
    ASSERT_EQ(ptr3.size(), 0);
  }
}
