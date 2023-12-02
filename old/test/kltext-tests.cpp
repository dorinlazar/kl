#include <kl/kltext.hpp>
#include <unordered_map>
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltext, test_text_construction) {
  Text txt("Hello world");
  EXPECT_EQ(txt.to_string(), "Hello world"s);

  Text txt2("Hello world"s);
  EXPECT_EQ(txt2.to_string(), "Hello world"s);

  Text txt3;
  txt3 = txt2;
  EXPECT_EQ(txt3.to_string(), "Hello world"s);

  Text txt4('h');
  EXPECT_EQ(txt4, "h"_t);

  auto t2 = Text(txt, 4, 20);
  EXPECT_EQ(t2.to_string(), "o world"s);
  EXPECT_EQ(t2.size(), 7);

  Text tnull(nullptr, 2, 4);
  EXPECT_EQ(tnull.size(), 0);
  EXPECT_EQ(tnull.to_string(), ""s);
  auto tn2 = Text(tnull, 2, 4);
  EXPECT_EQ(tn2.size(), 0);
  EXPECT_EQ(tn2.to_string(), ""s);

  Text tempty(""s);
  EXPECT_EQ(tempty.size(), 0);
  EXPECT_EQ(tempty.to_string(), ""s);

  Text t1("Hello World", 2);
  EXPECT_EQ(t1.size(), 2);
  EXPECT_EQ(t1.to_string(), "He"s);

  Text t3("Hello World\0", 12);
  EXPECT_EQ(t3.size(), 12);
  EXPECT_EQ(t3[11], '\0');

  Text t4("Hello World\0Other World", 23);
  EXPECT_EQ(t4.size(), 23);
  EXPECT_EQ(t4[11], '\0');
  EXPECT_EQ(t4.subpos(12, 24), "Other World");
}

TEST(kltext, test_trimming) {
  Text txt(" \t hello   \n ");
  EXPECT_EQ(txt.trim().to_string(), "hello"s);
  EXPECT_EQ(txt.trim_left().to_string(), "hello   \n "s);
  EXPECT_EQ(txt.trim_right().to_string(), " \t hello"s);

  Text txt2("hello");
  EXPECT_EQ(txt2.trim().to_string(), "hello"s);
  EXPECT_EQ(txt2.trim_left().to_string(), "hello"s);
  EXPECT_EQ(txt2.trim_right().to_string(), "hello"s);

  Text txt3;
  EXPECT_EQ(txt3.trim().to_string(), ""s);
  EXPECT_EQ(txt3.trim_left().to_string(), ""s);
  EXPECT_EQ(txt3.trim_right().to_string(), ""s);
}

TEST(kltext, test_starts_with) {
  Text txt("Hello");
  EXPECT_FALSE(txt.starts_with(nullptr));
  EXPECT_TRUE(txt.starts_with(""));
  EXPECT_TRUE(txt.starts_with("H"));
  EXPECT_TRUE(txt.starts_with("Hell"));
  EXPECT_TRUE(txt.starts_with("Hello"));
  EXPECT_FALSE(txt.starts_with("Hello world"));
  EXPECT_FALSE(txt.starts_with("world"));

  EXPECT_TRUE(txt.starts_with(""_t));
  EXPECT_TRUE(txt.starts_with("H"_t));
  EXPECT_TRUE(txt.starts_with("Hell"_t));
  EXPECT_TRUE(txt.starts_with("Hello"_t));
  EXPECT_FALSE(txt.starts_with("Hello world"_t));
  EXPECT_FALSE(txt.starts_with("world"_t));
}

TEST(kltext, test_ends_with) {
  Text txt("Hello");
  EXPECT_TRUE(txt.ends_with(""));
  EXPECT_TRUE(txt.ends_with("o"));
  EXPECT_TRUE(txt.ends_with('o'));
  EXPECT_TRUE(txt.ends_with("ello"));
  EXPECT_TRUE(txt.ends_with("Hello"));
  EXPECT_FALSE(txt.ends_with("Hello world"));
  EXPECT_FALSE(txt.ends_with("world"));
  EXPECT_FALSE(""_t.ends_with('o'));
}

