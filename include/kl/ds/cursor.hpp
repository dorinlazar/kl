#pragma once

#include <kl/inttypes.hpp>

namespace kl {

class Cursor {
  TSize m_position;

public:
  constexpr Cursor(TSize pos = 0) : m_position(pos) {}

  constexpr TSize position() const { return m_position; }

  constexpr Cursor& operator++() {
    m_position++;
    return *this;
  }
  constexpr Cursor& operator--() {
    m_position--;
    return *this;
  }
  constexpr Cursor operator++(int) {
    auto tmp = *this;
    m_position++;
    return tmp;
  }
  constexpr Cursor operator--(int) {
    auto tmp = *this;
    m_position--;
    return tmp;
  }

  constexpr auto operator<=>(const Cursor&) const = default;
};

} // namespace kl