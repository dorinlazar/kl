#pragma once

namespace kl {

template <typename T>
constexpr auto min(const T& a, const T& b) {
  return a < b ? a : b;
}

template <typename T>
constexpr auto max(const T& a, const T& b) {
  return a > b ? a : b;
}

} // namespace kl