TEST(kltext, test_comparisons) {
  Text hello("hello");
  Text hfllo("hfllo");
  Text empty;

  EXPECT_EQ(hello, "hello");
  EXPECT_TRUE(hello < "hfllo");
  EXPECT_TRUE(hello <= "hfllo");
  EXPECT_TRUE(hello != "hfllo");
  EXPECT_TRUE(hello != "hell");
  EXPECT_FALSE(hello == "hell");
  EXPECT_TRUE(hello >= "hallo");
  EXPECT_TRUE(hello >= "ahllo");
  EXPECT_TRUE(hello > "hallo");
  EXPECT_TRUE(hello > "hell");
  EXPECT_TRUE(hello < "hello world");
  EXPECT_TRUE(hello > "");
  EXPECT_TRUE(hello > nullptr);

  EXPECT_EQ(hello, "hello"s);
  EXPECT_TRUE(hello < "hfllo"s);
  EXPECT_TRUE(hello <= "hfllo"s);
  EXPECT_TRUE(hello != "hfllo"s);
  EXPECT_TRUE(hello != "hell"s);
  EXPECT_FALSE(hello == "hell"s);
  EXPECT_TRUE(hello >= "hallo"s);
  EXPECT_TRUE(hello >= "ahllo"s);
  EXPECT_TRUE(hello > "hallo"s);
  EXPECT_TRUE(hello > "hell"s);
  EXPECT_TRUE(hello < "hello world"s);
  EXPECT_TRUE(hello > ""s);

  EXPECT_EQ(hello, "hello"_t);
  EXPECT_TRUE(hello < "hfllo"_t);
  EXPECT_TRUE(hello <= "hfllo"_t);
  EXPECT_TRUE(hello != "hfllo"_t);
  EXPECT_TRUE(hello != "hell"_t);
  EXPECT_FALSE(hello == "hell"_t);
  EXPECT_TRUE(hello >= "hallo"_t);
  EXPECT_TRUE(hello >= "ahllo"_t);
  EXPECT_TRUE(hello > "hallo"_t);
  EXPECT_TRUE(hello > "hell"_t);
  EXPECT_TRUE(hello < "hello world"_t);
  EXPECT_TRUE(hello > ""_t);
}

TEST(kltext, test_split) {
  Text txt("hello");
  EXPECT_EQ(get<0>(txt.split_pos(0)), "");
  EXPECT_EQ(get<1>(txt.split_pos(0)), "hello");
  EXPECT_EQ(get<0>(txt.split_pos(1)), "h");
  EXPECT_EQ(get<1>(txt.split_pos(1)), "ello");
  EXPECT_EQ(get<0>(txt.split_pos(2)), "he");
  EXPECT_EQ(get<1>(txt.split_pos(2)), "llo");
  EXPECT_EQ(get<0>(txt.split_pos(-2)), "hel");
  EXPECT_EQ(get<1>(txt.split_pos(-2)), "lo");
  EXPECT_EQ(get<0>(txt.split_pos(-1)), "hell");
  EXPECT_EQ(get<1>(txt.split_pos(-1)), "o");

  EXPECT_EQ(get<0>(txt.split_pos(-5)), "");
  EXPECT_EQ(get<1>(txt.split_pos(-5)), "hello");
  EXPECT_EQ(get<0>(txt.split_pos(-10)), "");
  EXPECT_EQ(get<1>(txt.split_pos(-10)), "hello");

  EXPECT_EQ(get<0>(txt.split_pos(5)), "hello");
  EXPECT_EQ(get<1>(txt.split_pos(5)), "");
  EXPECT_EQ(get<0>(txt.split_pos(15)), "hello");
  EXPECT_EQ(get<1>(txt.split_pos(15)), "");
  EXPECT_EQ(get<0>(""_t.split_pos(15)), "");
  EXPECT_EQ(get<1>(""_t.split_pos(15)), "");
}

TEST(kltext, test_contains) {
  Text t("hello");
  EXPECT_TRUE(t.contains('h'));
  EXPECT_TRUE(t.contains('e'));
  EXPECT_TRUE(t.contains('o'));
  EXPECT_TRUE(t.contains('l'));
  EXPECT_FALSE(t.contains('a'));
  EXPECT_FALSE(t.contains('\0'));
  EXPECT_FALSE(""_t.contains('\0'));
}

