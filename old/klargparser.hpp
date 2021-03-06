#pragma once

#include "kltext.hpp"
#include <unordered_map>
#include <vector>

namespace kl {

struct Argument {
  char short_arg = '\0';
  Text long_arg = {};
  bool has_param = false;
  Text info;
};

// Inspiration source: https://docs.python.org/3/library/argparse.html
class ArgumentParser {
public:
  ArgumentParser(Text prog_name, Text description, Text epilog);
  ArgumentParser(const ArgumentParser&) = default;
  ArgumentParser(ArgumentParser&&) = default;
  ArgumentParser& operator=(const ArgumentParser&) = default;
  ArgumentParser& operator=(ArgumentParser&&) = default;
  ~ArgumentParser() = default;

  void add(const Argument& argument);
  // ArgumentParser& create_subparser(kl::Text feature);

  List<Text> get_help() const;

private:
  ArgumentParser();
  bool m_child_parser = false;
  std::unordered_map<Text, std::unique_ptr<ArgumentParser>> m_children;
  Text m_prog_name;
  Text m_description;
  Text m_epilogue;
  std::vector<Argument> m_arguments;
  size_t m_large_param_length = 0;
};

} // namespace kl
