#include <kl/klfs.hpp>
#include <gtest/gtest.h>
using namespace kl::literals;

TEST(klfs, test_file_path) {
  kl::FilePath path("/foldername/sub/folder/file.name.txt");
  EXPECT_EQ(path.stem(), "file.name");
  EXPECT_EQ(path.filename(), "file.name.txt");
  EXPECT_EQ(path.extension(), "txt");
  EXPECT_EQ(path.folder_name(), "/foldername/sub/folder");
  EXPECT_EQ(path.base_folder(), "/foldername");
  EXPECT_EQ(path.base_folder(2), "/foldername/sub");
  EXPECT_EQ(path.base_folder(3), "/foldername/sub/folder");
  EXPECT_EQ(path.base_folder(10), "/foldername/sub/folder");

  path = kl::FilePath("/foldername/sub/folder/.filenametxt");
  EXPECT_EQ(path.stem(), ".filenametxt");
  EXPECT_EQ(path.filename(), ".filenametxt");
  EXPECT_EQ(path.extension(), ""_t);
  EXPECT_EQ(path.folder_name(), "/foldername/sub/folder");
}

TEST(klfs, test_replace_components) {
  kl::FilePath p("/test1/file.txt"_t);
  EXPECT_EQ(p.replace_extension("cpp").full_path(), "/test1/file.cpp"_t);
  EXPECT_EQ(p.replace_extension("cpp").extension(), "cpp"_t);
  EXPECT_EQ(p.replace_extension("").full_path(), "/test1/file"_t);
  kl::FilePath p2("/test1/.file"_t);
  EXPECT_EQ(p2.replace_extension("cpp"_t).full_path(), "/test1/.file.cpp"_t);
  EXPECT_EQ(p2.replace_extension(""_t).full_path(), "/test1/.file"_t);
  kl::FilePath p3("file"_t);
  kl::FilePath p4("/1/2/3/4/5/6/7/file"_t);
  kl::FilePath p5("1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.base_folder(), "1");
  EXPECT_EQ(p5.base_folder(2), "1/2");
  EXPECT_EQ(p5.base_folder(3), "1/2/3");
  EXPECT_EQ(p.depth(), 1);
  EXPECT_EQ(p.discard_base_folder(3).full_path(), "file.txt"_t);
  EXPECT_EQ(p.discard_base_folder(0).full_path(), "/test1/file.txt"_t);
  EXPECT_EQ(p.discard_base_folder(1).full_path(), "file.txt"_t);
  EXPECT_EQ(p.discard_base_folder(2).full_path(), "file.txt"_t);
  EXPECT_EQ(p3.depth(), 0);
  EXPECT_EQ(p3.discard_base_folder(3), p3);
  EXPECT_EQ(p3.discard_base_folder(0), p3);
  EXPECT_EQ(p4.depth(), 7);
  EXPECT_EQ(p4.discard_base_folder(0).full_path(), "/1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(1).full_path(), "2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(2).full_path(), "3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(3).full_path(), "4/5/6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(4).full_path(), "5/6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(5).full_path(), "6/7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(6).full_path(), "7/file"_t);
  EXPECT_EQ(p4.discard_base_folder(7).full_path(), "file"_t);
  EXPECT_EQ(p4.discard_base_folder(8).full_path(), "file"_t);
  EXPECT_EQ(p5.depth(), 7);
  EXPECT_EQ(p5.discard_base_folder(0).full_path(), "1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(1).full_path(), "2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(2).full_path(), "3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(3).full_path(), "4/5/6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(4).full_path(), "5/6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(5).full_path(), "6/7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(6).full_path(), "7/file"_t);
  EXPECT_EQ(p5.discard_base_folder(7).full_path(), "file"_t);
  EXPECT_EQ(p5.discard_base_folder(8).full_path(), "file"_t);

  EXPECT_EQ(p.replace_base_folder("x").full_path(), "x/file.txt"_t);
  EXPECT_EQ(p.replace_base_folder("x", 0).full_path(), "x/test1/file.txt"_t);
  EXPECT_EQ(p3.replace_base_folder("x", 0).full_path(), "x/file"_t);
  EXPECT_EQ(p3.replace_base_folder("x").full_path(), "x/file"_t);
  EXPECT_EQ(p5.replace_base_folder("x/y/z", 3).full_path(), "x/y/z/4/5/6/7/file"_t);
  EXPECT_EQ(p4.replace_base_folder("x/y/z", 3).full_path(), "x/y/z/4/5/6/7/file"_t);
}

TEST(klfs, test_file_path_normalization) {
  kl::FilePath path("/");
  EXPECT_EQ(path.full_path(), "/"_t);
  path = kl::FilePath("/hello/world/!!!");
  EXPECT_EQ(path.full_path(), "/hello/world/!!!"_t);
  path = kl::FilePath("/hello/world/!!!/");
  EXPECT_EQ(path.full_path(), "/hello/world/!!!"_t);
  path = kl::FilePath("hello/world/!!!/");
  EXPECT_EQ(path.full_path(), "hello/world/!!!"_t);
  path = kl::FilePath("./hello/world/!!!/");
  EXPECT_EQ(path.full_path(), "hello/world/!!!"_t);
  path = kl::FilePath("./hello///world//!!!/");
  EXPECT_EQ(path.full_path(), "hello/world/!!!"_t);
  path = kl::FilePath("////////");
  EXPECT_EQ(path.full_path(), "/"_t);
  path = kl::FilePath("./");
  EXPECT_EQ(path.full_path(), "."_t);
  path = kl::FilePath("./////");
  EXPECT_EQ(path.full_path(), "."_t);
  path = kl::FilePath("./////");
  EXPECT_EQ(path.full_path(), "."_t);
}