TEST(kltext, test_skip) {
  Text t("hello");
  EXPECT_EQ(t.skip("hel"), "o");
  EXPECT_EQ(t.skip("el"), "hello");
  EXPECT_EQ(t.skip(""), "hello");
  EXPECT_EQ(t.skip("abcdefghijklmnopqrstuvwxyz"), "");
  EXPECT_EQ(""_t.skip("abcdefghijklmnopqrstuvwxyz"), "");

  EXPECT_EQ(t.skip(2), "llo"_t);
  EXPECT_EQ(t.skip(1), "ello"_t);
  EXPECT_EQ(t.skip(4), "o"_t);
  EXPECT_EQ(t.skip(5), ""_t);
  EXPECT_EQ(t.skip(10), ""_t);
  EXPECT_EQ(t.skip(100), ""_t);

  EXPECT_EQ(t.skip_bom(), "hello");
  Text bom_hello("\xEF\xBB\xBF"
                 "bomhello");
  EXPECT_EQ(bom_hello.skip_bom(), "bomhello"_t);
  Text bom("\xEF\xBB\xBF");
  EXPECT_EQ(bom.skip_bom(), ""_t);
  Text bo("\xEF\xBB");
  EXPECT_EQ(bo.skip_bom(), "\xEF\xBB"_t);
}

TEST(kltext, test_pos) {
  Text t("hello world");
  EXPECT_EQ(t.pos('l'), 2);
  EXPECT_EQ(t.pos('l', 2), 3);
  EXPECT_EQ(t.pos('l', 3), 9);
  EXPECT_EQ(t.pos('d'), 10);
  EXPECT_EQ(t.pos(' '), 5);
  EXPECT_FALSE(t.pos('o', 3).has_value());
  EXPECT_FALSE(t.pos('k').has_value());

  EXPECT_FALSE(""_t.pos('x').has_value());

  EXPECT_EQ(t.pos("l"_t), 2);
  EXPECT_EQ(t.pos("ll"_t), 2);
  EXPECT_EQ(t.pos("l"_t, 2), 3);
  EXPECT_EQ(t.pos("l"_t, 3), 9);
  EXPECT_EQ(t.pos("o w"_t), 4);
  EXPECT_EQ(t.pos("d"_t), 10);
  EXPECT_FALSE(t.pos("o W"_t).has_value());
  EXPECT_FALSE(t.pos(""_t).has_value());
  EXPECT_FALSE(t.pos("hello world!"_t).has_value());
  EXPECT_EQ(t.pos("hello w"_t), 0);
  EXPECT_EQ(t.pos("hello world"_t), 0);

  EXPECT_FALSE(""_t.pos("x"_t).has_value());
  EXPECT_FALSE(""_t.pos(""_t).has_value());
}

TEST(kltext, test_split_char) {
  Text t1("My,CSV,text,with,,an,ending,,");
  auto sp1 = t1.split_by_char(',');

  EXPECT_EQ(sp1.size(), 6);
  EXPECT_EQ(sp1[0], "My"_t);
  EXPECT_EQ(sp1[1], "CSV"_t);
  EXPECT_EQ(sp1[2], "text"_t);
  EXPECT_EQ(sp1[3], "with"_t);
  EXPECT_EQ(sp1[4], "an"_t);
  EXPECT_EQ(sp1[5], "ending"_t);

  auto sp2 = t1.split_by_char(',', SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], "My"_t);
  EXPECT_EQ(sp2[1], "CSV"_t);
  EXPECT_EQ(sp2[2], "text"_t);
  EXPECT_EQ(sp2[3], "with"_t);
  EXPECT_EQ(sp2[4], ""_t);
  EXPECT_EQ(sp2[5], "an"_t);
  EXPECT_EQ(sp2[6], "ending"_t);
  EXPECT_EQ(sp2[7], ""_t);
  EXPECT_EQ(sp2[8], ""_t);

  auto sp3 = "Hello World"_t.split_by_char(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.split_by_char(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.split_by_char(' ', SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 5);
  EXPECT_EQ(sp3[0], ""_t);
  EXPECT_EQ(sp3[1], ""_t);
  EXPECT_EQ(sp3[2], ""_t);
  EXPECT_EQ(sp3[3], "Hello"_t);
  EXPECT_EQ(sp3[4], "World"_t);

  auto sp4 = "Hello_World"_t.split_by_char(' ');
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_t);
}

