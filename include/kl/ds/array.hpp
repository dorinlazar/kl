#pragma once
#include <kl/inttypes.hpp>
#include <kl/ds/tags.hpp>
#include <kl/except.hpp>
#include <utility>
#include <initializer_list>

#include <kl/ds/cursor.hpp>

namespace kl {

template <typename T>
class Array {
  T* m_data;
  TSize m_size;
  TSize m_reserved;

  constexpr TSize flex_index(TSize index) const {
    if (index < 0) {
      index += m_size;
    }
    return strict_index(index);
  }

  constexpr TSize strict_index(TSize index) const {
    if (index >= m_size || index < 0) [[unlikely]] {
      throw Exception("Out of range: {} out of {}", index, m_size);
    }
    return index;
  }

  constexpr void release_data() {
    for (TSize i = 0; i < m_size; i++) {
      m_data[i].~T();
    }
    ::operator delete(m_data);
  }

  constexpr void allocate_space_for_next(int count = 1) {
    if (count < 1) {
      return;
    }
    if (TSIZE_MAX - count < m_size) [[unlikely]] {
      throw Exception("Out of range");
    }
    if (m_size + count > m_reserved) {
      auto required = std::max((m_size + count) - m_reserved, 8);
      auto new_items_size = m_reserved + std::max(m_reserved, required);
      reserve(new_items_size);
    }
  }

public:
  constexpr Array() : m_data(nullptr), m_size(0), m_reserved(0) {}
  template <typename... Args>
  constexpr Array(TagBuild, TSize size, Args&&... args)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(size), m_reserved(size) {
    for (TSize i = 0; i < m_size; i++) {
      new (m_data + i) T(std::forward<Args>(args)...);
    }
  }
  constexpr Array(TagNoInit, TSize size)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(size), m_reserved(size) {}
  constexpr Array(TagReserve, TSize size)
      : m_data(static_cast<T*>(::operator new(size * sizeof(T)))), m_size(0), m_reserved(size) {}

  constexpr Array(std::initializer_list<T> list) : Array(TagNoInit{}, list.size()) {
    TSize i = 0;
    for (const auto& item: list) {
      new (m_data + i) T(item);
      i++;
    }
  }

  constexpr ~Array() {
    release_data();
    m_data = nullptr;
    m_size = 0;
    m_reserved = 0;
  }

  constexpr T& push_back(const T& value) {
    allocate_space_for_next();
    new (m_data + m_size) T(value);
    ++m_size;
    return m_data[m_size - 1];
  }

  template <typename... Args>
  constexpr T& emplace_back(Args&&... args) {
    allocate_space_for_next();
    new (m_data + m_size) T(std::forward(args)...);
    ++m_size;
    return m_data[m_size - 1];
  }

  constexpr void reserve(TSize size) {
    if (size > m_reserved) {
      auto new_data = static_cast<T*>(::operator new(size * sizeof(T)));
      for (TSize i = 0; i < m_size; i++) {
        new (new_data + i) T(std::move(m_data[i]));
      }
      release_data();
      m_data = new_data;
      m_reserved = size;
    }
  }

  constexpr T* begin() const { return m_data; }
  constexpr T* end() const { return m_data + m_size; }

  constexpr Cursor first() const { return {0}; }
  constexpr Cursor last() const { return {m_size - 1}; }
  constexpr bool valid(Cursor cur) const { return cur.position() >= 0 && cur.position() < m_size; }

  constexpr const T& operator[](TSize index) const { return m_data[flex_index(index)]; }
  constexpr T& operator[](TSize index) { return m_data[flex_index(index)]; }
  constexpr const T& operator[](Cursor index) const { return m_data[strict_index(index.position())]; }
  constexpr T& operator[](Cursor index) { return m_data[strict_index(index.position())]; }

  constexpr TSize size() const { return m_size; }
  constexpr TSize reserved() const { return m_reserved; }
  constexpr T* data() const { return m_data; }
};

} // namespace kl