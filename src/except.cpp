#include "kl/except.hpp"
#include <cstring>

namespace kl {

RuntimeError::RuntimeError(const char* message) : m_message(message) {}
RuntimeError::RuntimeError(Text message) : m_message(message) {}
Text RuntimeError::Message() const { return m_message; }

} // namespace kl
