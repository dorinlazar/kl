#pragma once
#include <kl/text.hpp>

namespace kl {

class RuntimeError {
  Text m_message;

public:
  explicit RuntimeError(const char* message);
  explicit RuntimeError(Text message);
  Text Message() const;
};

} // namespace kl
