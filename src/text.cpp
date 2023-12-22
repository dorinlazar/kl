#include <kl/text.hpp>
#include <kl/except.hpp>

namespace kl {

TextRefCountedBase* TextRefCountedBase::allocate(TSize payload_size) {
  if (payload_size <= 0) {
    throw RuntimeError("TextRefCountedBase::allocate: payload_size < 0");
  }
  auto ptr = new char[sizeof(TextRefCountedBase) + payload_size];
  auto base = reinterpret_cast<TextRefCountedBase*>(ptr);
  base->size = payload_size;
  base->refcount = 1;
  return base;
}

Text::Text(const Text& value) {
  m_text_buffer = value.m_text_buffer;
  Base()->add_ref();
  m_start = value.m_start;
  m_end = value.m_end;
}

Text::Text(Text&& dying) noexcept {
  m_text_buffer = dying.m_text_buffer;
  m_start = dying.m_start;
  m_end = dying.m_end;
  dying.m_text_buffer = nullptr;
  dying.m_start = 0;
  dying.m_end = 0;
}

Text& Text::operator=(const Text& value) {
  m_text_buffer = value.m_text_buffer;
  Base()->add_ref();
  m_start = value.m_start;
  m_end = value.m_end;
  return *this;
}

Text& Text::operator=(Text&& dying) noexcept {
  m_text_buffer = dying.m_text_buffer;
  m_start = dying.m_start;
  m_end = dying.m_end;
  dying.m_text_buffer = nullptr;
  dying.m_start = 0;
  dying.m_end = 0;
  return *this;
}

Text::Text(char c) {
  char v[2]{c, 0};
  *this = Text{v};
};
Text::Text(const char* ptr) {}
Text::Text(const char* ptr, int32_t size) {}

} // namespace kl