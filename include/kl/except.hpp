#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace kl {

class RuntimeError : public std::runtime_error {
public:
  explicit RuntimeError(const char* message);
  explicit RuntimeError(const std::string& message);
  static RuntimeError OperationNotSupported(const std::string& op, const std::string& reason);
  static RuntimeError InvalidInputData(const std::string& received, const std::string& expected);
  static RuntimeError DuplicateIndex(const std::string& where);
  static RuntimeError IOException(const std::string& why);
  static RuntimeError CurrentStandardIOError();
};

} // namespace kl
