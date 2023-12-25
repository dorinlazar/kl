#pragma once
#include <kl/inttypes.hpp>
#include <kl/ds/tags.hpp>
#include <kl/except.hpp>
#include <kl/klbase.hpp>

namespace kl {

template <typename T>
class Array {
  T* m_data;
  TSize m_size;
  TSize m_reserved;

  constexpr TSize get_index(TSize index) const {
    if (index < 0) {
      index += m_size;
    }
    if (index >= m_size || index < 0) [[unlikely]] {
      throw RuntimeError("Out of range");
    }
    return index;
  }

  constexpr void release_data() {
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
    release_data();
    m_data = nullptr;
    m_size = 0;
    m_reserved = 0;
  }

  constexpr const auto& operator[](TSize index) const { return m_data[get_index(index)]; }
  constexpr auto& operator[](TSize index) { return m_data[get_index(index)]; }

  constexpr TSize size() const { return m_size; }
  constexpr TSize reserved() const { return m_reserved; }
  constexpr T* data() const { return m_data; }

  constexpr T* begin() const { return m_data; }
  constexpr T* end() const { return m_data + m_size; }

  constexpr void push_back(const T& value) {
    if (m_size == m_reserved) {
      auto new_items_size = m_reserved + kl::min(kl::max(m_reserved * 2, 8), 128);
      reserve(new_items_size);
    }
    new (m_data + m_size) T(value);
    ++m_size;
  }

  constexpr void reserve(TSize size) {
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