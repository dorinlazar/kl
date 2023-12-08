#pragma once
#include <cstdint>

namespace kl {

constexpr int32_t RefCountedGuard{static_cast<int32_t>(0xDEAD7C27)};

struct TextRefCountedBase {
  int32_t size = 0;
  int32_t refcount = 0;

  constexpr TextRefCountedBase() {
    if consteval {
      refcount = RefCountedGuard;
    }
  }

  constexpr void add_ref() {
    if (refcount != RefCountedGuard) {
      refcount++;
    }
  }
  constexpr bool remove_ref_and_test() {
    if (refcount != RefCountedGuard) {
      refcount--;
      return refcount > 0;
    }
    return true;
  }
};

} // namespace kl