TEST(kltext, test_split_text) {
  Text t1("This||is||||||some||text||||");
  auto sp1 = t1.split_by_text("||"_t);

  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);

  auto sp2 = t1.split_by_text("||"_t, SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 8);
  EXPECT_EQ(sp2[0], "This"_t);
  EXPECT_EQ(sp2[1], "is"_t);
  EXPECT_EQ(sp2[2], ""_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "some"_t);
  EXPECT_EQ(sp2[5], "text"_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], ""_t);

  auto sp3 = "Hello World"_t.split_by_text(" "_t);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.split_by_text(" "_t);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World "_t.split_by_text(" "_t, SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 6);
  EXPECT_EQ(sp3[0], ""_t);
  EXPECT_EQ(sp3[1], ""_t);
  EXPECT_EQ(sp3[2], ""_t);
  EXPECT_EQ(sp3[3], "Hello"_t);
  EXPECT_EQ(sp3[4], "World"_t);
  EXPECT_EQ(sp3[5], ""_t);

  auto sp4 = "Hello_World"_t.split_by_text(" "_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_t);

  sp4 = "Hello"_t.split_by_text("HelloWorld"_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_t);

  sp4 = "Hello"_t.split_by_text("Hello"_t);
  EXPECT_EQ(sp4.size(), 0);

  sp4 = "Hello"_t.split_by_text(""_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_t);

  sp4 = "ttttt"_t.split_by_text("tt"_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "t"_t);
}

TEST(kltext, test_split_next_char) {
  auto sp1 = "This is some bad text  "_t;
  auto [first, next] = sp1.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "This ");
  EXPECT_EQ(next, "is some bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "is ");
  EXPECT_EQ(next, "some bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "some ");
  EXPECT_EQ(next, "bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "bad ");
  EXPECT_EQ(next, "text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "text ");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, " ");
  EXPECT_EQ(next.size(), 0);

  std::tie(first, next) = sp1.split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, " is some bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, " is some bad text  ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, " some bad text  ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, " bad text  ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, " text  ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, "  ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = sp1.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, "is some bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, "some bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, "bad text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, "text  ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");
}

TEST(kltext, test_split_next_line) {
  auto sp1 = "This\nis\r\nsome\r\nbad\ntext\n\n"_t;
  auto [first, next] = sp1.split_next_line();
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, "is\r\nsome\r\nbad\ntext\n\n");
  std::tie(first, next) = next.split_next_line();
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, "some\r\nbad\ntext\n\n");
  std::tie(first, next) = next.split_next_line();
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, "bad\ntext\n\n");
  std::tie(first, next) = next.split_next_line();
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, "text\n\n");
  std::tie(first, next) = next.split_next_line();
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, "\n");
  std::tie(first, next) = next.split_next_line();
  EXPECT_EQ(first.size(), 0);
  EXPECT_EQ(next.size(), 0);

  auto sp2 = "A bad example"_t;
  std::tie(first, next) = sp2.split_next_line();
  EXPECT_EQ(first, sp2);
  EXPECT_EQ(next.size(), 0);

  auto sp3 = "A simple\nexample"_t;
  std::tie(first, next) = sp3.split_next_line();
  EXPECT_EQ(first, "A simple");
  EXPECT_EQ(next, "example");

  std::tie(first, next) = ""_t.split_next_line();
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");
}

TEST(kltext, test_split_lines) {
  auto sp1 = "This\nis\nsome\ntext\n"_t.split_lines();
  EXPECT_EQ(sp1.size(), 5);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);
  EXPECT_EQ(sp1[4], ""_t);

  sp1 = "This\nis\nsome\ntext\n"_t.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);

  auto sp2 = "\n\nThis\n\nis\n\n\nsome\ntext"_t.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\nsome\ntext"_t.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "Hello\r\n"_t.split_lines();
  EXPECT_EQ(sp2.size(), 2);
  EXPECT_EQ(sp2[0], "Hello"_t);
  EXPECT_EQ(sp2[1], ""_t);

  sp2 = "Hello\r\n"_t.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 1);
  EXPECT_EQ(sp2[0], "Hello"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 4);
  EXPECT_EQ(sp2[0], "This"_t);
  EXPECT_EQ(sp2[1], "is"_t);
  EXPECT_EQ(sp2[2], "some"_t);
  EXPECT_EQ(sp2[3], "text"_t);
}

