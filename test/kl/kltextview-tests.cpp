#include <kl/kltext.hpp>
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltextview, text_view_construction) {
  TextView txt("Hello world", 3);
  EXPECT_EQ(txt.size(), 3);
  EXPECT_EQ(txt.view(), "Hel");

  TextView tnull(nullptr, 2);
  EXPECT_EQ(tnull.size(), 0);
  EXPECT_EQ(tnull.view(), "");
  auto tn2 = TextView(tnull);
  EXPECT_EQ(tn2.size(), 0);
  EXPECT_EQ(tn2.view(), "");
  auto tn3 = TextView(std::move(tnull));
  EXPECT_EQ(tn3.size(), 0);
  EXPECT_EQ(tn3.view(), "");
  auto tn4 = TextView(nullptr);
  EXPECT_EQ(tn4.size(), 0);
  EXPECT_EQ(tn4.view(), "");

  TextView tempty("");
  EXPECT_EQ(tempty.size(), 0);
  EXPECT_EQ(tempty.view(), "");

  TextView tempty2;
  EXPECT_EQ(tempty2.size(), 0);
  EXPECT_EQ(tempty2.view(), "");

  TextView t1("Hello World", 25);
  EXPECT_EQ(t1.size(), 11);
  EXPECT_EQ(t1.view(), "Hello World");
  TextView t1_1(std::move(t1));
  EXPECT_EQ(t1_1.size(), 11);
  EXPECT_EQ(t1_1.view(), "Hello World");

  TextView t3("Hello World\0", 12);
  EXPECT_EQ(t3.size(), 11);
  EXPECT_EQ(t3[t3.size() - 1], 'd');

  TextView t4("Hello World\0Other World", 23);
  EXPECT_EQ(t4.size(), 11);
  EXPECT_EQ(t4[10], 'd');
}

TEST(kltextview, test_trimming) {
  TextView txt(" \t hello   \n ");
  EXPECT_EQ(txt.trim().view(), "hello");
  EXPECT_EQ(txt.trim_left().view(), "hello   \n ");
  EXPECT_EQ(txt.trim_right().view(), " \t hello"s);

  TextView txt2("hel    lo");
  EXPECT_EQ(txt2.trim().view(), "hel    lo");
  EXPECT_EQ(txt2.trim_left().view(), "hel    lo");
  EXPECT_EQ(txt2.trim_right().view(), "hel    lo");

  TextView txt3;
  EXPECT_EQ(txt3.trim().view(), "");
  EXPECT_EQ(txt3.trim_left().view(), "");
  EXPECT_EQ(txt3.trim_right().view(), "");
}
TEST(kltextview, test_starts_with) {
  TextView txt("Hello");
  EXPECT_TRUE(txt.starts_with('H'));
  EXPECT_FALSE(txt.starts_with('e'));

  EXPECT_TRUE(txt.starts_with(""));
  EXPECT_TRUE(txt.starts_with("H"));
  EXPECT_TRUE(txt.starts_with("Hell"));
  EXPECT_TRUE(txt.starts_with("Hello"));
  EXPECT_FALSE(txt.starts_with("Hello world"));
  EXPECT_FALSE(txt.starts_with("world"));

  EXPECT_TRUE(txt.starts_with(""sv));
  EXPECT_TRUE(txt.starts_with("H"sv));
  EXPECT_TRUE(txt.starts_with("Hell"sv));
  EXPECT_TRUE(txt.starts_with("Hello"sv));
  EXPECT_FALSE(txt.starts_with("Hello world"sv));
  EXPECT_FALSE(txt.starts_with("world"sv));

  EXPECT_TRUE(txt.starts_with(""_tv));
  EXPECT_TRUE(txt.starts_with("H"_tv));
  EXPECT_TRUE(txt.starts_with("Hell"_tv));
  EXPECT_TRUE(txt.starts_with("Hello"_tv));
  EXPECT_FALSE(txt.starts_with("Hello world"_tv));
  EXPECT_FALSE(txt.starts_with("world"_tv));
}

