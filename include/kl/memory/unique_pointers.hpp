#pragma once
#include <kl/except.hpp>
#include <kl/memory/deleters.hpp>

namespace kl {

template <typename T, class Deleter = DefaultDeleter<T>>
class UniquePointer {
  T* m_ptr = nullptr;

public:
  constexpr UniquePointer(T* ptr = nullptr) : m_ptr(ptr) {}
  constexpr UniquePointer(UniquePointer&& ptr) { m_ptr = ptr.release(); }
  UniquePointer(const UniquePointer& ptr) = delete;
  constexpr UniquePointer& operator=(UniquePointer&& ptr) {
    if (this != &ptr) {
      reset();
      m_ptr = ptr.m_ptr;
      ptr.m_ptr = nullptr;
    }
    return *this;
  }
  UniquePointer& operator=(const UniquePointer& ptr) = delete;
  constexpr UniquePointer& operator=(nullptr_t ptr) {
    reset();
    return *this;
  }

  constexpr ~UniquePointer() { reset(); }

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
class UniqueArrayPointer {
  T* m_ptr = nullptr;
  size_t m_size = 0;

public:
  constexpr UniqueArrayPointer() noexcept = default;
  constexpr UniqueArrayPointer(T* ptr, size_t size) : m_ptr(ptr), m_size(size) {}
  constexpr UniqueArrayPointer(UniqueArrayPointer&& ptr) {
    m_size = ptr.m_size;
    m_ptr = ptr.release();
  }
  UniqueArrayPointer(const UniqueArrayPointer& ptr) = delete;
  constexpr UniqueArrayPointer& operator=(UniqueArrayPointer&& ptr) {
    if (this != &ptr) {
      reset();
      m_size = ptr.m_size;
      m_ptr = ptr.release();
    }
    return *this;
  }
  UniqueArrayPointer& operator=(const UniqueArrayPointer& ptr) = delete;
  constexpr UniqueArrayPointer& operator=(nullptr_t ptr) {
    reset();
    return *this;
  }

  constexpr ~UniqueArrayPointer() { reset(); }

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
constexpr UniquePointer<T> make_ptr(Args&&... args) {
  return UniquePointer<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
constexpr UniqueArrayPointer<T> make_array_ptr(size_t size) {
  return UniqueArrayPointer<T>(new T[size], size);
}

} // namespace kl
