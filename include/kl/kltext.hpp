#pragma once
#include "kl.hpp"
#include "klds.hpp"
#include <algorithm> // this comment is a test, really
#include <compare>
#include <cstring>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <vector>
#include <span>

// NOLINTBEGIN(google-explicit-constructor)
namespace kl {

enum class SplitEmpty { Keep, Discard };
enum class SplitDirection { KeepLeft, Discard, KeepRight };

class TextView {
public:
  TextView();
  TextView(std::string_view v);
  TextView(const char* text);
  TextView(const char* text, size_t length);
  TextView(const TextView&) = default;
  TextView(TextView&&) noexcept = default;
  TextView& operator=(const TextView&) = default;
  TextView& operator=(TextView&&) noexcept = default;
  ~TextView() = default;

public:
  TextView trim() const;
  TextView trim_left() const;
  TextView trim_right() const;

  bool starts_with(char c) const;
  bool starts_with(const TextView& tv) const;
  bool ends_with(char c) const;
  bool ends_with(const TextView& tv) const;

  char operator[](ssize_t index) const;

  size_t size() const;
  const char* begin() const;
  const char* end() const;

  std::strong_ordering operator<=>(const TextView&) const;
  std::strong_ordering operator<=>(const std::string_view&) const;
  std::strong_ordering operator<=>(const std::string&) const;
  std::strong_ordering operator<=>(const char*) const;
  bool operator==(const TextView&) const;
  bool operator==(const std::string_view&) const;
  bool operator==(const std::string&) const;
  bool operator==(const char*) const;

  operator std::string_view() const;
  std::string_view view() const;

  bool contains(char c) const;

  TextView skip(const TextView& skippables) const;
  TextView skip(size_t n) const;
  TextView skip_bom() const;

  // substring position based. The string will contain the character from ending position too.
  TextView subpos(size_t start, size_t end) const;

  // substring length based. The return value will have a string of at most <len> characters
  TextView sublen(size_t start, size_t len) const;

  // occurence is one based - so first occurence is 1;
  std::optional<size_t> pos(char c, size_t occurence = 1) const;
  std::optional<size_t> pos(const TextView& t, size_t occurence = 1) const;
  std::optional<size_t> last_pos(char c) const;

  std::pair<TextView, TextView> split_pos(ssize_t where) const;
  std::pair<TextView, std::optional<TextView>>
  split_next_char(char c, SplitDirection direction = SplitDirection::Discard) const;
  std::pair<TextView, std::optional<TextView>> split_next_line() const;
  List<TextView> split_lines(SplitEmpty on_empty = SplitEmpty::Keep) const;
  List<TextView> split_by_char(char c, SplitEmpty on_empty = SplitEmpty::Discard) const;
  List<TextView> split_by_text(const TextView& t, SplitEmpty on_empty = SplitEmpty::Discard) const;

  // returns a value that skips the starting text
  std::optional<TextView> expect(const TextView& t) const;
  // returns a value that skips the whitespace in the text
  std::optional<TextView> expect_ws(const TextView& t) const;
  // returns a value that skips the starting text
  std::optional<TextView> expect(char c) const;
  // returns a value that skips the whitespace in the text
  std::optional<TextView> expect_ws(char c) const;

  // returns text after <indentLevel> whitespaces, or empty;
  std::optional<TextView> skip_indent(size_t indentLevel) const;
  // returns whitespace indent level
  size_t get_indent() const;
  // how many times the character c appears in the text
  size_t count(char c) const;

private:
  std::string_view m_view;
};

class TextRefCounter {
  int64_t m_ref_count = 1;
  [[nodiscard]] bool release();

public:
  TextRefCounter* acquire();
  char* text_data();
  static TextRefCounter* allocate(size_t text_size);
  static void release(TextRefCounter* ref);
  static TextRefCounter m_s_empty;
};
static_assert(sizeof(TextRefCounter) == sizeof(int64_t));

class Text {
  TextRefCounter* m_memblock;
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

public:
  void clear();
  // This is useful when we don't want to keep the big source text like when we're parsing some file
  // for a small bit of information and we want to free that piece of memory.
  Text copy() const;

  char operator[](ssize_t index) const;

  std::strong_ordering operator<=>(const Text&) const;
  std::strong_ordering operator<=>(const char*) const;
  std::strong_ordering operator<=>(const std::string&) const;

  bool operator==(const Text&) const;
  bool operator==(const char*) const;
  bool operator==(const std::string&) const;

public:
  size_t size() const;
  const char* begin() const;
  const char* end() const;

  Text trim() const;
  Text trim_left() const;
  Text trim_right() const;

