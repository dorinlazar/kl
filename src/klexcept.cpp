#include "klexcept.hpp"
#include <cstring>

namespace kl {

RuntimeError::RuntimeError(const char* message) : std::runtime_error(message) {}
RuntimeError::RuntimeError(const std::string& message) : std::runtime_error(message) {}

RuntimeError RuntimeError::OperationNotSupported(const std::string& op, const std::string& reason) {
  return RuntimeError("Operation not supported" + op + ": " + reason);
}
RuntimeError RuntimeError::InvalidInputData(const std::string& received, const std::string& expected) {
  return RuntimeError("Invalid input data: [" + received + "], expected: [" + expected + "]");
}

RuntimeError RuntimeError::DuplicateIndex(const std::string& where) {
  return RuntimeError("Duplicate index: " + where);
}

RuntimeError RuntimeError::IOException(const std::string& why) { return RuntimeError("IOException: " + why); }
RuntimeError RuntimeError::CurrentStandardIOError() { return IOException(strerror(errno)); }

} // namespace kl
