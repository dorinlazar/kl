#pragma once
#include <kl/inttypes.hpp>
#include <kl/ds/tags.hpp>
#include <kl/except.hpp>

namespace kl {

template <typename T>
class Array {
  T* m_data;
  TSize m_size;
  TSize m_reserved;

  TSize get_index(TSize index) const {
    if (index < 0) {
      index += m_size;
    }
    if (index >= m_size || index < 0) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return index;
  }

  void release_data() {
    for (TSize i = 0; i < m_size; ++i) {
      m_data[i].~T();
    }
    ::operator delete(m_data);
  }

public:
  constexpr Array() : m_data(nullptr), m_size(0), m_reserved(0) {}
  template <typename... Args>
  constexpr Array(TagBuild, TSize size, Args&&... args)
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
    ::operator delete(m_data);
    m_data = nullptr;
    m_size = 0;
    m_reserved = 0;
  }

  constexpr const auto& operator[](TSize index) const { return m_data[get_index(index)]; }
  constexpr auto& operator[](TSize index) { return m_data[get_index(index)]; }

  constexpr TSize size() const { return m_size; }
  constexpr TSize reserved() const { return m_reserved; }
  constexpr T* data() const { return m_data; }

  void reserve(TSize size) {
    if (size > m_reserved) {
      auto new_data = static_cast<T*>(::operator new(size * sizeof(T)));
      for (TSize i = 0; i < m_size; ++i) {
        new (new_data + i) T(std::move(m_data[i]));
      }
      release_data();
      m_data = new_data;
      m_reserved = size;
    }
  }
};

} // namespace kl