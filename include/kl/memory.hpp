#pragma once
#include <kl/klexcept.hpp>

namespace kl {

template <typename T>
struct DefaultDeleter {
  DefaultDeleter() noexcept = default;
  void operator()(T* ptr) { delete ptr; }
};

template <typename T>
struct DefaultMultiDeleter {
  DefaultMultiDeleter() noexcept = default;
  void operator()(T* ptr) { delete[] ptr; }
};

template <typename T, class Deleter = DefaultDeleter<T>>
class UniquePtr {
  T* m_ptr = nullptr;

public:
  constexpr UniquePtr(T* ptr = nullptr) : m_ptr(ptr) {}
  constexpr UniquePtr(UniquePtr&& ptr) {
    m_ptr = ptr.m_ptr;
    ptr.m_ptr = nullptr;
  }
  UniquePtr(const UniquePtr& ptr) = delete;
  constexpr UniquePtr& operator=(UniquePtr&& ptr) {
    if (this != &ptr) {
      reset();
      m_ptr = ptr.m_ptr;
      ptr.m_ptr = nullptr;
    }
    return *this;
  }
  UniquePtr& operator=(const UniquePtr& ptr) = delete;
  constexpr UniquePtr& operator=(nullptr_t ptr) {
    reset();
    return *this;
  }

  ~UniquePtr() { reset(); }

  constexpr T& operator->() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw Exception("Null reference exception");
    }
    return *m_ptr;
  }

  constexpr T& operator*() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw Exception("Null reference exception");
    }
    return *m_ptr;
  }

  constexpr T* get() const { return m_ptr; }
  constexpr T* release() {
    auto ptr = m_ptr;
    m_ptr = nullptr;
    return ptr;
  }

  constexpr void reset() {
    Deleter()(m_ptr);
    m_ptr = nullptr;
  }
};

template <typename T, typename... Args>
UniquePtr<T> make_ptr(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
UniquePtr<T, DefaultMultiDeleter<T>> make_ptr_multi(size_t size) {
  return UniquePtr<T, DefaultMultiDeleter<T>>(new T[size]);
}

} // namespace kl
