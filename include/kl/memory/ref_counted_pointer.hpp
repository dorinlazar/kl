#pragma once
#include <kl/memory/deleters.hpp>
#include <stdint.h>

namespace kl {

template <typename T>
struct RefCountedValue {
  int64_t reference_count = 0;
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

struct RefCountedBase {
  int64_t reference_count = 0;

  template <typename T>
  constexpr T* start_address() noexcept {
    return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(this) + sizeof(RefCountedBase));
  }
  constexpr void add_new_ref() noexcept { reference_count++; }
  constexpr bool remove_and_check_alive() noexcept {
    reference_count--;
    return reference_count > 0; // maybe this could be optimized by have a size_t and checking against 0;
  }
};

enum class InitializationType { Constructor, None };

template <typename T>
class SharedArrayPointer {
  RefCountedBase* m_ptr;
  int64_t m_size;

public:
  constexpr SharedArrayPointer(int64_t size, InitializationType init_type = InitializationType::Constructor)
      : m_size(size) {
    if (size > 0) {
      m_ptr = reinterpret_cast<RefCountedBase*>(malloc(sizeof(RefCountedBase) + m_size * sizeof(T)));
      if (init_type == InitializationType::Constructor) {
        for (int i = 0; i < m_size; i++) {
          new (m_ptr->start_address<T>() + i) T();
        }
      }
      m_ptr->reference_count = 1;
    } else {
      m_ptr = nullptr;
      m_size = 0;
    }
  }

  constexpr SharedArrayPointer(const SharedArrayPointer& val) noexcept : m_ptr(val.m_ptr), m_size(val.m_size) {
    if (m_ptr) {
      m_ptr->add_new_ref();
    }
  }
  constexpr SharedArrayPointer(SharedArrayPointer&& val) noexcept : m_ptr(val.m_ptr), m_size(val.m_size) {
    val.m_ptr = nullptr;
    val.m_size = 0;
  }

  constexpr SharedArrayPointer& operator=(const SharedArrayPointer& val) noexcept {
    if (&val != this) {
      reset();
      m_ptr = val.m_ptr;
      m_size = val.m_size;
      if (m_ptr) {
        m_ptr->add_new_ref();
      }
    }
    return *this;
  }
  constexpr SharedArrayPointer& operator=(SharedArrayPointer&& val) noexcept {
    reset();
    m_ptr = val.m_ptr;
    m_size = val.m_size;
    val.m_ptr = nullptr;
    val.m_size = 0;
    return *this;
  }
  constexpr SharedArrayPointer& operator=(nullptr_t) noexcept {
    reset();
    return *this;
  }
  constexpr ~SharedArrayPointer() noexcept { reset(); }

  constexpr void reset() noexcept {
    if (m_ptr) {
      if (!m_ptr->remove_and_check_alive()) {
        for (int64_t i = 0; i < m_size; i++) {
          (m_ptr->start_address<T>() + i)->~T();
        }
        free(m_ptr);
      }
      m_ptr = nullptr;
      m_size = 0;
    }
  }

  constexpr T* get() const {
    if (m_ptr) {
      return m_ptr->start_address<T>();
    }
    return nullptr;
  }
  constexpr int64_t size() const { return m_size; }
  constexpr T& operator[](int64_t index) {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    if (index < 0) {
      index += m_size;
    }
    if (index >= m_size || index < 0) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return m_ptr->start_address<T>()[index];
  }
};

} // namespace kl
