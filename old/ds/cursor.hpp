#pragma once
#include <cstddef>

namespace kl {

class LinearCursor {
  size_t m_position = 0;

public:
  LinearCursor() = default;
  explicit LinearCursor(size_t start_pos) : m_position(start_pos) {}

  void inc() { m_position++; }
  size_t pos() const { return m_position; }
};

} // namespace kl
