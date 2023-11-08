#include "kltext.hpp"
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <numeric>

using namespace std::literals;

namespace kl {

constexpr std::string_view WHITESPACE = " \t\n\r"sv;

TextView::TextView() : m_view(WHITESPACE.begin(), WHITESPACE.begin()) {}
TextView::TextView(std::string_view v) : m_view(v) {}
TextView::TextView(const char* text) {
  if (text != nullptr) {
    m_view = text;
  } else {
    m_view = std::string_view(WHITESPACE.begin(), WHITESPACE.begin());
  }
}

TextView TextView::trim() const { return trim_left().trim_right(); }
TextView TextView::trim_left() const { return skip(WHITESPACE); }
TextView TextView::trim_right() const {
  auto position = m_view.find_last_not_of(WHITESPACE);
  if (position == std::string_view::npos) {
    return {};
  }
  return m_view.substr(0, position + 1);
}

bool TextView::starts_with(char c) const { return !m_view.empty() && m_view.front() == c; }
bool TextView::starts_with(const TextView& tv) const { return m_view.starts_with(tv.m_view); }
bool TextView::ends_with(char c) const { return !m_view.empty() && m_view.back() == c; }
bool TextView::ends_with(const TextView& tv) const { return m_view.ends_with(tv.m_view); }

char TextView::operator[](ssize_t index) const {
  if (index >= static_cast<ssize_t>(size()) || (-index > static_cast<ssize_t>(size()))) [[unlikely]] {
    throw std::out_of_range(std::format("Requested index {} out of {}", index, size()));
  }
  return m_view[index + ((index < 0) ? size() : 0)];
}

size_t TextView::size() const { return m_view.size(); }
const char* TextView::begin() const { return m_view.begin(); }
const char* TextView::end() const { return m_view.end(); }

std::strong_ordering TextView::operator<=>(const TextView& v) const { return m_view <=> v.m_view; }
std::strong_ordering TextView::operator<=>(const std::string_view& sv) const { return m_view <=> sv; }
std::strong_ordering TextView::operator<=>(const std::string& s) const { return m_view <=> s; }
std::strong_ordering TextView::operator<=>(const char* s) const { return m_view <=> TextView(s); }
bool TextView::operator==(const TextView& v) const { return m_view == v.m_view; }
bool TextView::operator==(const std::string_view& sv) const { return m_view == sv; }
bool TextView::operator==(const std::string& s) const { return m_view == s; }
bool TextView::operator==(const char* s) const { return m_view == s; }

TextView::operator std::string_view() const { return m_view; }
std::string_view TextView::view() const { return m_view; }

bool TextView::contains(char c) const { return pos(c).has_value(); }

TextView TextView::skip(const TextView& skippables) const {
  auto v = m_view;
  v.remove_prefix(std::min(v.find_first_not_of(skippables.m_view), v.size()));
  return v;
}
TextView TextView::skip(size_t n) const { return m_view.substr(std::min(n, size())); }
TextView TextView::skip_bom() const {
  if (size() >= 3) {
    auto buf = begin();
    if (buf[0] == '\xEF' && buf[1] == '\xBB' && buf[2] == '\xBF') {
      return skip(3);
    }
  }
  return *this;
}

// substring position based. The string will contain the character from ending position too.
TextView TextView::subpos(size_t start, size_t end) const {
  if (start > end) {
    return {};
  }
  end = std::min(end + 1, size());
  start = std::min(start, size());

  return m_view.substr(std::min(start, size()), end - start);
}

// substring length based. The return value will have a string of at most <len> characters
TextView TextView::sublen(size_t start, size_t len) const { return m_view.substr(start, len); }

std::optional<size_t> TextView::pos(char c, size_t occurence) const {
  if (occurence == 0) {
    return std::nullopt;
  }
  size_t position = 0;
  while (position < m_view.size()) {
    position = m_view.find_first_of(c, position);
    if (position != std::string_view::npos) {
      occurence--;
      if (occurence == 0) {
        return position;
      }
      position++;
    }
  }

  return std::nullopt;
}

std::optional<size_t> TextView::pos(const TextView& t, size_t occurence) const {
  if (occurence == 0 || t.size() == 0) {
    return std::nullopt;
  }
  size_t position = 0;
  while (position < m_view.size()) {
    position = m_view.find(t.m_view, position);
    if (position < m_view.size()) {
      occurence--;
      if (occurence == 0) {
        return position;
      }
      position += t.size();
    }
  }

  return std::nullopt;
}

std::optional<size_t> TextView::last_pos(char c) const {
  auto res = m_view.find_last_of(c);
  if (res != std::string_view::npos) {
    return res;
  }
  return std::nullopt;
}

std::pair<TextView, TextView> TextView::split_pos(ssize_t where) const {
  auto maxn = static_cast<ssize_t>(size());
  if (where < 0) {
    where += maxn;
    if (where < 0) {
      where = 0;
    }
  } else {
    where = std::min(where, maxn);
  }
  return std::pair<TextView, TextView>{sublen(0, where), sublen(where, maxn - where)};
}

std::pair<TextView, std::optional<TextView>> TextView::split_next_char(char c, SplitDirection direction) const {
  auto pos = m_view.find_first_of(c);
  if (pos == std::string_view::npos) {
    return std::pair<TextView, std::optional<TextView>>{m_view, std::nullopt};
  }
  if (direction == SplitDirection::Discard) {
    return std::pair<TextView, std::optional<TextView>>{m_view.substr(0, pos), m_view.substr(pos + 1)};
  }
  if (direction == SplitDirection::KeepLeft) {
    pos++;
  }
  return std::pair<TextView, std::optional<TextView>>{m_view.substr(0, pos), m_view.substr(pos)};
}

std::pair<TextView, std::optional<TextView>> TextView::split_next_line() const {
  auto pos = m_view.find_first_of('\n');
  if (pos == std::string_view::npos) {
    return std::pair<TextView, std::optional<TextView>>{m_view, std::nullopt};
  }
  if (pos > 0 && m_view[pos - 1] == '\r') {
    return std::pair<TextView, std::optional<TextView>>{m_view.substr(0, pos - 1), m_view.substr(pos + 1)};
  }
  return std::pair<TextView, std::optional<TextView>>{m_view.substr(0, pos), m_view.substr(pos + 1)};
}

List<TextView> TextView::split_lines(SplitEmpty on_empty) const {
  List<TextView> res;
  std::optional<TextView> view = *this;
  TextView first_line;
  while (view.has_value()) {
    std::tie(first_line, view) = view.value().split_next_line();
    if (first_line.size() > 0 || on_empty == SplitEmpty::Keep) {
      res.add(first_line);
    }
  }
  return res;
}

List<TextView> TextView::split_by_char(char c, SplitEmpty on_empty) const {
  List<TextView> res;
  std::optional<TextView> view = *this;
  TextView first_line;
  while (view.has_value()) {
    std::tie(first_line, view) = view.value().split_next_char(c);
    if (first_line.size() > 0 || on_empty == SplitEmpty::Keep) {
      res.add(first_line);
    }
  }

  return res;
}

List<TextView> TextView::split_by_text(const TextView& t, SplitEmpty on_empty) const {
  List<TextView> res;
  TextView view = *this;
  TextView first_line;
  while (view.size() > 0) {
    auto opos = view.pos(t);
    if (opos.has_value()) {
      first_line = view.sublen(0, opos.value());
      view = view.sublen(opos.value() + t.size(), view.size());
    } else {
      first_line = view;
      view = {};
    }
    if (first_line.size() > 0 || on_empty == SplitEmpty::Keep) {
      res.add(first_line);
    }
    if (on_empty == SplitEmpty::Keep && view.size() == 0 && opos.has_value()) {
      res.add(view);
    }
  }
  return res;
}

std::optional<TextView> TextView::expect(const TextView& t) const {
  if (starts_with(t)) {
    return skip(t.size());
  }
  return {};
}

std::optional<TextView> TextView::expect_ws(const TextView& t) const { return trim_left().expect(t); }

std::optional<TextView> TextView::expect(char c) const {
  if (starts_with(c)) {
    return skip(1);
  }
  return {};
}

std::optional<TextView> TextView::expect_ws(char c) const { return trim_left().expect(c); }

std::optional<TextView> TextView::skip_indent(size_t indentLevel) const {
  if (size() < indentLevel) [[unlikely]] {
    return {};
  }
  auto ptr = begin();
  for (size_t i = 0; i < indentLevel; i++) {
    if (*ptr != ' ') {
      return {};
    }
    ptr++;
  }
  return skip(indentLevel);
}

size_t TextView::get_indent() const {
  size_t level = 0;
  for (const char c: *this) {
    if (c == ' ') {
      level++;
    } else {
      return level;
    }
  }
  return level;
}

size_t TextView::count(char c) const {
  size_t count = 0;
  for (auto ch: *this) {
    if (ch == c) {
      count++;
    }
  }
  return count;
}

TextRefCounter* TextRefCounter::acquire() {
  m_ref_count++;
  return this;
}
bool TextRefCounter::release() {
  m_ref_count--;
  return m_ref_count == 0;
}

char* TextRefCounter::text_data() { return reinterpret_cast<char*>(this) + sizeof(TextRefCounter); }
TextRefCounter* TextRefCounter::allocate(size_t text_size) {
  auto buffer = reinterpret_cast<TextRefCounter*>(malloc(sizeof(TextRefCounter) + text_size));
  buffer->m_ref_count = 1;
  return buffer;
}

void TextRefCounter::release(TextRefCounter* ref) {
  if (ref != &TextRefCounter::m_s_empty) {
    if (ref->release()) {
      free(ref);
    }
  }
}

TextRefCounter TextRefCounter::m_s_empty;

Text::Text() : m_memblock(&TextRefCounter::m_s_empty) {}

Text::~Text() { clear(); }

Text::Text(const Text& value) : m_memblock(value.m_memblock->acquire()), m_start(value.m_start), m_end(value.m_end) {}

Text::Text(Text&& dying) noexcept
    : m_memblock(std::exchange(dying.m_memblock, &TextRefCounter::m_s_empty)), m_start(std::exchange(dying.m_start, 0)),
      m_end(std::exchange(dying.m_end, 0)) {}

Text& Text::operator=(const Text& value) {
  if (this != &value) {
    clear();
    if (value.size() > 0) {
      m_memblock = value.m_memblock->acquire();
      m_start = value.m_start;
      m_end = value.m_end;
    }
  }
  return *this;
}

Text& Text::operator=(Text&& dying) noexcept {
  clear();
  if (dying.size() > 0) {
    m_memblock = std::exchange(dying.m_memblock, &TextRefCounter::m_s_empty);
    m_start = std::exchange(dying.m_start, 0);
    m_end = std::exchange(dying.m_end, 0);
  }
  return *this;
}

Text::Text(char c) {
  m_memblock = TextRefCounter::allocate(sizeof(c));
  m_memblock->text_data()[0] = c;
  m_end = sizeof(c);
}

Text::Text(const std::string& s) {
  m_start = 0;
  m_end = s.size();
  if (m_end > 0) {
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(s.begin(), s.end(), m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::m_s_empty;
  }
}

Text::Text(const char* ptr) {
  if ((ptr != nullptr) && (*ptr != '\0')) {
    m_end = std::strlen(ptr);
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(ptr, ptr + m_end, m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::m_s_empty;
  }
}

Text::Text(const char* ptr, size_t size) {
  if ((ptr != nullptr) && size > 0) {
    m_end = size;
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(ptr, ptr + m_end, m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::m_s_empty;
  }
}

Text::Text(const Text& t, size_t start, size_t length) {
  auto real_start = std::min(t.m_start + start, t.m_end);
  auto real_end = std::min(real_start + length, t.m_end);
  if (real_start == real_end) {
    m_memblock = &TextRefCounter::m_s_empty;
  } else {
    m_start = real_start;
    m_end = real_end;
    m_memblock = t.m_memblock->acquire();
  }
}

Text::Text(TextRefCounter* buffer, size_t length) : m_memblock(buffer), m_end(length) {}

void Text::clear() {
  TextRefCounter::release(m_memblock);
  m_memblock = &TextRefCounter::m_s_empty;
  m_start = 0;
  m_end = 0;
}

Text Text::copy() const { return {m_memblock->text_data() + m_start, size()}; }

char Text::operator[](ssize_t index) const {
  if (index >= static_cast<ssize_t>(size()) || (-index > static_cast<ssize_t>(size()))) [[unlikely]] {
    throw std::out_of_range(std::format("Requested index {} out of {}", index, size()));
  }
  return *(m_memblock->text_data() + m_start + index + ((index < 0) ? size() : 0));
}

std::strong_ordering Text::operator<=>(const Text& v) const {
  const char* p = begin();
  const char* e = end();
  const char* vp = v.begin();
  const char* ve = v.end();
  while (p != e && vp != ve) {
    auto res = *p <=> *vp;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    vp++;
  }
  return size() <=> v.size();
}

std::strong_ordering Text::operator<=>(const char* v) const {
  if (v == nullptr) [[unlikely]] {
    return size() <=> 0;
  }
  const char* p = begin();
  const char* e = end();
  while (p != e) {
    auto res = *p <=> *v;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    v++;
  }
  return *v == 0 ? std::strong_ordering::equal : std::strong_ordering::less;
}

std::strong_ordering Text::operator<=>(const std::string& v) const {
  const char* p = begin();
  const char* e = end();
  auto vp = v.begin();
  auto ve = v.end();
  while (p != e && vp != ve) {
    auto res = *p <=> *vp;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    vp++;
  }
  return size() <=> v.size();
}

bool Text::operator==(const Text& tv) const {
  if (tv.size() != size()) {
    return false;
  }
  return operator<=>(tv) == std::strong_ordering::equal;
}

bool Text::operator==(const char* v) const { return operator<=>(v) == std::strong_ordering::equal; }

bool Text::operator==(const std::string& v) const {
  if (v.size() != size()) {
    return false;
  }
  return operator<=>(v) == std::strong_ordering::equal;
}

size_t Text::size() const { return m_end - m_start; }
const char* Text::begin() const { return m_memblock->text_data() + m_start; }
const char* Text::end() const { return m_memblock->text_data() + m_end; }

std::string Text::to_string() const { return std::string(begin(), size()); }
std::string_view Text::to_view() const { return std::string_view(begin(), size()); }
TextView Text::to_text_view() const { return TextView(to_view()); }
std::span<uint8_t> Text::to_raw_data() const {
  auto* st = reinterpret_cast<uint8_t*>(const_cast<char*>(begin()));
  auto* en = reinterpret_cast<uint8_t*>(const_cast<char*>(end()));
  return std::span<uint8_t>{st, en};
}
int64_t Text::to_int() const { return std::stoll(to_string()); }

Text Text::trim() const { return trim_left().trim_right(); }
Text Text::trim_left() const { return skip(WHITESPACE); }
Text Text::trim_right() const {
  auto position = to_view().find_last_not_of(WHITESPACE);
  if (position == std::string_view::npos) {
    return {};
  }
  return Text(*this, 0, position + 1);
}

bool Text::starts_with(const Text& tv) const { return to_view().starts_with(tv.to_view()); }
bool Text::starts_with(const char* v) const {
  if (v == nullptr) [[unlikely]] {
    return false;
  }
  return to_view().starts_with(v);
}
bool Text::starts_with(char c) const { return size() > 0 && *begin() == c; }

bool Text::ends_with(const Text& tv) const { return to_view().ends_with(tv.to_view()); }
bool Text::ends_with(char c) const {
  if (size() > 0) {
    return c == *(begin() + size() - 1);
  }
  return false;
}

bool Text::contains(char c) const { return to_text_view().contains(c); }

Text Text::skip(std::string_view skippables) const { return skip(to_view().find_first_not_of(skippables)); }

Text Text::skip(size_t n) const {
  if (n < size()) {
    return sublen(n, m_end - n);
  };
  return {};
}

std::optional<size_t> Text::pos(char c, size_t occurence) const { return to_text_view().pos(c, occurence); }

std::optional<size_t> Text::pos(const Text& t, size_t occurence) const {
  return to_text_view().pos(t.to_text_view(), occurence);
}

std::optional<size_t> Text::last_pos(char c) const { return to_text_view().last_pos(c); }

std::pair<Text, Text> Text::split_pos(ssize_t where) const {
  auto maxn = static_cast<ssize_t>(size());
  if (where < 0) {
    where += maxn;
    if (where < 0) {
      where = 0;
    }
  }
  return std::pair<Text, Text>{sublen(0, where), sublen(where, maxn)};
}

std::pair<Text, Text> Text::split_next_char(char c, SplitDirection direction) const {
  auto position = pos(c, 1);
  if (!position.has_value()) {
    return std::pair<Text, Text>{*this, {}};
  }
  if (direction == SplitDirection::Discard) {
    return std::pair<Text, Text>{sublen(0, position.value()), sublen(position.value() + 1, m_end)};
  }
  auto split_position = position.value() + ((direction == SplitDirection::KeepLeft) ? 1 : 0);
  return std::pair<Text, Text>{sublen(0, split_position), sublen(split_position, m_end)};
}

std::pair<Text, Text> Text::split_next_line() const {
  auto [left, right] = split_next_char('\n', SplitDirection::Discard);
  if (left.size() > 0 && left[-1] == '\r') {
    return std::pair<Text, Text>{left.sublen(0, left.size() - 1), right};
  }
  return std::pair<Text, Text>{left, right};
}

List<Text> Text::split_lines(SplitEmpty on_empty) const {
  List<Text> res;
  auto right = *this;
  Text left;
  while (right.size() > 0) {
    std::tie(left, right) = right.split_next_line();
    if (left.size() > 0 || on_empty == SplitEmpty::Keep) {
      res.add(left);
    }
  }
  if (on_empty == SplitEmpty::Keep && (size() == 0 || operator[](-1) == '\n')) {
    res.add(Text{});
  }
  return res;
}

List<Text> Text::split_by_char(char c, SplitEmpty on_empty) const {
  List<Text> res;
  auto right = *this;
  Text left;
  while (right.size() > 0) {
    std::tie(left, right) = right.split_next_char(c, SplitDirection::Discard);
    if (left.size() > 0 || on_empty == SplitEmpty::Keep) {
      res.add(left);
    }
  }
  if (on_empty == SplitEmpty::Keep && (size() == 0 || operator[](-1) == c)) {
    res.add(Text{});
  }

  return res;
}

List<Text> Text::split_by_text(const Text& t, SplitEmpty on_empty) const {
  if (t.size() == 0) {
    return {*this};
  }
  List<Text> res;
  auto position = pos(t);
  auto left_over = *this;
  while (position.has_value()) {
    auto pos_value = position.value(); // NOLINT(bugprone-unchecked-optional-access)
    if (pos_value > 0 || on_empty != SplitEmpty::Discard) {
      res.add(left_over.sublen(0, pos_value));
    }
    left_over = left_over.sublen(pos_value + t.size(), left_over.size());
    position = left_over.pos(t);
  }
  if (left_over.size() > 0 || on_empty != SplitEmpty::Discard) {
    res.add(left_over);
  }
  return res;
}

Text Text::subpos(size_t start, size_t end) const {
  if (start >= size() || end < start) {
    return {};
  }
  end++; // include the ending character
  if (end > size()) {
    end = size();
  }
  return Text(*this, start, end - start);
}

Text Text::sublen(size_t start, size_t len) const {
  if (start >= size()) {
    return Text();
  }
  return Text(*this, start, len);
}

std::optional<Text> Text::expect(const Text& t) const {
  if (starts_with(t)) {
    return skip(t.size());
  }
  return {};
}

std::optional<Text> Text::expect_ws(const Text& t) const { return trim_left().expect(t); }

std::optional<Text> Text::skip_indent(size_t indentLevel) const {
  if (size() < indentLevel) [[unlikely]] {
    return {};
  }
  auto ptr = begin();
  for (size_t i = 0; i < indentLevel; i++) {
    if (*ptr != ' ') {
      return {};
    }
    ptr++;
  }
  return skip(indentLevel);
}

size_t Text::get_indent() const {
  size_t level = 0;
  for (const char c: *this) {
    if (c == ' ') {
      level++;
    } else {
      return level;
    }
  }
  return level;
}

void Text::fill_c_buffer(char* dest, size_t bufsize) const {
  if (bufsize == 0) {
    return;
  }
  const size_t amount_to_copy = bufsize <= size() ? bufsize - 1 : size();
  std::copy(begin(), begin() + amount_to_copy, dest);
  dest[amount_to_copy] = 0;
}

size_t Text::count(char c) const {
  return std::accumulate(begin(), end(), 0UZ, [c](size_t count, char ch) { return count + (ch == c ? 1 : 0); });
}

size_t Text::count(Text t) const {
  return std::accumulate(begin(), end(), 0UZ, [&t](size_t count, char c) { return count + (t.contains(c) ? 1 : 0); });
}

const Text QuoteAndBackslash("\"\\");
const Text Quote(QuoteAndBackslash.sublen(0, 1));

Text Text::quote_escaped() const {
  const auto escapes = count(QuoteAndBackslash);
  if (escapes == 0) {
    return TextChain{Quote, *this, Quote};
  }
  const size_t length = escapes + size() + 2;
  auto memblock = TextRefCounter::allocate(length);
  auto ptr = memblock->text_data();
  ptr[0] = '"';
  size_t offset = 1;

  for (const char c: *this) {
    if (c == '"' || c == '\\') {
      ptr[offset++] = '\\';
    }
    ptr[offset++] = c;
  }
  ptr[length - 1] = '"';
  return Text(memblock, length);
}

void TextChain::update_length() {
  m_length = 0;
  for (const auto& t: m_chain) {
    m_length += t.size();
  }
}

TextChain::TextChain(std::initializer_list<Text> l) : m_chain(l) { update_length(); }
TextChain::TextChain(List<Text>&& l) : m_chain(l) { update_length(); }
TextChain::TextChain(const List<Text>& l) : m_chain(l) { update_length(); }
const List<Text>& TextChain::chain() const { return m_chain; }
void TextChain::operator+=(const Text& text) {
  m_chain.add(text);
  m_length += text.size();
}

Text TextChain::to_text() const {
  if (m_chain.size() == 0) {
    return {};
  }
  if (m_chain.size() == 1) {
    return m_chain[0];
  }

  auto memblock = TextRefCounter::allocate(m_length);
  char* ptr = memblock->text_data();
  uint32_t offset = 0;
  for (const auto& tv: m_chain) {
    std::copy(tv.begin(), tv.end(), ptr + offset);
    offset += tv.size();
  }
  return Text(memblock, m_length);
}

Text TextChain::join(char splitchar) const {
  if (m_chain.size() == 0) {
    return ""_t;
  }
  if (m_chain.size() == 1) {
    return m_chain[0];
  }
  const size_t size = m_length + (splitchar != '\0' ? (m_chain.size() - 1) : 0);

  auto memblock = TextRefCounter::allocate(size);
  char* ptr = memblock->text_data();

  size_t offset = 0;
  bool split_element = false;
  for (const auto& t: m_chain) {
    if (splitchar != '\0' && split_element) {
      ptr[offset] = splitchar;
      offset++;
    }
    split_element = true;
    std::copy(t.begin(), t.end(), ptr + offset);
    offset += t.size();
  }
  return Text(memblock, size);
}

kl::Text TextChain::join(const kl::Text& split_text, const kl::Text& prefix, const kl::Text& suffix) const {
  if (m_chain.size() == 0) {
    return ""_t;
  }
  if (m_chain.size() == 1) {
    return m_chain[0];
  }
  const size_t size = m_length + split_text.size() * (m_chain.size() - 1) + prefix.size() + suffix.size();

  auto memblock = TextRefCounter::allocate(size);
  char* ptr = memblock->text_data();

  bool split_element = false;
  const size_t split_size = split_text.size();
  std::copy(prefix.begin(), prefix.end(), ptr);
  size_t offset = prefix.size();
  for (const auto& t: m_chain) {
    if (split_size > 0 && split_element) {
      std::copy(split_text.begin(), split_text.end(), ptr + offset);
      offset += split_size;
    }
    split_element = true;
    std::copy(t.begin(), t.end(), ptr + offset);
    offset += t.size();
  }
  std::copy(suffix.begin(), suffix.end(), ptr + offset);
  return Text(memblock, size);
}

void TextChain::add(const Text& text) {
  m_chain.add(text);
  m_length += text.size();
};

void TextChain::operator+=(const TextChain& text) {
  m_chain.add(text.m_chain);
  m_length += text.m_length;
}

void TextChain::add(const TextChain& text) {
  m_chain.add(text.m_chain);
  m_length += text.m_length;
};

TextChain::operator Text() const { return to_text(); }
void TextChain::clear() {
  m_chain = {};
  m_length = 0;
}

Text Text::skip_bom() const {
  if (size() >= 3) {
    const auto* buf = begin();
    // BOM is 0xEFBBBF for UTF8
    if (buf[0] == '\xEF' && buf[1] == '\xBB' && buf[2] == '\xBF') {
      return skip(3);
    }
  }
  return *this;
}

inline namespace literals {
Text operator"" _t(const char* p, size_t s) { return {p, s}; }
} // namespace literals

size_t TextChain::size() const { return m_length; }

std::ostream& operator<<(std::ostream& os, const kl::TextView& tv) { return os << tv.view(); }
std::ostream& operator<<(std::ostream& os, const kl::Text& t) { return os << t.to_view(); }
std::ostream& operator<<(std::ostream& os, const kl::TextChain& tc) { return os << std::format("{}", tc); }

} // namespace kl

std::size_t std::hash<kl::Text>::operator()(const kl::Text& s) const noexcept {
  return std::hash<std::string_view>{}(s.to_view());
}

kl::TextChain operator+(const kl::Text& t, const char* p) {
  kl::TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}

kl::TextChain operator+(const kl::Text& t1, const kl::Text& t2) {
  kl::TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

kl::TextChain operator+(const kl::TextChain& t1, const kl::Text& t2) {
  kl::TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

kl::TextChain operator+(const kl::TextChain& t, const char* p) {
  kl::TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}
