#pragma once
#include <kl/inttypes.hpp>
#include <kl/ds/tags.hpp>

namespace kl {

template <typename T>
class Array {
  T* m_data;
  TSize m_size;
  TSize m_reserved;

public:
  constexpr Array() : m_data(nullptr), m_size(0), m_reserved(0) {}
  template <typename... Args>
  constexpr Array(TagBuild, TSize size, Args...&& args)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(size), m_reserved(size) {
    for (TSize i = 0; i < m_size; ++i) {
      new (m_data + i) T(std::forward<Args>(args)...);
    }
  }
  constexpr Array(TagNoInit, TSize size)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(size), m_reserved(size) {}
  constexpr Array(TagReserve, TSize size)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(0), m_reserved(size) {}

  constexpr ~Array() {
    for (TSize i = 0; i < m_size; ++i) {
      m_data[i].~T();
    }
    delete static_cast<void*>(m_data);
    m_data = nullptr;
    m_size = 0;
    m_reserved = 0;
  }

  template <class Self>
  constexpr auto&& operator[](this Self&& self, TSize index) {
    if (self.m_ptr == nullptr) [[unlikely]] {
      throw RuntimeError("Null reference");
    }
    if (index < 0) {
      index += self.m_size;
    }
    if (index >= self.m_size || index < 0) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return self.m_ptr[index];
  }

  constexpr TSize size() const { return m_size; }
  constexpr TSize reserved() const { return m_reserved; }
  constexpr T* data() const { return m_data; }

  void reserve(TSize size) {
    if (size > m_reserved) {
      auto new_data = static_cast<T*>(::operator new(size * sizeof(T)));
      for (TSize i = 0; i < m_size; ++i) {
        new (new_data + i) T(std::move(m_data[i]));
        m_data[i].~T();
      }
      delete static_cast<void*> m_data;
      m_data = new_data;
      m_reserved = size;
    }
  }
};

} // namespace kl