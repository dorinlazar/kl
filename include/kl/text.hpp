#pragma once
#include <cstdint>

namespace kl {

constexpr int32_t REF_COUNTED_GUARD = 0xDEAD7C27;

struct TextRefCountedBase {
  int32_t size = 0;
  int32_t refcount = 0;

  constexpr TextRefCountedBase() {
    if consteval {
      refcount = REF_COUNTED_GUARD;
    }
  }

  constexpr void add_ref() { refcount++; }
  constexpr bool remove_ref_and_test() {
    refcount--;
    return refcount > 0;
  }
};

} // namespace kl