  bool starts_with(const Text& tv) const;
  bool starts_with(const char*) const;
  bool starts_with(char c) const;
  bool ends_with(const Text& tv) const;
  bool ends_with(char c) const;

  std::string to_string() const;
  std::string_view to_view() const;
  TextView to_text_view() const;
  std::span<uint8_t> to_raw_data() const;
  int64_t to_int() const;

  bool contains(char c) const;
  Text skip(std::string_view skippables) const;
  Text skip(size_t n) const;
  Text skip_bom() const;

  // substring position based. The string will contain the character from ending position too.
  Text subpos(size_t start, size_t end) const;

  // substring length based. The return value will have a string of at most <len> characters
  Text sublen(size_t start, size_t len) const;

  // occurence is one based - so first occurence is 1;
  std::optional<size_t> pos(char c, size_t occurence = 1) const;
  std::optional<size_t> pos(const Text& t, size_t occurence = 1) const;
  std::optional<size_t> last_pos(char c) const;

  std::pair<Text, Text> split_pos(ssize_t where) const;
  std::pair<Text, Text> split_next_char(char c, SplitDirection direction = SplitDirection::Discard) const;
  std::pair<Text, Text> split_next_line() const;
  List<Text> split_lines(SplitEmpty on_empty = SplitEmpty::Keep) const;
  List<Text> split_by_char(char c, SplitEmpty on_empty = SplitEmpty::Discard) const;
  List<Text> split_by_text(const Text& t, SplitEmpty on_empty = SplitEmpty::Discard) const;

  // returns a value that skips the starting text
  std::optional<Text> expect(const Text& t) const;
  // returns a value that skips the whitespace in the text
  std::optional<Text> expect_ws(const Text& t) const;

  // returns text after <indentLevel> whitespaces, or empty;
  std::optional<Text> skip_indent(size_t indentLevel) const;
  // returns whitespace indent level
  size_t get_indent() const;

  // fills a C buffer, preallocated with bufsize bytes;
  void fill_c_buffer(char* dest, size_t bufsize) const;
  // how many times the character c appears in the text
  size_t count(char c) const;
  size_t count(Text t) const;

  // Quote escaped: x="He\llo" becomes "x=\"He\\llo\""
  Text quote_escaped() const;
};

class TextChain final {
  uint32_t m_length = 0;
  List<Text> m_chain;

  void update_length();

public:
  TextChain() = default;
  TextChain(const TextChain&) = default;
  TextChain(TextChain&&) = default;
  TextChain& operator=(const TextChain&) = default;
  TextChain& operator=(TextChain&&) = default;
  TextChain(std::initializer_list<Text> l);
  TextChain(List<Text>&& l);
  TextChain(const List<Text>& l);
  ~TextChain() = default;

  void operator+=(const Text& text);
  void add(const Text& text);
  void operator+=(const TextChain& text);
  void add(const TextChain& text);

  Text to_text() const;
  operator Text() const;
  size_t size() const;

  const List<Text>& chain() const;

  void clear();
  kl::Text join(char splitchar = '\0') const;
  kl::Text join(const kl::Text& split_text, const kl::Text& prefix = {}, const kl::Text& suffix = {}) const;
};

inline namespace literals {
kl::Text operator"" _t(const char* p, size_t s);
kl::TextView operator"" _tv(const char* p, size_t s);
} // namespace literals

std::ostream& operator<<(std::ostream& os, const kl::TextView& tv);
std::ostream& operator<<(std::ostream& os, const kl::Text& t);
std::ostream& operator<<(std::ostream& os, const kl::TextChain& tc);

} // namespace kl

template <>
struct std::hash<kl::Text> {
  std::size_t operator()(const kl::Text& s) const noexcept;
};

template <>
struct std::formatter<kl::TextView> : public std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(const kl::TextView& c, FormatContext& ctx) const {
    return std::formatter<std::string_view>::format(c.view(), ctx);
  }
};

template <>
struct std::formatter<kl::Text> : public std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(const kl::Text& c, FormatContext& ctx) const {
    return std::formatter<std::string_view>::format(c.to_view(), ctx);
  }
};

template <>
struct std::formatter<kl::TextChain> : public std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(const kl::TextChain& tc, FormatContext& ctx) const {
    const kl::Text split{R"("}, {")"};
    const kl::Text prefix{R"({")"};
    const kl::Text suffix{R"("})"};
    return std::formatter<std::string_view>::format(tc.join(split, prefix, suffix).to_view(), ctx);
  }
};

kl::TextChain operator+(const kl::Text&, const char*);
kl::TextChain operator+(const kl::Text&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const char*);
// NOLINTEND(google-explicit-constructor)
