#include "dumageview/application.h"
#include "dumageview/cmdline.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <QSurfaceFormat>

#include <boost/assert.hpp>
#include <boost/program_options.hpp>

#include <string>

namespace {
  namespace po = boost::program_options;
}

namespace dumageview::application {
  Application::Application(int& argc, char** argv)
      : QApplication(argc, argv), _log(log::initAppLogger()) {
    DUMAGEVIEW_LOG_TRACE(_log);

    cmdline::Parser cmdParser{argc, argv};

    try {
      _cmdArgs = cmdParser.parse();
    } catch (cmdline::Error const& ex) {
      fmt::print("\n{}\n", ex.what());
      cmdParser.printUsage();
      throw;
    }
  }

  void Application::init() {
    DUMAGEVIEW_LOG_TRACE(_log);

    // set global opengl state
    auto surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setVersion(2, 1);
    surfaceFormat.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    setApplicationName("Dumageview");

    _controller = std::make_unique<AppController>(_cmdArgs.value());
  }
}
