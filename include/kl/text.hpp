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

template <int size>
struct TextRefCounted {
  TextRefCountedBase base;
  char data[size];
};

class Text {
  TextRefCountedBase* m_memblock;
  size_t m_start = 0;
  size_t m_end = 0;

public:
  Text();
  ~Text();
  Text(const Text& value);
  Text(Text&& dying) noexcept;
  Text& operator=(const Text& value);
  Text& operator=(Text&& dying) noexcept;

  Text(char c);
  Text(const std::string& s);
  Text(const char* ptr);
  Text(const char* ptr, size_t size);
  Text(const Text& t, size_t start, size_t length);
  Text(TextRefCounter* buffer, size_t length);
};

} // namespace kl
