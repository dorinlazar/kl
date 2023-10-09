#include "klargparser.hpp"
#include "klexcept.hpp"

namespace kl
{

  ArgumentParser &ArgumentParser::create_subparser(kl::Text feature)
  {
    if (m_children.contains(feature))
    {
      throw DuplicateIndex(feature);
    }
    auto parser = new ArgumentParser();
    m_children.emplace(feature, std::unique_ptr<ArgumentParser>{parser});
    return *parser;
  }

} // namespace kl
