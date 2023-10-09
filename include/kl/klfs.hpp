#pragma once
#include "kltext.hpp"
#include "kltime.hpp"
#include <functional>

namespace kl {

class FilePath {
  Text m_full_name;
  std::optional<uint32_t> m_last_slash_pos;
  std::optional<uint32_t> m_last_dot_pos;

public:
  FilePath() = default;
  explicit FilePath(const Text& path);
  [[nodiscard]] Text folder_name() const;
  [[nodiscard]] Text filename() const;
  [[nodiscard]] Text extension() const;
  [[nodiscard]] Text stem() const;
  [[nodiscard]] Text full_path() const;

  [[nodiscard]] FilePath replace_extension(const kl::Text& new_ext) const;

  [[nodiscard]] Text base_folder(uint32_t levels = 1) const;
  [[nodiscard]] FilePath discard_base_folder(uint32_t levels = 1) const;
  [[nodiscard]] FilePath replace_base_folder(const kl::Text& new_folder, uint32_t levels = 1) const;

  [[nodiscard]] uint32_t depth() const;
  [[nodiscard]] uint32_t folder_depth() const; // depth if path is folder (usually depth()+1).

  [[nodiscard]] List<Text> breadcrumbs() const;
  [[nodiscard]] FilePath add(const kl::Text& component) const;

  std::strong_ordering operator<=>(const FilePath& fp) const;
  bool operator==(const FilePath& fp) const;
};

enum class FileType { Directory, File };
struct FileSystemEntryInfo {
  FileType type;
  DateTime last_write;
  FilePath path;
};

enum class NavigateInstructions { Continue, Skip, Stop };

struct FileSystem {
  static Text executable_path(const Text& exename);
  static bool make_directory(const Text& path);
  static bool is_directory(const Text& path);
  static bool is_file(const Text& path);
  static bool exists(const Text& path);

  static void navigate_tree(const Text& treeBase, std::function<NavigateInstructions(const FileSystemEntryInfo& file)>);
};

struct InputSource {
  InputSource() = default;
  InputSource(const InputSource&) = delete;
  InputSource(InputSource&&) = delete;
  InputSource& operator=(const InputSource&) = delete;
  InputSource& operator=(InputSource&&) = delete;
  virtual ~InputSource() = default;

  virtual std::optional<Text> read_line() = 0;
  virtual std::optional<char> read_char() = 0;
  virtual List<Text> read_all_lines(SplitEmpty onEmpty) = 0;
  virtual Text read_all() = 0;
  virtual bool has_data() = 0;
};

struct FileReader final : public InputSource {
  explicit FileReader(const Text& name);
  FileReader(const FileReader&) = delete;
  FileReader(FileReader&&) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader& operator=(FileReader&&) = delete;
  ~FileReader() override = default;
  [[nodiscard]] std::optional<Text> read_line() override;
  [[nodiscard]] std::optional<char> read_char() override;
  [[nodiscard]] List<Text> read_all_lines(SplitEmpty onEmpty) override;
  [[nodiscard]] Text read_all() override;
  [[nodiscard]] bool has_data() override;

private:
  Text m_unread_content; // we can do away with this.
};

struct Folder {
  const Folder* m_parent = nullptr;
  kl::Text m_name;
  kl::FilePath m_path;
  kl::Dict<kl::Text, kl::ptr<Folder>> m_folders;
  kl::List<kl::FileSystemEntryInfo> m_files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent);
  void add_item(const kl::FileSystemEntryInfo& file, const kl::Text& path);

  [[nodiscard]] kl::ptr<Folder> get_folder(const kl::Text& folder) const;
  [[nodiscard]] kl::List<kl::ptr<Folder>> get_folders() const;
  [[nodiscard]] kl::ptr<Folder> create_folder(const kl::FilePath& path);
  [[nodiscard]] const kl::FilePath& full_path() const;
  [[nodiscard]] const kl::List<kl::FileSystemEntryInfo>& files() const;
  [[nodiscard]] bool has_file(const kl::Text& file) const;
};

struct DirectoryEntry {
  kl::Text name;
  DateTime last_modified;
  size_t size;
};

class Directory {

public:
  Directory();
  Directory(Directory&&) = delete;
  Directory(const Directory&) = delete;
  Directory& operator=(Directory&&) = delete;
  Directory& operator=(const Directory&) = delete;
  ~Directory() = default;

  [[nodiscard]] kl::Text name() const;
  [[nodiscard]] const kl::List<kl::DirectoryEntry>& subdirectories() const;
  [[nodiscard]] const kl::List<kl::DirectoryEntry>& files() const;
};

} // namespace kl