TEST(kltextview, test_ends_with) {
  TextView txt("Hello");
  EXPECT_TRUE(txt.ends_with('o'));
  EXPECT_FALSE(txt.ends_with('H'));

  EXPECT_TRUE(txt.ends_with(""));
  EXPECT_TRUE(txt.ends_with("o"));
  EXPECT_TRUE(txt.ends_with("ello"));
  EXPECT_TRUE(txt.ends_with("Hello"));
  EXPECT_FALSE(txt.ends_with("Hello world"));
  EXPECT_FALSE(txt.ends_with("world"));

  EXPECT_TRUE(txt.ends_with(""sv));
  EXPECT_TRUE(txt.ends_with("o"sv));
  EXPECT_TRUE(txt.ends_with("ello"sv));
  EXPECT_TRUE(txt.ends_with("Hello"sv));
  EXPECT_FALSE(txt.ends_with("Hello world"sv));
  EXPECT_FALSE(txt.ends_with("world"sv));

  EXPECT_TRUE(txt.ends_with(""_tv));
  EXPECT_TRUE(txt.ends_with("o"_tv));
  EXPECT_TRUE(txt.ends_with("ello"_tv));
  EXPECT_TRUE(txt.ends_with("Hello"_tv));
  EXPECT_FALSE(txt.ends_with("Hello world"_tv));
  EXPECT_FALSE(txt.ends_with("world"_tv));
}

TEST(kltextview, test_comparisons) {
  TextView hello("hello");
  TextView hfllo("hfllo");
  TextView empty;

  EXPECT_EQ(hello, "hello");
  EXPECT_TRUE(hello < "hfllo");
  EXPECT_TRUE(hello <= "hfllo");
  EXPECT_TRUE(hello != "hfllo");
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
  EXPECT_TRUE(hello >= "hallo"s);
  EXPECT_TRUE(hello >= "ahllo"s);
  EXPECT_TRUE(hello > "hallo"s);
  EXPECT_TRUE(hello > "hell"s);
  EXPECT_TRUE(hello < "hello world"s);
  EXPECT_TRUE(hello > ""s);

  EXPECT_EQ(hello, "hello"sv);
  EXPECT_TRUE(hello < "hfllo"sv);
  EXPECT_TRUE(hello <= "hfllo"sv);
  EXPECT_TRUE(hello != "hfllo"sv);
  EXPECT_TRUE(hello >= "hallo"sv);
  EXPECT_TRUE(hello >= "ahllo"sv);
  EXPECT_TRUE(hello > "hallo"sv);
  EXPECT_TRUE(hello > "hell"sv);
  EXPECT_TRUE(hello < "hello world"sv);
  EXPECT_TRUE(hello > ""sv);

  EXPECT_EQ(hello, "hello"_tv);
  EXPECT_TRUE(hello < "hfllo"_tv);
  EXPECT_TRUE(hello <= "hfllo"_tv);
  EXPECT_TRUE(hello != "hfllo"_tv);
  EXPECT_TRUE(hello >= "hallo"_tv);
  EXPECT_TRUE(hello >= "ahllo"_tv);
  EXPECT_TRUE(hello > "hallo"_tv);
  EXPECT_TRUE(hello > "hell"_tv);
  EXPECT_TRUE(hello < "hello world"_tv);
  EXPECT_TRUE(hello > ""_tv);
}

TEST(kltextview, test_split) {
  TextView txt("hello");
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
  EXPECT_EQ(get<0>(""_tv.split_pos(15)), "");
  EXPECT_EQ(get<1>(""_tv.split_pos(15)), "");
}

TEST(kltextview, test_contains) {
  TextView t("hello");
  EXPECT_TRUE(t.contains('h'));
  EXPECT_TRUE(t.contains('e'));
  EXPECT_TRUE(t.contains('o'));
  EXPECT_TRUE(t.contains('l'));
  EXPECT_FALSE(t.contains('a'));
  EXPECT_FALSE(t.contains('\0'));
  EXPECT_FALSE(""_tv.contains('\0'));
}

TEST(kltextview, test_skip) {
  TextView t("hello");
  EXPECT_EQ(t.skip("hel"), "o");
  EXPECT_EQ(t.skip("el"), "hello");
  EXPECT_EQ(t.skip(""), "hello");
  EXPECT_EQ(t.skip("abcdefghijklmnopqrstuvwxyz"), "");
  EXPECT_EQ(""_tv.skip("abcdefghijklmnopqrstuvwxyz"), "");

  EXPECT_EQ(t.skip(2), "llo"_tv);
  EXPECT_EQ(t.skip(1), "ello"_tv);
  EXPECT_EQ(t.skip(4), "o"_tv);
  EXPECT_EQ(t.skip(5), ""_tv);
  EXPECT_EQ(t.skip(10), ""_tv);
  EXPECT_EQ(t.skip(100), ""_tv);

  EXPECT_EQ(t.skip_bom(), "hello");
  TextView bom_hello("\xEF\xBB\xBF"
                     "bomhello");
  EXPECT_EQ(bom_hello.skip_bom(), "bomhello"_tv);
  TextView bom("\xEF\xBB\xBF");
  EXPECT_EQ(bom.skip_bom(), ""_tv);
  TextView bo("\xEF\xBB");
  EXPECT_EQ(bo.skip_bom(), "\xEF\xBB"_tv);
}

