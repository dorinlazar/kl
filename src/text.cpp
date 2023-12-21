#include <kl/text.hpp>

namespace kl {
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

Text::Text(char c){auto ptr = new char[]} Text::Text(const char* ptr) {}
Text::Text(const char* ptr, int32_t size) {}

} // namespace kl