#include "dumageview/cmdline.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <string>

namespace dumageview::cmdline {
  Parser::Parser(int argc, char** argv) : parser_(argc, argv) {
    generalOpts_.add_options()("help,h", "display help message");

    hiddenOpts_.add_options()("input", po::value<std::string>(), "input image");

    visibleOpts_.add(generalOpts_);
    allOpts_.add(generalOpts_).add(hiddenOpts_);

    positionalArgs_.add("input", 1);

    parser_.options(allOpts_);
    parser_.positional(positionalArgs_);

    // allow opts that Qt might use
    // parser_.allow_unregistered();
  }

  Args Parser::parse() {
    auto parsedOpts = parser_.run();

    po::variables_map varMap;
    po::store(parsedOpts, varMap);
    po::notify(varMap);

    // exit on help
    if (varMap.find("help") != varMap.end()) {
      throw HelpError("Usage:");
    }

    // check input image path
    std::optional<Path> imagePath;

    if (varMap.find("input") != varMap.end()) {
      std::string inputArg = varMap.at("input").as<std::string>();

      // TODO: check for existence
      imagePath.emplace(inputArg);
    }

    return {imagePath};
  }

  void Parser::printUsage() {
    fmt::print("{}\n", visibleOpts_);
  }
}
