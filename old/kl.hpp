#pragma once
#include <iostream>
#include <functional>
#include <source_location>
#include <format>
#include <memory>

// TODO(dorin): use std::print

namespace kl {
template <typename... Args>
inline void log(std::format_string<Args...> frmt, Args&&... args) {
  std::cout << std::format(frmt, std::forward<Args>(args)...) << "\n";
}

template <typename... Args>
inline void err(std::format_string<Args...> frmt, Args&&... args) {
  std::cerr << std::format(frmt, std::forward<Args>(args)...) << "\n";
}

template <typename... Args>
[[noreturn]] void fatal(std::format_string<Args...> frmt, Args&&... args) {
  kl::err(frmt, std::forward<Args>(args)...);
  std::cout.flush();
  std::cerr.flush();
  std::abort();
}

inline void check(bool value) {
  if (value) [[likely]] {
    return;
  }
  fatal("");
}

template <typename... Args>
inline void check(bool value, std::format_string<Args...> frmt, Args&&... args) {
  if (value) [[likely]] {
    return;
  }
  fatal(frmt, std::forward<Args>(args)...);
}

inline void check_st(bool value, const std::source_location& location = std::source_location::current()) {
  if (value) {
    return;
  }
  fatal("Failure at: {} {}", location.file_name(), location.line());
}

template <typename EX>
inline void expect_ex(std::function<void()> op,
                      const std::source_location& location = std::source_location::current()) {
  try {
    op();
    fatal("Expected exception was not triggered at: {:s}:{:d}", location.file_name(), location.line());
  } catch (const EX&) {
  } catch (...) {
    fatal("Invalid exception triggered at: {:s}:{:d}", location.file_name(), location.line());
  }
}

template <typename T>
using ptr = std::shared_ptr<T>;

template <typename T>
using uptr = std::unique_ptr<T>;

} // namespace kl
