#pragma once
#include <stdexcept>
#include <format>

namespace kl {

class Exception : std::exception {
  std::string m_message;

public:
  template <class... Args>
  Exception(std::format_string<Args...> fmt, Args&&... args)
      : m_message(std::format(fmt, std::forward<Args>(args)...)) {}
};

} // namespace kl
