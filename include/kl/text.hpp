#pragma once
#include <kl/inttypes.hpp>

namespace kl {

constexpr TSize RefCountedGuard{static_cast<TSize>(0xDEAD7C27)};

struct TextRefCountedBase {
  TSize size = 0;
  TSize refcount = 1;

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

  static constexpr TextRefCountedBase* from_text_address(char* ptr) {
    return reinterpret_cast<TextRefCountedBase*>(ptr - sizeof(TextRefCountedBase));
  }
  static constexpr TextRefCountedBase* allocate(TSize payload_size) {
    if (payload_size < 0) {
      payload_size = 0;
    }
    auto ptr = new char[sizeof(TextRefCountedBase) + payload_size];
    auto base = reinterpret_cast<TextRefCountedBase*>(ptr);
    base->size = payload_size;
    base->refcount = 1;
    return base;
  }

  static constexpr void deallocate(TextRefCountedBase* ptr) {
    if (ptr->refcount != RefCountedGuard) {
      delete[] reinterpret_cast<char*>(ptr);
    }
  }
};

template <TSize Size>
struct TextLiteral {
  TextRefCountedBase base;
  char data[Size];
  static_assert(Size > 0, "The size of a literal should be >0");

  constexpr TextLiteral(const char (&literal)[Size]) {
    base.size = Size - 1;
    if (literal[Size - 1]) {
      base.size++;
    }
    for (TSize i = 0; i < Size; i++) {
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
  TSize m_start = 0;
  TSize m_end = 0;

  constexpr TextRefCountedBase* base() {
    return reinterpret_cast<TextRefCountedBase*>(m_text_buffer - sizeof(TextRefCountedBase));
  }

public:
  constexpr Text() : Text(""_tr) {}
  constexpr ~Text() {
    if (m_text_buffer) {
      auto ptr = base();
      if (ptr->remove_ref_and_test()) {
        TextRefCountedBase::deallocate(ptr);
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
  constexpr Text(std::nullptr_t) : Text() {}
  Text(const char* ptr, TSize size);
  constexpr Text(TextRefCountedBase* ptr) : m_text_buffer(ptr->text_address()), m_start(0), m_end(ptr->size) {}

  constexpr TSize size() { return m_end - m_start; }
};

template <TextLiteral Item>
constexpr Text operator""_t() {
  return Text(const_cast<TextRefCountedBase*>(&Item.base));
}

} // namespace kl
