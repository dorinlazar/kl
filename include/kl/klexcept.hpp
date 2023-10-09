#pragma once

#include <exception>
#include <stdexcept>
#include "kltext.hpp"

namespace kl {

class KlException : public std::logic_error {
public:
  explicit KlException(Text message);
};

class OperationNotSupported : public KlException {
public:
  OperationNotSupported(Text op, Text reason);
};

class InvalidInputData : public KlException {
public:
  InvalidInputData(Text received, Text expected);
};

class DuplicateIndex : public KlException {
public:
  explicit DuplicateIndex(Text index);
};

class IOException : KlException {
public:
  explicit IOException(Text why);
  static IOException current_standard_error();
};

} // namespace kl
