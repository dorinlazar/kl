#pragma once

namespace kl {

template <typename T>
struct DefaultDeleter {
  DefaultDeleter() noexcept = default;
  void operator()(T* ptr) { delete ptr; }
};

template <typename T>
struct DefaultMultiDeleter {
  DefaultMultiDeleter() noexcept = default;
  void operator()(T* ptr) { delete[] ptr; }
};

} // namespace kl
