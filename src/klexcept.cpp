#include "klexcept.hpp"
#include <cstring>

namespace kl {

Exception::Exception(const char* message) : std::logic_error(message) {}
Exception::Exception(const std::string& message) : std::logic_error(message) {}

Exception Exception::OperationNotSupported(const std::string& op, const std::string& reason) {
  return Exception("Operation not supported" + op + ": " + reason);
}
Exception Exception::InvalidInputData(const std::string& received, const std::string& expected) {
  return Exception("Invalid input data: [" + received + "], expected: [" + expected + "]");
}

Exception Exception::DuplicateIndex(const std::string& where) { return Exception("Duplicate index: " + where); }

Exception Exception::IOException(const std::string& why) { return Exception("IOException: " + why); }
Exception Exception::CurrentStandardIOError() { return IOException(strerror(errno)); }

} // namespace kl
