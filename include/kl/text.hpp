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
      return refcount <= 0;
    }
    return false;
  }
  constexpr char* text_address() const {
    return reinterpret_cast<char*>(const_cast<TextRefCountedBase*>(this)) + sizeof(*this);
  }
};

template <uint32_t Size>
struct TextLiteral {
  TextRefCountedBase base;
  char data[Size];
  static_assert(Size > 0, "The size of a literal should be >0");

  constexpr TextLiteral(const char (&literal)[Size]) {
    base.size = Size - 1;
    if (literal[Size - 1]) {
      base.size++;
    }
    for (uint32_t i = 0; i < Size; i++) {
      data[i] = literal[i];
    }
  }
};

template <TextLiteral Item>
constexpr TextRefCountedBase* operator""_tr() {
  return const_cast<TextRefCountedBase*>(&Item.base);
}

class Text {
  char* m_text_buffer = nullptr;
  int32_t m_start = 0;
  int32_t m_end = 0;

  constexpr TextRefCountedBase* Base() {
    return reinterpret_cast<TextRefCountedBase*>(m_text_buffer - sizeof(TextRefCountedBase));
  }

public:
  constexpr Text() : Text(""_tr) {}
  constexpr ~Text() {
    if (m_text_buffer) {
      auto ptr = Base();
      if (ptr->remove_ref_and_test()) {
        delete ptr;
      }
      ptr = nullptr;
      m_start = 0;
      m_end = 0;
    }
  }
  Text(const Text& value);
  Text(Text&& dying) noexcept;
  Text& operator=(const Text& value);
  Text& operator=(Text&& dying) noexcept;

  Text(char c);
  Text(const char* ptr);
  Text(const char* ptr, int32_t size);
  constexpr Text(TextRefCountedBase* ptr) : m_text_buffer(ptr->text_address()), m_start(0), m_end(ptr->size) {}
};

template <TextLiteral Item>
constexpr Text operator""_t() {
  return Text(const_cast<TextRefCountedBase*>(&Item.base));
}

} // namespace kl
