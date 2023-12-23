#pragma once
#include <kl/inttypes.hpp>

namespace kl {

template <typename T>
class Array {
  T* m_data;
  TSize m_size;
};

} // namespace kl