TEST(kltextview, test_indices) {
  TextView t("hello");
  ASSERT_EQ(t[0], 'h');
  ASSERT_EQ(t[2], 'l');
  ASSERT_EQ(t[4], 'o');
  ASSERT_EQ(t[-1], 'o');
  ASSERT_EQ(t[-4], 'e');
  ASSERT_EQ(t[-5], 'h');
  EXPECT_THROW(t[5], std::out_of_range);
  EXPECT_THROW(t[500], std::out_of_range);
  EXPECT_THROW(t[-6], std::out_of_range);
  EXPECT_THROW(t[-600], std::out_of_range);
}

TEST(kltextview, test_pos) {
  TextView t("hello world");
  EXPECT_EQ(t.pos('l'), 2);
  EXPECT_EQ(t.pos('l', 2), 3);
  EXPECT_EQ(t.pos('l', 3), 9);
  EXPECT_EQ(t.pos('d'), 10);
  EXPECT_EQ(t.pos(' '), 5);
  EXPECT_FALSE(t.pos('o', 3).has_value());
  EXPECT_FALSE(t.pos('k').has_value());
  EXPECT_FALSE(t.pos('o', 0).has_value());

  EXPECT_FALSE(""_tv.pos('x').has_value());

  EXPECT_EQ(t.pos("l"_tv), 2);
  EXPECT_EQ(t.pos("ll"_tv), 2);
  EXPECT_EQ(t.pos("l"_tv, 2), 3);
  EXPECT_EQ(t.pos("l"_tv, 3), 9);
  EXPECT_EQ(t.pos("o w"_tv), 4);
  EXPECT_EQ(t.pos("d"_tv), 10);
  EXPECT_FALSE(t.pos("o W"_tv).has_value());
  EXPECT_FALSE(t.pos(""_tv).has_value());
  EXPECT_FALSE(t.pos("hello world!"_tv).has_value());
  EXPECT_EQ(t.pos("hello w"_tv), 0);
  EXPECT_EQ(t.pos("hello world"_tv), 0);
  EXPECT_FALSE(t.pos("o"_tv, 0).has_value());

  EXPECT_FALSE(""_tv.pos("x"_tv).has_value());
  EXPECT_FALSE(""_tv.pos(""_tv).has_value());
}

TEST(kltextview, test_split_char) {
  TextView t1("My,CSV,text,with,,an,ending,,");
  auto sp1 = t1.split_by_char(',');

  EXPECT_EQ(sp1.size(), 6);
  EXPECT_EQ(sp1[0], "My"_tv);
  EXPECT_EQ(sp1[1], "CSV"_tv);
  EXPECT_EQ(sp1[2], "text"_tv);
  EXPECT_EQ(sp1[3], "with"_tv);
  EXPECT_EQ(sp1[4], "an"_tv);
  EXPECT_EQ(sp1[5], "ending"_tv);

  auto sp2 = t1.split_by_char(',', SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], "My"_tv);
  EXPECT_EQ(sp2[1], "CSV"_tv);
  EXPECT_EQ(sp2[2], "text"_tv);
  EXPECT_EQ(sp2[3], "with"_tv);
  EXPECT_EQ(sp2[4], ""_tv);
  EXPECT_EQ(sp2[5], "an"_tv);
  EXPECT_EQ(sp2[6], "ending"_tv);
  EXPECT_EQ(sp2[7], ""_tv);
  EXPECT_EQ(sp2[8], ""_tv);

  auto sp3 = "Hello World"_tv.split_by_char(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_tv);
  EXPECT_EQ(sp3[1], "World"_tv);

  sp3 = "   Hello World"_tv.split_by_char(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_tv);
  EXPECT_EQ(sp3[1], "World"_tv);

  sp3 = "   Hello World"_tv.split_by_char(' ', SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 5);
  EXPECT_EQ(sp3[0], ""_tv);
  EXPECT_EQ(sp3[1], ""_tv);
  EXPECT_EQ(sp3[2], ""_tv);
  EXPECT_EQ(sp3[3], "Hello"_tv);
  EXPECT_EQ(sp3[4], "World"_tv);

  auto sp4 = "Hello_World"_tv.split_by_char(' ');
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_tv);
}

