#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <array>
#include "klfs.hpp"

namespace kl {
namespace fs = std::filesystem;

const std::array<Text, 2> DiscardableFolders{"."_t, ".."_t};
const Text FolderSeparator("/");
const std::size_t LargestSupportedFile = 0x8FFFFFFFULL;

Text klfs_read_file_impl(const Text& filename) {
  const fs::path p =
      filename.starts_with(FolderSeparator[0]) ? filename.to_view() : fs::current_path() / filename.to_view();
  auto size = fs::file_size(p); // throws if error;

  if (size > LargestSupportedFile) [[unlikely]] {
    // we don't work with oversized data
    throw std::out_of_range("File size exceeds expectations");
  }

  std::ifstream is;
  is.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  is.open(p, std::ifstream::binary);
  if (is.bad()) {
    return {};
  }
  if (size >= 3) {
    const char a = static_cast<char>(is.get());
    const char b = static_cast<char>(is.get());
    const char c = static_cast<char>(is.get());
    if (a != '\xEF' || b != '\xBB' || c != '\xBF') {
      is.seekg(0);
    } else {
      size -= 3;
    }
  }
  if (size > 0) {
    auto* memblock = TextRefCounter::allocate(size);
    is.read(memblock->text_data(), static_cast<std::streamsize>(size));
    return {memblock, size};
  }
  return {};
}

Text klfs_normalize_path(const Text& filename) {
  TextChain tc;
  uint32_t last_pos = 0;
  bool last_was_slash = false;
  uint32_t last_slash_pos = 0;
  bool cut_needed = false;
  for (uint32_t i = 0; i < filename.size(); i++) {
    if (filename[i] == FolderSeparator[0]) {
      if (last_was_slash) {
        cut_needed = true;
      } else {
        last_was_slash = true;
        cut_needed = false;
        last_slash_pos = i;
      }
    } else {
      if (cut_needed) {
        tc.add(filename.subpos(last_pos, last_slash_pos));
        last_pos = i;
      }
      cut_needed = false;
      last_was_slash = false;
    }
  }

  if (last_was_slash) {
    tc.add(filename.subpos(last_pos, last_slash_pos == 0 ? 0 : last_slash_pos - 1)); // don't include the ending /
    last_pos = filename.size();
  }
  tc.add(filename.skip(last_pos));
  Text res = tc.to_text();
  if (res.starts_with("./") && res.size() > 2) {
    res = res.skip(2);
  }
  return res;
}

FilePath::FilePath(const Text& path) : m_full_name(klfs_normalize_path(path)) {
  m_last_slash_pos = m_full_name.last_pos(FolderSeparator[0]);
  m_last_dot_pos = m_full_name.last_pos('.');
  if (m_last_dot_pos.has_value() && (*m_last_dot_pos == 0 || m_full_name[*m_last_dot_pos - 1] == FolderSeparator[0])) {
    m_last_dot_pos = {};
  }
}
Text FilePath::folder_name() const {
  return m_last_slash_pos.has_value() ? Text(m_full_name, 0, *m_last_slash_pos == 0 ? 1 : *m_last_slash_pos) : Text();
}
Text FilePath::filename() const {
  return m_last_slash_pos.has_value() ? m_full_name.skip(*m_last_slash_pos + 1) : m_full_name;
}
Text FilePath::extension() const { return m_last_dot_pos.has_value() ? m_full_name.skip(*m_last_dot_pos + 1) : Text(); }
Text FilePath::stem() const {
  const uint32_t stem_start = m_last_slash_pos.has_value() ? *m_last_slash_pos + 1 : 0;
  const uint32_t stem_end = m_last_dot_pos.value_or(m_full_name.size());
  return Text(m_full_name, stem_start, stem_end - stem_start);
}
Text FilePath::full_path() const { return m_full_name; }

FilePath FilePath::replace_extension(const kl::Text& new_ext) const {
  if (new_ext.size() > 0) {
    if (m_last_dot_pos.has_value()) {
      return FilePath(m_full_name.subpos(0, *m_last_dot_pos) + new_ext);
    }
    return FilePath(m_full_name + "."_t + new_ext);
  }
  if (m_last_dot_pos.has_value()) {
    return FilePath(m_full_name.subpos(0, *m_last_dot_pos - 1));
  }
  return *this;
}

Text FilePath::base_folder(uint32_t levels) const {
  if (levels == 0 || m_full_name.size() == 0) {
    return {};
  }
  if (levels < depth()) {
    if (m_full_name[0] == FolderSeparator[0]) {
      levels++;
    }
    const auto pos = m_full_name.pos(FolderSeparator[0], levels);
    if (pos.has_value()) {
      return m_full_name.subpos(0, *pos - 1);
    }
  }
  return folder_name();
}

FilePath FilePath::discard_base_folder(uint32_t levels) const {
  if (levels == 0 || m_full_name.size() == 0) {
    return *this;
  }
  if (levels < depth()) {
    const Text p = m_full_name[0] == FolderSeparator[0] ? m_full_name.skip(1) : m_full_name;
    auto pos = p.pos(FolderSeparator[0], levels);
    if (pos.has_value()) {
      return FilePath{p.subpos(*pos + 1, p.size())};
    }
  }
  return FilePath(filename());
}

FilePath FilePath::replace_base_folder(const kl::Text& new_folder, uint32_t levels) const {
  const FilePath fp = discard_base_folder(levels);
  return FilePath(new_folder + FolderSeparator + fp.m_full_name);
}

uint32_t FilePath::depth() const {
  return m_full_name.count(FolderSeparator[0]) - (m_full_name.starts_with(FolderSeparator[0]) ? 1 : 0);
}
uint32_t FilePath::folder_depth() const {
  if (m_full_name.size() == 0 || (m_full_name.size() == 1 && m_full_name[0] == '.')) {
    return 0;
  }
  return depth() + 1;
}

List<Text> FilePath::breadcrumbs() const { return m_full_name.split_by_char(FolderSeparator[0]); }

FilePath FilePath::add(const kl::Text& component) const {
  if (m_full_name.size() == 0 || (m_full_name.size() == 1 && m_full_name[0] == '.')) {
    return FilePath(component);
  }
  return FilePath(m_full_name + FolderSeparator + component);
}

std::strong_ordering FilePath::operator<=>(const FilePath& fp) const { return m_full_name <=> fp.m_full_name; }
bool FilePath::operator==(const FilePath& fp) const { return m_full_name == fp.m_full_name; }

const std::size_t MaxPathSize = 1024;

std::vector<FileSystemEntryInfo> klfs_get_directory_entries(const Text& folder) {
  std::vector<FileSystemEntryInfo> res;
  std::array<char, MaxPathSize> buffer;
  folder.fill_c_buffer(buffer.data(), buffer.size());
  DIR* dir = opendir(buffer.data());
  dirent* de;
  const Text padded_folder = folder + FolderSeparator;
  while ((de = readdir(dir)) != nullptr) {
    const Text t(de->d_name);
    if (t == DiscardableFolders[0] || t == DiscardableFolders[1]) {
      continue;
    }
    if (de->d_type == DT_REG || de->d_type == DT_DIR || de->d_type == DT_LNK) {
      const Text full_path = padded_folder + t;
      full_path.fill_c_buffer(buffer.data(), buffer.size());
      struct stat statbuf;
      FileType ft = FileType::Directory;
      if (0 == stat(buffer.data(), &statbuf)) {
        if (S_ISREG(statbuf.st_mode)) {
          ft = FileType::File;
        } else if (S_ISDIR(statbuf.st_mode)) {
          ft = FileType::Directory;
        } else {
          continue;
        }
        const DateTime last_write(statbuf.st_mtim.tv_sec, static_cast<int32_t>(statbuf.st_mtim.tv_nsec));
        FileSystemEntryInfo fi{.type = ft, .last_write = last_write, .path = FilePath(full_path)};
        res.emplace_back(std::move(fi));
      }
    }
  }
  closedir(dir);
  return res;
}

void FileSystem::navigate_tree(const Text& treeBase,
                               std::function<NavigateInstructions(const FileSystemEntryInfo& file)> processor) {
  std::queue<FileSystemEntryInfo> to_process;
  to_process.push({.type = FileType::Directory, .last_write = DateTime::UnixEpoch, .path = FilePath(treeBase)});
  while (!to_process.empty()) {
    const FileSystemEntryInfo fi = to_process.front();
    to_process.pop();
    auto entries = klfs_get_directory_entries(fi.path.full_path());
    for (const auto& entry: entries) {
      auto res = processor(entry);
      if (res == NavigateInstructions::Continue && entry.type == FileType::Directory) {
        to_process.push(entry);
      }
      if (res == NavigateInstructions::Stop) {
        return;
      }
    }
  }
}

Text FileSystem::executable_path(const Text& exename) {
  if (!exename.contains(FolderSeparator[0])) {
    auto folders = Text(getenv("PATH")).split_by_char(':');
    for (const auto& f: folders) {
      const FilePath fp(f + "/"_t + exename);
      if (FileSystem::exists(fp.full_path())) {
        return fp.full_path();
      }
    }
  }
  return exename;
}

bool FileSystem::make_directory(const Text& path) {
  // TODO(dorin) try to not do it like a lazy individual that we all know you are.
  return std::filesystem::create_directories(path.to_view());
}

bool FileSystem::is_directory(const Text& path) {
  // TODO(dorin) try to not do it like a lazy individual that we all know you are.
  return std::filesystem::is_directory(path.to_view());
}

bool FileSystem::is_file(const Text& path) {
  // TODO(dorin) try to not do it like a lazy individual that we all know you are.
  return std::filesystem::is_regular_file(path.to_view());
}

bool FileSystem::exists(const Text& path) {
  // TODO(dorin) try to not do it like a lazy individual that we all know you are.
  return std::filesystem::exists(path.to_view());
}

FileReader::FileReader(const Text& name) { m_unread_content = klfs_read_file_impl(name); }

std::optional<Text> FileReader::read_line() {
  if (m_unread_content.size() > 0) [[likely]] {
    auto [res, next] = m_unread_content.split_next_line();
    m_unread_content = next;
    return res;
  }
  return {};
}

List<Text> FileReader::read_all_lines(SplitEmpty onEmpty) {
  auto res = m_unread_content.split_lines(onEmpty);
  m_unread_content.clear();
  return res;
}

Text FileReader::read_all() {
  auto res = m_unread_content;
  m_unread_content.clear();
  return res;
}

std::optional<char> FileReader::read_char() {
  if (m_unread_content.size() > 0) [[likely]] {
    char c = m_unread_content[0];
    m_unread_content = m_unread_content.skip(1);
    return c;
  }
  return {};
}

bool FileReader::has_data() { return m_unread_content.size() > 0; }

Folder::Folder(const kl::Text& name, const kl::Text& path, const Folder* parent)
    : m_parent(parent), m_name(name), m_path(path) {}

void Folder::add_item(const kl::FileSystemEntryInfo& fi, const kl::Text& full_path) {
  if (fi.path.folder_name().size() == 0) {
    if (fi.type == kl::FileType::Directory) {
      m_folders.add(fi.path.full_path(), std::make_shared<Folder>(fi.path.full_path(), full_path, this));
    } else {
      m_files.add(fi);
    }
  } else {
    auto fi2 = fi;
    auto base_folder = fi.path.base_folder();
    kl::check(m_folders.has(base_folder), "Sanity check: File in folder {} added, but folder not recorded",
              base_folder);
    fi2.path = fi2.path.discard_base_folder();
    m_folders[base_folder]->add_item(fi2, full_path);
  }
}

kl::ptr<Folder> Folder::get_folder(const kl::Text& folder) const { return m_folders.get(folder, nullptr); }
kl::List<kl::ptr<Folder>> Folder::get_folders() const { return m_folders.values(); }
kl::ptr<Folder> Folder::create_folder(const kl::FilePath& path) {
  if (path.full_path().size() == 0) {
    return nullptr;
  }
  kl::ptr<Folder> where = nullptr;
  kl::FilePath current_path;
  for (const auto& fld: path.breadcrumbs()) {
    current_path = current_path.add(fld);
    Folder* ptr = where != nullptr ? where.get() : this;
    where = ptr->get_folder(fld);
    if (where == nullptr) {
      where = std::make_shared<Folder>();
      ptr->m_folders.add(fld, where);
    }
  }
  return where;
}

const FilePath& Folder::full_path() const { return m_path; }
const List<FileSystemEntryInfo>& Folder::files() const { return m_files; }

bool Folder::has_file(const kl::Text& file) const {
  return m_files.any([file](const auto& f) { return f.path.filename() == file; });
}

} // namespace kl
