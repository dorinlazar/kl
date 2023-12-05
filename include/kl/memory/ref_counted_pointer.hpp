#pragma once
#include <kl/memory/deleters.hpp>

namespace kl {

template <typename T>
struct RefCountedValue {
  ssize_t reference_count = 0;
  T value;

  template <typename... Args>
  constexpr RefCountedValue(Args&&... args) : value(std::forward<Args>(args)...) {}
  constexpr T* value_address() noexcept { return &value; }
  constexpr void add_new_ref() noexcept { reference_count++; }
  constexpr bool remove_and_check_alive() noexcept {
    reference_count--;
    return reference_count > 0; // maybe this could be optimized by have a size_t and checking against 0;
  }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class ShareableMutablePointer {
  RefCountedValue<T>* m_ptr;

public:
  constexpr ShareableMutablePointer(RefCountedValue<T>* ptr) : m_ptr(ptr) {
    if (m_ptr) {
      m_ptr->add_new_ref();
    }
  }

  constexpr ShareableMutablePointer(const ShareableMutablePointer& val) noexcept : m_ptr(val.m_ptr) {
    if (m_ptr) {
      m_ptr->add_new_ref();
    }
  }
  constexpr ShareableMutablePointer(ShareableMutablePointer&& val) noexcept : m_ptr(val.m_ptr) { val.m_ptr = nullptr; }
  constexpr ShareableMutablePointer& operator=(const ShareableMutablePointer& val) noexcept {
    if (&val != this) {
      reset();
      m_ptr = val.m_ptr;
      if (m_ptr) {
        m_ptr->add_new_ref();
      }
    }
    return *this;
  }
  constexpr ShareableMutablePointer& operator=(ShareableMutablePointer&& val) noexcept {
    reset();
    m_ptr = val.m_ptr;
    val.m_ptr = nullptr;
    return *this;
  }
  constexpr ~ShareableMutablePointer() noexcept { reset(); }

  constexpr void reset() noexcept {
    if (m_ptr) {
      if (!m_ptr->remove_and_check_alive()) {
        delete m_ptr;
      }
      m_ptr = nullptr;
    }
  }

  constexpr T* operator->() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    return m_ptr->value_address();
  }
  constexpr T& operator*() {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    return *(m_ptr->value_address());
  }
};

template <typename T, typename... Args>
constexpr ShareableMutablePointer<T> make_mutable_shareable(Args&&... args) {
  return ShareableMutablePointer<T>(new RefCountedValue<T>(std::forward<Args>(args)...));
}

template <typename T, typename Deleter = DefaultDeleter<T>>
class ShareablePointer {
  RefCountedValue<T>* m_ptr;

public:
  constexpr ShareablePointer(RefCountedValue<T>* ptr) : m_ptr(ptr) { m_ptr->add_new_ref(); }

  constexpr ShareablePointer(const ShareablePointer& val) noexcept : m_ptr(val.m_ptr) { m_ptr->add_new_ref(); }
  constexpr ShareablePointer(ShareablePointer&& val) = delete;
  constexpr ShareablePointer& operator=(const ShareablePointer& val) = delete;
  constexpr ShareablePointer& operator=(ShareablePointer&& val) = delete;
  constexpr ~ShareablePointer() noexcept {
    if (!m_ptr->remove_and_check_alive()) {
      delete m_ptr;
    }
    m_ptr = nullptr;
  }

  constexpr T* operator->() const { return m_ptr->value_address(); }
  constexpr T& operator*() const { return *(m_ptr->value_address()); }
};

template <typename T, typename... Args>
constexpr ShareablePointer<T> make_shareable(Args&&... args) {
  return ShareablePointer<T>(new RefCountedValue<T>(std::forward<Args>(args)...));
}

} // namespace kl