TEST(kltextview, test_split_text) {
  TextView t1("This||is||||||some||text||||");
  auto sp1 = t1.split_by_text("||"_tv);

  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_tv);
  EXPECT_EQ(sp1[1], "is"_tv);
  EXPECT_EQ(sp1[2], "some"_tv);
  EXPECT_EQ(sp1[3], "text"_tv);

  auto sp2 = t1.split_by_text("||"_tv, SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 8);
  EXPECT_EQ(sp2[0], "This"_tv);
  EXPECT_EQ(sp2[1], "is"_tv);
  EXPECT_EQ(sp2[2], ""_tv);
  EXPECT_EQ(sp2[3], ""_tv);
  EXPECT_EQ(sp2[4], "some"_tv);
  EXPECT_EQ(sp2[5], "text"_tv);
  EXPECT_EQ(sp2[6], ""_tv);
  EXPECT_EQ(sp2[7], ""_tv);

  auto sp3 = "Hello World"_tv.split_by_text(" "_tv);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_tv);
  EXPECT_EQ(sp3[1], "World"_tv);

  sp3 = "   Hello World"_tv.split_by_text(" "_tv);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_tv);
  EXPECT_EQ(sp3[1], "World"_tv);

  sp3 = "   Hello World "_tv.split_by_text(" "_tv, SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 6);
  EXPECT_EQ(sp3[0], ""_tv);
  EXPECT_EQ(sp3[1], ""_tv);
  EXPECT_EQ(sp3[2], ""_tv);
  EXPECT_EQ(sp3[3], "Hello"_tv);
  EXPECT_EQ(sp3[4], "World"_tv);
  EXPECT_EQ(sp3[5], ""_tv);

  auto sp4 = "Hello_World"_tv.split_by_text(" "_tv);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_tv);

  sp4 = "Hello"_tv.split_by_text("HelloWorld"_tv);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_tv);

  sp4 = "Hello"_tv.split_by_text("Hello"_tv);
  EXPECT_EQ(sp4.size(), 0);

  sp4 = "Hello"_tv.split_by_text(""_tv);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_tv);

  sp4 = "ttttt"_tv.split_by_text("tt"_tv);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "t"_tv);
}

TEST(kltextview, test_split_next_char) {
  auto sp1 = "This is some bad text  "_tv;
  auto [first, next] = sp1.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "This ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "is some bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "is ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "some bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "some ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "bad ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "text ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, " ");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "");

  std::tie(first, next) = sp1.split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "This");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " is some bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " is some bad text  ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "is");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " some bad text  ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "some");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " bad text  ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "bad");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " text  ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "text");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "  ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " ");
  std::tie(first, next) = next.value().skip(1).split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_FALSE(next.has_value());

  std::tie(first, next) = sp1.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "This");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "is some bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "is");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "some bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "some");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "bad text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "bad");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "text  ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "text");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), " ");
  std::tie(first, next) = next.value().split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "");

  std::tie(first, next) = ""_tv.split_next_char(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "");
  EXPECT_FALSE(next.has_value());

  std::tie(first, next) = ""_tv.split_next_char(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_FALSE(next.has_value());

  std::tie(first, next) = ""_tv.split_next_char(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_FALSE(next.has_value());
}

TEST(kltextview, test_split_next_line) {
  auto sp1 = "This\nis\r\nsome\r\nbad\ntext\n\n"_tv;
  auto [first, next] = sp1.split_next_line();
  EXPECT_EQ(first, "This");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "is\r\nsome\r\nbad\ntext\n\n");
  std::tie(first, next) = next.value().split_next_line();
  EXPECT_EQ(first, "is");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "some\r\nbad\ntext\n\n");
  std::tie(first, next) = next.value().split_next_line();
  EXPECT_EQ(first, "some");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "bad\ntext\n\n");
  std::tie(first, next) = next.value().split_next_line();
  EXPECT_EQ(first, "bad");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "text\n\n");
  std::tie(first, next) = next.value().split_next_line();
  EXPECT_EQ(first, "text");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "\n");
  std::tie(first, next) = next.value().split_next_line();
  EXPECT_EQ(first.size(), 0);
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value().size(), 0);

  auto sp2 = "A bad example"_tv;
  std::tie(first, next) = sp2.split_next_line();
  EXPECT_EQ(first, sp2);
  EXPECT_FALSE(next.has_value());

  auto sp3 = "A simple\nexample"_tv;
  std::tie(first, next) = sp3.split_next_line();
  EXPECT_EQ(first, "A simple");
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), "example");

  std::tie(first, next) = ""_tv.split_next_line();
  EXPECT_EQ(first, "");
  EXPECT_FALSE(next.has_value());
}