TEST(kltext, test_last_pos) {
  auto filename = "/123456/89.1234"_t;
  EXPECT_EQ(filename.last_pos('.'), 10);
  EXPECT_EQ(filename.last_pos('/'), 7);
  EXPECT_EQ(filename.last_pos('4'), 14);
  EXPECT_EQ(filename.last_pos('9'), 9);
  EXPECT_FALSE(filename.last_pos('+').has_value());
}

TEST(kltext, test_sublen_subpos) {
  Text t("0123456789");
  EXPECT_EQ(t.sublen(0, 2), "01"_t);
  EXPECT_EQ(t.sublen(0, 4), "0123"_t);
  EXPECT_EQ(t.sublen(2, 4), "2345"_t);
  EXPECT_EQ(t.sublen(2, 0), ""_t);
  EXPECT_EQ(t.sublen(8, 4), "89"_t);

  EXPECT_EQ(t.subpos(0, 2), "012"_t);
  EXPECT_EQ(t.subpos(0, 4), "01234"_t);
  EXPECT_EQ(t.subpos(2, 4), "234"_t);
  EXPECT_EQ(t.subpos(2, 0), ""_t);
  EXPECT_EQ(t.subpos(2, 2), "2"_t);
  EXPECT_EQ(t.sublen(8, 9), "89"_t);
  EXPECT_EQ(t.sublen(8, 10), "89"_t);
  EXPECT_EQ(t.sublen(8, 20), "89"_t);
}

TEST(kltext, test_count) {
  Text t("hello world");
  EXPECT_EQ(t.count('h'), 1);
  EXPECT_EQ(t.count('x'), 0);
  EXPECT_EQ(t.count('e'), 1);
  EXPECT_EQ(t.count('l'), 3);
  EXPECT_EQ(t.count('o'), 2);
  EXPECT_EQ(t.count(' '), 1);

  EXPECT_EQ(t.count("l"), 3);
  EXPECT_EQ(t.count("lo"_t), 5);
  EXPECT_EQ(t.count(""_t), 0);
  EXPECT_EQ(t.count("lox"_t), 5);
}

TEST(kltext, test_fill_c_buff) {
  auto t = "123"_t;
  char buffer[100] = "Helloworld";
  t.fill_c_buffer(buffer, 100);
  EXPECT_EQ("123"s, buffer);
  t.fill_c_buffer(buffer, 3);
  EXPECT_EQ("12"s, buffer);
  t.fill_c_buffer(buffer, 2);
  EXPECT_EQ("1"s, buffer);
  t.fill_c_buffer(buffer, 1);
  EXPECT_EQ(""s, buffer);
  t.fill_c_buffer(buffer, 100);
  EXPECT_EQ("123"s, buffer);
  t.fill_c_buffer(buffer, 0);
  EXPECT_EQ("123"s, buffer);
}

TEST(kltext, test_indent) {
  EXPECT_EQ(""_t.get_indent(), 0);
  EXPECT_EQ("  "_t.get_indent(), 2);
  EXPECT_EQ("  xxx"_t.get_indent(), 2);
  EXPECT_EQ(" x "_t.get_indent(), 1);

  EXPECT_FALSE(""_t.skip_indent(2).has_value());
  EXPECT_TRUE(""_t.skip_indent(0).has_value());
  EXPECT_EQ(""_t.skip_indent(0), ""_t);
  EXPECT_TRUE("  "_t.skip_indent(1).has_value());
  EXPECT_EQ("  "_t.skip_indent(1), " "_t);
  EXPECT_TRUE("  "_t.skip_indent(2).has_value());
  EXPECT_EQ("  "_t.skip_indent(2), ""_t);
  EXPECT_FALSE("  "_t.skip_indent(3).has_value());
  EXPECT_FALSE("  "_t.skip_indent(100).has_value());
  EXPECT_FALSE("  xxx"_t.skip_indent(3).has_value());
  EXPECT_EQ("  xxx"_t.skip_indent(2), "xxx"_t);
  EXPECT_EQ("  xxx"_t.skip_indent(1), " xxx"_t);
  EXPECT_EQ("  xxx"_t.skip_indent(0), "  xxx"_t);
  EXPECT_FALSE(" x "_t.skip_indent(3).has_value());
  EXPECT_FALSE(" x "_t.skip_indent(2).has_value());
  EXPECT_EQ(" x "_t.skip_indent(1), "x "_t);
  EXPECT_EQ(" x "_t.skip_indent(0), " x "_t);
}

