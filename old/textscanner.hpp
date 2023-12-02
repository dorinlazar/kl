#pragma once
#include <exception>
#include <kl/kltext.hpp>

namespace kl {
class ParsingError : public std::logic_error {
  uint32_t m_line;
  uint32_t m_column;

public:
  ParsingError(const Text& why, uint32_t line, uint32_t column);
  uint32_t line() const;
  uint32_t column() const;
};

struct ParsedCharacter {
  char character = '\0';
  bool escaped = false;
};

enum class NewLineHandling { Skip, Keep };

class TextScanner {

public:
  explicit TextScanner(const Text& source);

  uint32_t line() const;
  uint32_t column() const;

  void skip_whitespace(NewLineHandling handling = NewLineHandling::Skip);
  bool empty() const;
  char top_char() const;
  ParsedCharacter read_char();
  ParsedCharacter read_char_escaped();
  Text read_quoted_string();
  Text read_until(char character);
  Text read_word();
  Text read_line();
  Text remainder() const;
  int32_t read_digit();
  int32_t read_fixed_int(uint32_t n_digits);
  void expect(char character);
  void expect_ws(char character, NewLineHandling handling = NewLineHandling::Keep);
  bool starts_with(const Text& what) const;
  void skip(uint32_t n_chars);
  uint32_t get_indent_level() const;

public:
  struct DataLocation {
    friend class TextScanner;

  protected:
    uint32_t m_line = 1;
    uint32_t m_column = 1;
    uint32_t m_offset = 0;
    uint32_t m_data_left = 0;
    const char* m_current;
  };

  void rewind();
  const DataLocation& location() const;
  void restore_location(const DataLocation& location);

public:
  void error(const Text& why) const;

private:
  DataLocation m_loc;
  Text m_original_source;
  void advance();
};

} // namespace kl
