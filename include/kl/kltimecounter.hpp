#pragma once
#include <kl/kltime.hpp>

namespace kl {

class TimeCounter {
  kl::DateTime m_start;
  kl::Text m_op;

public:
  explicit TimeCounter(const Text& operation);
  TimeCounter(const TimeCounter&) = default;
  TimeCounter(TimeCounter&&) = default;
  TimeCounter& operator=(const TimeCounter&) = default;
  TimeCounter& operator=(TimeCounter&&) = default;
  ~TimeCounter();
};

} // namespace kl
