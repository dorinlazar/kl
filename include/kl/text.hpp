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

template <uint32_t Size>
struct TextRefCounted {
  TextRefCountedBase base;
  char data[Size];
  static_assert(Size > 0, "The size of a literal should be >0");
  constexpr TextRefCounted(const char literal[Size]) {
    base.size = Size - 1;
    if (literal[Size - 1]) {
      base.size++;
    }
    for (uint32_t i = 0; i < Size; i++) {
      data[i] = literal[i];
    }
  }
};

class Text {
  TextRefCountedBase* m_memblock;
  size_t m_start = 0;
  size_t m_end = 0;

  friend constexpr Text operator""_t(const char*, size_t);

public:
  Text();
  ~Text();
  Text(const Text& value);
  Text(Text&& dying) noexcept;
  Text& operator=(const Text& value);
  Text& operator=(Text&& dying) noexcept;

  Text(char c);
  Text(const char* ptr);
  Text(const char* ptr, size_t size);
};

template <size_t Size>
constexpr TextRefCounted<Size> operator""_t(const char* ptr, size_t size) {
  return TextRefCounted<Size>(ptr);
}

} // namespace kl
