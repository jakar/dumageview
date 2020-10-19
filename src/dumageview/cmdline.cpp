#include "dumageview/cmdline.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <string>

namespace dumageview::cmdline {
  Parser::Parser(int argc, char** argv) : _parser(argc, argv) {
    _generalOpts.add_options()("help,h", "display help message");

    _hiddenOpts.add_options()("input", po::value<std::string>(), "input image");

    _visibleOpts.add(_generalOpts);
    _allOpts.add(_generalOpts).add(_hiddenOpts);

    _positionalArgs.add("input", 1);

    _parser.options(_allOpts);
    _parser.positional(_positionalArgs);

    // allow opts that Qt might use
    // _parser.allow_unregistered();
  }

  Args Parser::parse() {
    auto parsedOpts = _parser.run();

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
    fmt::print("{}\n", _visibleOpts);
  }
}
