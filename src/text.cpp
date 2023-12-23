#include <kl/text.hpp>
#include <kl/except.hpp>
#include <cstring>

namespace kl {

Text::Text(const Text& value) {
  m_text_buffer = value.m_text_buffer;
  base()->add_ref();
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
  base()->add_ref();
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

Text::Text(char c) : Text(&c, 1) {}

Text::Text(const char* ptr) {
  TSize size = 0;

  if (ptr != nullptr) {
    size = std::strlen(ptr);
  }
  *this = Text(ptr, size);
}

Text::Text(const char* ptr, TSize size) {
  if (ptr == nullptr || size <= 0) {
    *this = Text{""_tr};
    return;
  }
  auto counted_base = TextRefCountedBase::allocate(size);
  m_text_buffer = counted_base->text_address();
  m_start = 0;
  m_end = size;

  for (TSize i = 0; i < size; i++) {
    m_text_buffer[i] = ptr[i];
  }
}

} // namespace kl