#pragma once
#include <kl/klexcept.hpp>
#include <kl/memory/deleters.hpp>

namespace kl {

template <typename T, class Deleter = DefaultDeleter<T>>
class UniquePtr {
  T* m_ptr = nullptr;

public:
  constexpr UniquePtr(T* ptr = nullptr) : m_ptr(ptr) {}
  constexpr UniquePtr(UniquePtr&& ptr) { m_ptr = ptr.release(); }
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

  constexpr ~UniquePtr() { reset(); }

  constexpr T* operator->() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    return m_ptr;
  }

  constexpr T& operator*() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
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
    if (m_ptr != nullptr) [[likely]] {
      Deleter()(m_ptr);
      m_ptr = nullptr;
    }
  }
};

template <typename T, class Deleter = DefaultMultiDeleter<T>>
class UniqueArrayPtr {
  T* m_ptr = nullptr;
  size_t m_size = 0;

public:
  constexpr UniqueArrayPtr() noexcept = default;
  constexpr UniqueArrayPtr(T* ptr, size_t size) : m_ptr(ptr), m_size(size) {}
  constexpr UniqueArrayPtr(UniqueArrayPtr&& ptr) {
    m_size = ptr.m_size;
    m_ptr = ptr.release();
  }
  UniqueArrayPtr(const UniqueArrayPtr& ptr) = delete;
  constexpr UniqueArrayPtr& operator=(UniqueArrayPtr&& ptr) {
    if (this != &ptr) {
      reset();
      m_size = ptr.m_size;
      m_ptr = ptr.release();
    }
    return *this;
  }
  UniqueArrayPtr& operator=(const UniqueArrayPtr& ptr) = delete;
  constexpr UniqueArrayPtr& operator=(nullptr_t ptr) {
    reset();
    return *this;
  }

  constexpr ~UniqueArrayPtr() { reset(); }

  constexpr T& operator[](size_t index) {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    if (index >= m_size) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return *m_ptr;
  }

  constexpr T* get() const { return m_ptr; }
  size_t size() const { return m_size; }
  constexpr T* release() {
    auto ptr = m_ptr;
    m_ptr = nullptr;
    m_size = 0;
    return ptr;
  }

  constexpr void reset() {
    if (m_ptr != nullptr) [[likely]] {
      Deleter()(m_ptr);
      m_ptr = nullptr;
      m_size = 0;
    }
  }
};

template <typename T, typename... Args>
constexpr UniquePtr<T> make_ptr(Args&&... args) {
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
constexpr UniqueArrayPtr<T> make_array_ptr(size_t size) {
  return UniqueArrayPtr<T>(new T[size], size);
}

} // namespace kl
