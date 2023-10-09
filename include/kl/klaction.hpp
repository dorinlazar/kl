#pragma once
#include <functional>

namespace kl {
class Action {
  std::function<void()> m_action;

public:
  Action(std::function<void()> action) : m_action(std::move(action)) {}
  void operator()() { m_action(); }
};

} // namespace kl