TEST(kltext, format_test) {
  Text v1("Hello");
  Text v2("World");

  EXPECT_EQ(std::format("{}, {}!", v1, v2), "Hello, World!");
  std::stringstream s;
  s << v1 << ", " << v2 << "!";
  EXPECT_EQ(s.view(), "Hello, World!"sv);
}

TEST(kltext, operator_index) {
  Text t("Hello");
  EXPECT_EQ(t[0], 'H');
  EXPECT_EQ(t[1], 'e');
  EXPECT_EQ(t[2], 'l');
  EXPECT_EQ(t[3], 'l');
  EXPECT_EQ(t[4], 'o');
  EXPECT_EQ(t[-5], 'H');
  EXPECT_EQ(t[-4], 'e');
  EXPECT_EQ(t[-3], 'l');
  EXPECT_EQ(t[-2], 'l');
  EXPECT_EQ(t[-1], 'o');
  EXPECT_THROW(t[5], std::out_of_range);
  EXPECT_THROW(t[-6], std::out_of_range);
}

TEST(kltext, test_reuse) {
  Text t("Hello");
  Text t1(t);
  EXPECT_EQ(t.begin(), t1.begin());
  EXPECT_EQ(t.begin() + 1, t1.sublen(1, 2).begin());
  Text t2{t.copy()};
  EXPECT_NE(t.begin(), t2.begin());
  EXPECT_EQ(t.size(), t2.size());
  EXPECT_EQ(t, t2);
  EXPECT_EQ(t.to_string(), t2.to_string());

  auto tspan = t.to_raw_data();
  EXPECT_EQ(tspan.size(), t.size());
  EXPECT_EQ(static_cast<const void*>(tspan.data()), static_cast<const void*>(t.begin()));
}

TEST(kltext, test_int_convert) {
  Text t("Hello");
  EXPECT_THROW(t.to_int(), std::invalid_argument);
  Text t2{"100"};
  EXPECT_EQ(t2.to_int(), 100);
  Text t3{"00100"};
  EXPECT_EQ(t3.to_int(), 100);
  Text t4{"100t"};
  EXPECT_EQ(t4.to_int(), 100);
  Text t5{"-100"};
  EXPECT_EQ(t5.to_int(), -100);
}

TEST(kltext, expect_tests) {
  Text t("Hello world");
  auto t1 = t.expect("Hello");
  EXPECT_TRUE(t1.has_value());
  EXPECT_EQ(t1.value(), " world");
  EXPECT_FALSE(t.expect("Hallo").has_value());
  EXPECT_FALSE(t1->expect("world").has_value());
  EXPECT_TRUE(t1->expect_ws("world").has_value());
  EXPECT_EQ(t1->expect_ws("world").value(), ""_t);
  EXPECT_TRUE(t.expect_ws("Hello").has_value());
  EXPECT_EQ(t.expect_ws("Hello").value(), " world"_t);
}

TEST(kltext, quote_escaped) {
  Text t(R"(Hell"o\ world)");
  auto q = t.quote_escaped();
  EXPECT_EQ(q.size(), t.size() + 4);
  EXPECT_EQ(q.sublen(0, 5), "\"Hell"_t);
  EXPECT_EQ(q.subpos(10, 100), " world\""_t);
  EXPECT_EQ(q[5], '\\');
  EXPECT_EQ(q[6], '"');
  EXPECT_EQ(q[7], 'o');
  EXPECT_EQ(q[8], '\\');
  EXPECT_EQ(q[9], '\\');

  Text t2("Hello world");
  auto q2 = t2.quote_escaped();
  EXPECT_EQ(q2.size(), t2.size() + 2);
  EXPECT_EQ(q2.sublen(1, t2.size()), t2);
  EXPECT_EQ(q2[0], '"');
  EXPECT_EQ(q2[-1], '"');
}

TEST(kltext, kltext_dict) {
  std::unordered_map<Text, Text> m;
  m["hello"] = "world";
  EXPECT_EQ(m["hello"], "world");
}
