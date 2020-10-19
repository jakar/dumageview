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
    po::command_line_parser parser_;
    po::options_description generalOpts_{"General options"};
    po::options_description hiddenOpts_;
    po::options_description visibleOpts_;
    po::options_description allOpts_;
    po::positional_options_description positionalArgs_;
  };

  using Error = po::error;

  class HelpError : public virtual Error {
   public:
    using Error::Error;
  };
}

#endif  // DUMAGEVIEW_CMDLINE_H_
