#include <kl/klvalue.hpp>
#include <gtest/gtest.h>

using namespace kl::literals;

TEST(klvalue, test_value_null) {
  auto v = kl::Value::create_null();
  EXPECT_TRUE(v->is_null());
  EXPECT_TRUE(!v->is_scalar());
  EXPECT_TRUE(!v->is_map());
  EXPECT_TRUE(!v->is_list());
}

TEST(klvalue, test_value_scalar) {
  auto v = kl::Value::create_scalar("Hello world"_t);
  EXPECT_TRUE(!v->is_null());
  EXPECT_TRUE(v->is_scalar());
  EXPECT_TRUE(!v->is_map());
  EXPECT_TRUE(!v->is_list());
}

TEST(klvalue, test_value_list) {
  auto pv = kl::Value::create_list();
  auto& v = *pv;
  EXPECT_TRUE(!v.is_null());
  EXPECT_TRUE(!v.is_scalar());
  EXPECT_TRUE(!v.is_map());
  EXPECT_TRUE(v.is_list());
  v.add(kl::Value::create_scalar("100"_t));
  v.add(kl::Value::create_scalar("200"_t));
  v.add(kl::Value::create_list());
  v.add(kl::Value::create_scalar("300"_t));
  v.add(kl::Value::create_scalar("400"_t));
  EXPECT_TRUE(v.size() == 5);
  EXPECT_TRUE(v[0].is_scalar());
  EXPECT_TRUE(v[0].get_value() == "100"_t);
  EXPECT_TRUE(v[1].is_scalar());
  EXPECT_TRUE(v[1].get_value() == "200"_t);
  EXPECT_TRUE(v[2].is_list());
  auto& list2 = v[2];
  list2.add(kl::Value::create_scalar("210"_t));
  list2.add(kl::Value::create_scalar("220"_t));
  list2.add(kl::Value::create_scalar("230"_t));
  list2.add(kl::Value::create_scalar("240"_t));
  list2.add(kl::Value::create_scalar("250"_t));
  EXPECT_TRUE(v[3].is_scalar());
  EXPECT_TRUE(v[3].get_value() == "300"_t);
  EXPECT_TRUE(v[4].is_scalar());
  EXPECT_TRUE(v[4].get_value() == "400"_t);
  EXPECT_TRUE(v[2].size() == 5);
  EXPECT_TRUE(v[2][0].get_value() == "210"_t);
  EXPECT_TRUE(v[2][1].get_value() == "220"_t);
  EXPECT_TRUE(v[2][2].get_value() == "230"_t);
  EXPECT_TRUE(v[2][3].get_value() == "240"_t);
  EXPECT_TRUE(v[2][4].get_value() == "250"_t);
  EXPECT_THROW(v[10], std::out_of_range);
}

TEST(klvalue, test_value_map) {
  auto pv = kl::Value::create_map();
  auto& v = *pv;
  EXPECT_TRUE(!v.is_null());
  EXPECT_TRUE(!v.is_scalar());
  EXPECT_TRUE(v.is_map());
  EXPECT_TRUE(!v.is_list());

  v.add("test"_t, kl::Value::create_scalar("test_value"_t));
  v.add("list"_t, kl::Value::create_list());
  v.add("map"_t, kl::Value::create_map());
  auto& lst = v["list"_t];
  auto& m = v["map"_t];
  lst.add("100"_t);
  lst.add("200"_t);
  lst.add("300"_t);
  lst.add("400"_t);
  m.add("m01", "m01_value");
  m.add("m02", "m02_value");
  m.add("m03", "m03_value");

  EXPECT_TRUE(v["test"].is_scalar());
  EXPECT_TRUE(v["test"].get_value() == "test_value");

  EXPECT_TRUE(v["list"].is_list());
  EXPECT_TRUE(v["list"].size() == 4);
  EXPECT_TRUE(v["list"][0] == "100");
  EXPECT_TRUE(v["list"][1] == "200");
  EXPECT_TRUE(v["list"][2] == "300");
  EXPECT_TRUE(v["list"][3] == "400");
  EXPECT_TRUE(v["map"]["m01"] == "m01_value");
  EXPECT_TRUE(v["map"]["m02"] == "m02_value");
  EXPECT_TRUE(v["map"]["m03"] == "m03_value");

  EXPECT_THROW(v["nonmap"], std::out_of_range);
}

TEST(klvalue, test_value_getopt) {
  auto root = kl::Value::create_map();
  root->add("test"_t, kl::Value::create_scalar("test_value"_t));
  root->add("map"_t, kl::Value::create_map());
  root->add("test2"_t, kl::Value::create_scalar("value2"_t));
  auto m = root->get("map"_t);
  m->add("path1", kl::Value::create_scalar("tv1"_t));
  m->add("path2", kl::Value::create_map());
  m->add("path3", kl::Value::create_scalar("tv3"_t));
  m = m->get("path2");
  m->add("deep1", kl::Value::create_scalar("dv1"_t));
  m->add("deep2", kl::Value::create_scalar("dv2"_t));
  m->add("deep3", kl::Value::create_scalar("dv3"_t));

  EXPECT_TRUE(root->get_opt("test") == "test_value");
  EXPECT_TRUE(root->get_opt("/test/") == "test_value");
  EXPECT_TRUE(root->get_opt("test2") == "value2");
  EXPECT_TRUE(root->get_opt("map/path1") == "tv1");
  EXPECT_TRUE(root->get_opt("map/path3") == "tv3");
  EXPECT_TRUE(root->get_opt("map/path3/") == "tv3");
  EXPECT_TRUE(root->get_opt("map/path2/deep1") == "dv1");
  EXPECT_TRUE(root->get_opt("map/path2/deep2") == "dv2");
  EXPECT_TRUE(root->get_opt("map/path2/deep3") == "dv3");
  EXPECT_TRUE(root->get_opt("map/path2/deep3/") == "dv3");

  EXPECT_TRUE(!root->get_opt("map/path1/deep3").has_value());
  EXPECT_TRUE(!root->get_opt("map2/path1/deep3").has_value());
  EXPECT_TRUE(!root->get_opt("map/path2").has_value());
  EXPECT_TRUE(!root->get_opt("map/").has_value());
}
