#ifndef DUMAGEVIEW_CMDLINE_H_
#define DUMAGEVIEW_CMDLINE_H_

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <optional>

namespace dumageview::cmdline {
  namespace po = boost::program_options;

  using Path = boost::filesystem::path;

  struct Args {
    std::optional<Path> imagePath;
  };

  class Parser {
   public:
    Parser(int argc, char** argv);
    Args parse();
    void printUsage();

   private:
    po::command_line_parser _parser;
    po::options_description _generalOpts{"General options"};
    po::options_description _hiddenOpts;
    po::options_description _visibleOpts;
    po::options_description _allOpts;
    po::positional_options_description _positionalArgs;
  };

  using Error = po::error;

  class HelpError : public virtual Error {
   public:
    using Error::Error;
  };
}

#endif  // DUMAGEVIEW_CMDLINE_H_
