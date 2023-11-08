#include "klargparser.hpp"
#include <algorithm>
#include <format>
#include "klexcept.hpp"

namespace kl {
ArgumentParser::ArgumentParser(Text prog_name, Text description, Text epilog)
    : m_prog_name(prog_name), m_description(description), m_epilogue(epilog) {
  m_arguments.push_back({.short_arg = 'h', .long_arg = Text{"help"}, .has_param = false, .info = "This help message"});
}

void ArgumentParser::add(const Argument& argument) {
  for (const auto& arg: m_arguments) {
    if (argument.short_arg == arg.short_arg) {
      throw Exception::DuplicateIndex(Text(argument.short_arg).to_string());
    }
    if (argument.long_arg == arg.long_arg) {
      throw Exception::DuplicateIndex(argument.long_arg.to_string());
    }
  }
  m_large_param_length = std::max(m_large_param_length, argument.long_arg.size());
  m_arguments.push_back(argument);
}

// ArgumentParser& ArgumentParser::create_subparser(kl::Text feature) {
//   if (m_children.contains(feature)) {
//     throw DuplicateIndex(feature);
//   }
//   auto parser = new ArgumentParser();
//   m_children.emplace(feature, std::unique_ptr<ArgumentParser>{parser});
//   return *parser;
// }

List<Text> ArgumentParser::get_help() const {
  List<Text> result;

  result.add(Text{std::format("Usage: {} [OPTIONS]...", m_prog_name)});
  result.add(m_description);

  for (const auto& arg: m_arguments) {
    TextChain tc;
    tc.add("    ");
    if (arg.short_arg) {
      tc.add("-");
      tc.add(Text{arg.short_arg});
      if (arg.long_arg.size() > 0) {
        tc.add(", ");
      } else {
        tc.add("  ");
      }
    } else {
      tc.add("    ");
    }

    if (arg.long_arg.size() > 0) {
      tc.add("--");
      tc.add(arg.long_arg);
    }
    for (uint32_t i = 0; i < m_large_param_length + 4 - arg.long_arg.size(); i++) {
      tc.add(" ");
    }
    tc.add("- ");
    tc.add(arg.info);

    result.add(tc.to_text());
  }

  result.add(m_epilogue);
  return result;
}

} // namespace kl
