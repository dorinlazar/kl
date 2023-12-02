#include "textscanner.hpp"

namespace kl {
ParsingError::ParsingError(const Text& why, uint32_t line, uint32_t column)
    : std::logic_error((why + "@"_t + std::to_string(line) + ":" + std::to_string(column)).to_text().to_string()),
      m_line(line), m_column(column) {}

uint32_t ParsingError::line() const { return m_line; }
uint32_t ParsingError::column() const { return m_column; }

TextScanner::TextScanner(const Text& source) : m_original_source(source) {
  m_loc.m_current = source.begin();
  m_loc.m_data_left = source.size();
}

void TextScanner::rewind() {
  m_loc.m_line = 1;
  m_loc.m_column = 1;
  m_loc.m_offset = 0;
  m_loc.m_data_left = m_original_source.size();
  m_loc.m_current = m_original_source.begin();
}

uint32_t TextScanner::line() const { return m_loc.m_line; }
uint32_t TextScanner::column() const { return m_loc.m_column; }

void TextScanner::skip_whitespace(NewLineHandling handling) {
  const Text spaces = handling == NewLineHandling::Skip ? " \t\n\r"_t : " \t"_t;
  while (m_loc.m_current < m_original_source.end()) {
    const char c = *m_loc.m_current;
    if (!spaces.contains(c)) {
      return;
    }
    advance();
  }
}

void TextScanner::advance() {
  if (m_loc.m_data_left == 0) [[unlikely]] {
    error("Trying to advance beyond end of text");
  }
  if (*m_loc.m_current == '\n') {
    m_loc.m_line++;
    m_loc.m_column = 1;
  } else {
    m_loc.m_column++;
  }
  m_loc.m_data_left--;
  m_loc.m_current++;
  m_loc.m_offset++;
}

bool TextScanner::empty() const { return m_loc.m_data_left == 0; }

char TextScanner::top_char() const {
  if (empty()) [[unlikely]] {
    error("Requesting data from empty container");
  }
  return *m_loc.m_current;
}

ParsedCharacter TextScanner::read_char() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  ParsedCharacter result{.character = *m_loc.m_current};
  advance();
  return result;
}

ParsedCharacter TextScanner::read_char_escaped() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  ParsedCharacter result{.character = *m_loc.m_current};
  advance();
  if (result.character == '\\') {
    if (empty()) {
      error("Reading from empty data mid-escape");
    }
    result.escaped = true;
    switch (*m_loc.m_current) {
    case 'r': result.character = '\r'; break;
    case 'n': [[fallthrough]];
    case '\n': result.character = '\n'; break;
    case 't': result.character = '\t'; break;
    case '0': result.character = '\0'; break;
    case '\\': result.character = '\\'; break;
    case '"': result.character = '"'; break;
    case '\'': result.character = '\''; break;
    default: error("Invalid escape character");
    }
    advance();
  }
  return result;
}

Text TextScanner::read_quoted_string() {
  if (empty() || *m_loc.m_current != '"') [[unlikely]] {
    error("Unexpected character");
  }
  advance();
  TextChain tc;
  auto start_offset = m_loc.m_offset;
  while (true) {
    auto last_offset = m_loc.m_offset;
    auto ch = read_char_escaped();
    if (ch.escaped) {
      tc.add(m_original_source.subpos(start_offset, last_offset - 1));
      tc.add(Text(&ch.character, 1));
      start_offset = m_loc.m_offset;
    } else if (ch.character == '"') {
      tc.add(m_original_source.subpos(start_offset, last_offset - 1));
      break;
    }
  }
  return tc.to_text();
}

Text TextScanner::read_until(char character) {
  auto start_offset = m_loc.m_offset;
  while (true) {
    auto ch = read_char();
    if (ch.character == character) {
      return m_original_source.subpos(start_offset, m_loc.m_offset - 2);
    }
  }
}

Text TextScanner::read_word() {
  auto start = m_loc.m_offset;
  while (!empty()) {
    const char c = *m_loc.m_current;
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_' || c == '-')) {
      advance();
    } else {
      break;
    }
  }
  return m_original_source.subpos(start, m_loc.m_offset - 1);
}

// TODO(dorin) make this smarter, to support the \rs as well
Text TextScanner::read_line() {
  auto start = m_loc.m_offset;
  while (!empty()) {
    if (*m_loc.m_current == '\n') {
      break;
    }
    advance();
  }
  auto res = m_original_source.subpos(start, m_loc.m_offset - 1);
  if (!empty()) {
    advance();
  }
  return res;
}

Text TextScanner::remainder() const {
  if (empty()) [[unlikely]] {
    return ""_t;
  }
  return m_original_source.skip(m_loc.m_offset);
}

void TextScanner::expect(char character) {
  auto ch = top_char();
  if (ch != character) {
    error("Unexpected character: "_t + Text(ch) + " vs: " + Text(character));
  }
  advance();
}

void TextScanner::expect_ws(char character, NewLineHandling handling) {
  skip_whitespace(handling);
  expect(character);
}

bool TextScanner::starts_with(const Text& what) const { return remainder().starts_with(what); }
void TextScanner::skip(uint32_t n_chars) {
  const uint32_t reasonably_small_value = 32;
  if (n_chars < reasonably_small_value) {
    for (uint32_t i = 0; i < n_chars; i++) {
      advance();
    }
  } else {
    const auto sub = remainder().sublen(0, n_chars);
    const auto n_lines = sub.count('\n');
    m_loc.m_offset += sub.size();
    m_loc.m_current += sub.size();
    m_loc.m_data_left -= sub.size();
    if (n_lines == 0) {
      m_loc.m_column += sub.size();
    } else {
      m_loc.m_line += n_lines;
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      m_loc.m_column = 1 + (sub.size() - *sub.last_pos('\n'));
    }
  }
}

void TextScanner::error(const Text& why) const { throw ParsingError(why, m_loc.m_line, m_loc.m_column); }

const TextScanner::DataLocation& TextScanner::location() const { return m_loc; }
void TextScanner::restore_location(const TextScanner::DataLocation& location) { m_loc = location; }

uint32_t TextScanner::get_indent_level() const {
  for (uint32_t i = 0; i < m_loc.m_data_left; i++) {
    if (m_loc.m_current[i] != ' ') {
      return i;
    }
  }
  return m_loc.m_data_left;
}

int32_t TextScanner::read_digit() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  const char c = *m_loc.m_current;
  if (c < '0' || c > '9') [[unlikely]] {
    error("Expected a digit");
  }
  advance();
  return c - '0';
}

int32_t TextScanner::read_fixed_int(uint32_t n_digits) {
  int32_t cumulated = 0;
  const uint32_t digits_base = 10;
  for (uint32_t index = 0; index < n_digits; index++) {
    cumulated *= digits_base;
    cumulated += read_digit();
  }
  return cumulated;
}

} // namespace kl
