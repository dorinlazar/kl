#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace kl {

class Exception : public std::logic_error {
public:
  explicit Exception(const char* message);
  explicit Exception(const std::string& message);
  static Exception OperationNotSupported(const std::string& op, const std::string& reason);
  static Exception InvalidInputData(const std::string& received, const std::string& expected);
  static Exception DuplicateIndex(const std::string& where);
  static Exception IOException(const std::string& why);
  static Exception CurrentStandardIOError();
};

} // namespace kl