TEST(kltextview, test_split_lines) {
  auto sp1 = "This\nis\nsome\ntext\n"_tv.split_lines();
  EXPECT_EQ(sp1.size(), 5);
  EXPECT_EQ(sp1[0], "This"_tv);
  EXPECT_EQ(sp1[1], "is"_tv);
  EXPECT_EQ(sp1[2], "some"_tv);
  EXPECT_EQ(sp1[3], "text"_tv);
  EXPECT_EQ(sp1[4], ""_tv);

  sp1 = "This\nis\nsome\ntext\n"_tv.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_tv);
  EXPECT_EQ(sp1[1], "is"_tv);
  EXPECT_EQ(sp1[2], "some"_tv);
  EXPECT_EQ(sp1[3], "text"_tv);

  auto sp2 = "\n\nThis\n\nis\n\n\nsome\ntext"_tv.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_tv);
  EXPECT_EQ(sp2[1], ""_tv);
  EXPECT_EQ(sp2[2], "This"_tv);
  EXPECT_EQ(sp2[3], ""_tv);
  EXPECT_EQ(sp2[4], "is"_tv);
  EXPECT_EQ(sp2[5], ""_tv);
  EXPECT_EQ(sp2[6], ""_tv);
  EXPECT_EQ(sp2[7], "some"_tv);
  EXPECT_EQ(sp2[8], "text"_tv);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\nsome\ntext"_tv.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_tv);
  EXPECT_EQ(sp2[1], ""_tv);
  EXPECT_EQ(sp2[2], "This"_tv);
  EXPECT_EQ(sp2[3], ""_tv);
  EXPECT_EQ(sp2[4], "is"_tv);
  EXPECT_EQ(sp2[5], ""_tv);
  EXPECT_EQ(sp2[6], ""_tv);
  EXPECT_EQ(sp2[7], "some"_tv);
  EXPECT_EQ(sp2[8], "text"_tv);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_tv.split_lines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_tv);
  EXPECT_EQ(sp2[1], ""_tv);
  EXPECT_EQ(sp2[2], "This"_tv);
  EXPECT_EQ(sp2[3], ""_tv);
  EXPECT_EQ(sp2[4], "is"_tv);
  EXPECT_EQ(sp2[5], ""_tv);
  EXPECT_EQ(sp2[6], ""_tv);
  EXPECT_EQ(sp2[7], "some"_tv);
  EXPECT_EQ(sp2[8], "text"_tv);

  sp2 = "Hello\r\n"_tv.split_lines();
  EXPECT_EQ(sp2.size(), 2);
  EXPECT_EQ(sp2[0], "Hello"_tv);
  EXPECT_EQ(sp2[1], ""_tv);

  sp2 = "Hello\r\n"_tv.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 1);
  EXPECT_EQ(sp2[0], "Hello"_tv);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_tv.split_lines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 4);
  EXPECT_EQ(sp2[0], "This"_tv);
  EXPECT_EQ(sp2[1], "is"_tv);
  EXPECT_EQ(sp2[2], "some"_tv);
  EXPECT_EQ(sp2[3], "text"_tv);
}

TEST(kltextview, test_last_pos) {
  auto filename = "/123456/89.1234"_tv;
  EXPECT_EQ(filename.last_pos('.'), 10);
  EXPECT_EQ(filename.last_pos('/'), 7);
  EXPECT_EQ(filename.last_pos('4'), 14);
  EXPECT_EQ(filename.last_pos('9'), 9);
  EXPECT_FALSE(filename.last_pos('+').has_value());
}

