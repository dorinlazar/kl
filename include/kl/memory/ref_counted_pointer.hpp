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

enum class InitializationType { Constructor, None };

template <typename T>
class SharedArrayPointer {
  RefCountedValue<T[]>* m_ptr;
  size_t m_size;

public:
  constexpr SharedArrayPointer(size_t size, InitializationType init_type = InitializationType::Constructor)
      : m_size(size) {
    if (size > 0) {
      m_ptr = reinterpret_cast<RefCountedValue<T[]>>(malloc(sizeof(RefCountedValue<T[]>) + m_size * sizeof(T)));
      if (init_type == InitializationType::Constructor) {
        for (const int i = 0; i < m_size; i++) {
          new (&(m_ptr->value()) + i) T();
        }
      }
      m_ptr->add_new_ref();
    } else {
      m_ptr = nullptr;
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
    m_ptr = nullptr;
    m_size = 0;
    return *this;
  }
  constexpr ~SharedArrayPointer() noexcept { reset(); }

  constexpr void reset() noexcept {
    if (m_ptr) {
      if (!m_ptr->remove_and_check_alive()) {
        delete m_ptr;
      }
      m_ptr = nullptr;
      m_size = 0;
    }
  }

  constexpr T* get() const {
    if (m_ptr) {
      return m_value.value_address();
    }
    return nullptr;
  }
  constexpr size_t size() const { return m_size; }
  constexpr T& operator[](size_t index) {
    if (m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    if (index >= m_size) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return m_ptr->value_address()[index];
  }
};

} // namespace kl
