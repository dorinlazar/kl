#include <kl/kltext.hpp>
#include <kl/klds.hpp>
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltextchain, text_construction) {
  TextChain tc{"Hello"_t, " "_t, "world"_t};
  EXPECT_EQ(tc.size(), 11);
  EXPECT_EQ(tc.to_text(), "Hello world");

  List<Text> l{"Hello"_t, " "_t, "world"_t};
  TextChain tc1(l);
  EXPECT_EQ(tc1.size(), 11);
  EXPECT_EQ(tc1.to_text(), "Hello world");
  EXPECT_EQ(l, tc1.chain());
  EXPECT_EQ(l, tc.chain());

  Dict<Text, Text> dict;
  dict.add("first", "Hello");
  dict.add("second", " ");
  dict.add("third", "world");
  TextChain tc2(dict.values());
  EXPECT_EQ(tc2.size(), 11);

  auto tc3(std::move(tc));
  EXPECT_EQ(tc3.size(), 11);
  EXPECT_EQ(tc3.to_text(), "Hello world");

  TextChain empty;
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(empty.to_text(), ""_t);
}

TEST(kltextchain, chain_ops) {
  TextChain tc{"Hello"_t};
  tc += " "_t;
  tc += "world";
  EXPECT_EQ(tc.size(), 11);
  EXPECT_EQ(tc.to_text(), "Hello world");
  List<Text> l{"Hello"_t, " "_t, "world"_t};
  EXPECT_EQ(l, tc.chain());
  TextChain tc1{"!"_t, " "_t, "Some more stuff"};
  tc += tc1;
  EXPECT_EQ(tc.to_text(), "Hello world! Some more stuff"_t);
  tc.clear();
  EXPECT_EQ(tc.to_text(), ""_t);

  tc = "Hello"_t + " world";
  EXPECT_EQ(tc.size(), 11);
  EXPECT_EQ(tc.to_text(), "Hello world");
  tc = tc + "!";
  EXPECT_EQ(tc.size(), 12);
  EXPECT_EQ(tc.to_text(), "Hello world!");
}

TEST(kltextchain, join_ops) {
  TextChain empty;
  EXPECT_EQ(empty.join('x'), ""_t);
  EXPECT_EQ(empty.join("x"), ""_t);

  TextChain tc{"Hello"_t, "world"_t};
  EXPECT_EQ(tc.size(), 10);
  EXPECT_EQ(tc.join(' '), "Hello world"_t);
  EXPECT_EQ(tc.join('\0'), "Helloworld"_t);
  EXPECT_EQ(tc.join('X'), "HelloXworld"_t);

  EXPECT_EQ(tc.join(" "), "Hello world"_t);
  EXPECT_EQ(tc.join(""), "Helloworld"_t);
  EXPECT_EQ(tc.join("'X'"), "Hello'X'world"_t);

  EXPECT_EQ(tc.join(" ", "--", "--"), "--Hello world--"_t);
  EXPECT_EQ(tc.join(" ", "++", "--"), "++Hello world--"_t);
  EXPECT_EQ(tc.join(" ", "", "--"), "Hello world--"_t);
  EXPECT_EQ(tc.join("", "--", "+"), "--Helloworld+"_t);
  EXPECT_EQ(tc.join("'-'", "'", "'"), "'Hello'-'world'"_t);

  TextChain tc1{"Hello"_t, ""_t, "world"_t};
  EXPECT_EQ(tc1.join(" "), "Hello  world"_t);
  EXPECT_EQ(tc1.join(""), "Helloworld"_t);
  EXPECT_EQ(tc1.join("'X'"), "Hello'X''X'world"_t);

  TextChain tc2{"Hello"_t};
  EXPECT_EQ(tc2.join(" "), "Hello"_t);
  EXPECT_EQ(tc2.join(""), "Hello"_t);
  EXPECT_EQ(tc2.join("'X'"), "Hello"_t);
  EXPECT_EQ(tc2.join(' '), "Hello"_t);
  EXPECT_EQ(tc2.join('\0'), "Hello"_t);
  EXPECT_EQ(tc2.join('X'), "Hello"_t);
}

TEST(kltextchain, formatting) {
  TextChain tc{"Hello"_t, "world"_t};
  EXPECT_EQ(std::format("{}", tc), R"({"Hello"}, {"world"})");
  std::stringstream s;
  s << tc;
  EXPECT_EQ(s.view(), R"({"Hello"}, {"world"})"sv);
}