TEST(kltextview, test_sublen_subpos) {
  TextView t("0123456789");
  EXPECT_EQ(t.sublen(0, 2), "01"_tv);
  EXPECT_EQ(t.sublen(0, 4), "0123"_tv);
  EXPECT_EQ(t.sublen(2, 4), "2345"_tv);
  EXPECT_EQ(t.sublen(2, 0), ""_tv);
  EXPECT_EQ(t.sublen(8, 4), "89"_tv);

  EXPECT_EQ(t.subpos(0, 2), "012"_tv);
  EXPECT_EQ(t.subpos(0, 4), "01234"_tv);
  EXPECT_EQ(t.subpos(2, 4), "234"_tv);
  EXPECT_EQ(t.subpos(2, 0), ""_tv);
  EXPECT_EQ(t.subpos(2, 2), "2"_tv);
  EXPECT_EQ(t.sublen(8, 9), "89"_tv);
  EXPECT_EQ(t.sublen(8, 10), "89"_tv);
  EXPECT_EQ(t.sublen(8, 20), "89"_tv);
}

TEST(kltextview, test_count) {
  TextView t("hello world");
  EXPECT_EQ(t.count('h'), 1);
  EXPECT_EQ(t.count('x'), 0);
  EXPECT_EQ(t.count('e'), 1);
  EXPECT_EQ(t.count('l'), 3);
  EXPECT_EQ(t.count('o'), 2);
  EXPECT_EQ(t.count(' '), 1);
}

TEST(kltextview, test_indent) {
  EXPECT_EQ(""_tv.get_indent(), 0);
  EXPECT_EQ("  "_tv.get_indent(), 2);
  EXPECT_EQ("  xxx"_tv.get_indent(), 2);
  EXPECT_EQ(" x "_tv.get_indent(), 1);

  EXPECT_FALSE(""_tv.skip_indent(2).has_value());
  EXPECT_TRUE(""_tv.skip_indent(0).has_value());
  EXPECT_EQ(""_tv.skip_indent(0), ""_tv);
  EXPECT_TRUE("  "_tv.skip_indent(1).has_value());
  EXPECT_EQ("  "_tv.skip_indent(1), " "_tv);
  EXPECT_TRUE("  "_tv.skip_indent(2).has_value());
  EXPECT_EQ("  "_tv.skip_indent(2), ""_tv);
  EXPECT_FALSE("  "_tv.skip_indent(3).has_value());
  EXPECT_FALSE("  "_tv.skip_indent(100).has_value());
  EXPECT_FALSE("  xxx"_tv.skip_indent(3).has_value());
  EXPECT_EQ("  xxx"_tv.skip_indent(2), "xxx"_tv);
  EXPECT_EQ("  xxx"_tv.skip_indent(1), " xxx"_tv);
  EXPECT_EQ("  xxx"_tv.skip_indent(0), "  xxx"_tv);
  EXPECT_FALSE(" x "_tv.skip_indent(3).has_value());
  EXPECT_FALSE(" x "_tv.skip_indent(2).has_value());
  EXPECT_EQ(" x "_tv.skip_indent(1), "x "_tv);
  EXPECT_EQ(" x "_tv.skip_indent(0), " x "_tv);
}

TEST(kltextview, format_test) {
  TextView v1("Hello");
  TextView v2("World");

  EXPECT_EQ(std::format("{}, {}!", v1, v2), "Hello, World!");
  std::stringstream s;
  s << v1 << ", " << v2 << "!";
  EXPECT_EQ(s.view(), "Hello, World!"sv);
}

TEST(kltextview, expect_test) {
  TextView v1("Hello");
  TextView v2("  World");

  EXPECT_FALSE(v1.expect('h').has_value());
  EXPECT_TRUE(v1.expect('H').has_value());
  EXPECT_EQ(v1.expect('H'), "ello"_tv);
  EXPECT_FALSE(v1.expect("h"_tv).has_value());
  auto llo = v1.expect("He"_tv);
  EXPECT_TRUE(llo.has_value());
  EXPECT_EQ(llo, "llo"_tv);

  EXPECT_TRUE(v2.expect(' ').has_value());
  EXPECT_EQ(v2.expect(' ').value(), " World"_tv);
  EXPECT_TRUE(v2.expect("  "_tv).has_value());
  EXPECT_EQ(v2.expect("  ").value(), "World"_tv);
  EXPECT_FALSE(v2.expect_ws(' ').has_value());
  EXPECT_TRUE(v2.expect_ws('W').has_value());
  EXPECT_FALSE(v2.expect_ws("He"_tv).has_value());
  EXPECT_TRUE(v2.expect_ws("Wo"_tv).has_value());
  EXPECT_EQ(v2.expect_ws("Wo"_tv).value(), "rld"_tv);
}
