#include "klexcept.hpp"

namespace kl {

KlException::KlException(Text message) : std::logic_error({message.begin(), message.end()}) {}

OperationNotSupported::OperationNotSupported(Text op, Text reason)
    : KlException(("Operation not supported"_t + op + ": " + reason).to_text()) {}

InvalidInputData::InvalidInputData(Text received, Text expected)
    : KlException(("Invalid input data: [" + received + "], expected: [" + expected + "]"_t).to_text()) {}

IOException::IOException(Text why) : KlException(("IOException: "_t + why).to_text()) {}
IOException IOException::current_standard_error() { return IOException{strerror(errno)}; }

} // namespace kl
