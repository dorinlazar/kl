#include "klargparser.hpp"

#include <format>
#include "klexcept.hpp"

namespace kl {
ArgumentParser::ArgumentParser(Text prog_name, Text description, Text epilog)
    : m_prog_name(prog_name), m_description(description), m_epilogue(epilog) {
  m_arguments.push_back({.short_arg = 'h', .long_arg = Text{"help"}, .n_params = 0, .info = "This help message"});
}

void ArgumentParser::add(const Argument& argument) {
  for (const auto& arg: m_arguments) {
    if (argument.short_arg == arg.short_arg) {
      throw DuplicateIndex(Text{argument.short_arg});
    }
    if (argument.long_arg == arg.long_arg) {
      throw DuplicateIndex(argument.long_arg);
    }
  }
  m_arguments.push_back(argument);
}

ArgumentParser& ArgumentParser::create_subparser(kl::Text feature) {
  if (m_children.contains(feature)) {
    throw DuplicateIndex(feature);
  }
  auto parser = new ArgumentParser();
  m_children.emplace(feature, std::unique_ptr<ArgumentParser>{parser});
  return *parser;
}

List<Text> ArgumentParser::get_help() const {
  List<Text> result;

  result.add(Text{std::format("Usage: {} [OPTIONS]...", m_prog_name)});
  result.add(m_description);

  result.add(m_epilogue);
  return result;
}

} // namespace kl
