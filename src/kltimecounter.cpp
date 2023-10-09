#include "kltimecounter.hpp"

namespace kl {
TimeCounter::TimeCounter(const kl::Text& op) : m_start(kl::DateTime::now()), m_op(op) {}
TimeCounter::~TimeCounter() { kl::log("{} done. Time spent: {}", m_op, kl::DateTime::now() - m_start); }
} // namespace kl
