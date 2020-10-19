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
      : QApplication(argc, argv),
        log_(log::initAppLogger()) {
    DUMAGEVIEW_LOG_TRACE(log_);
    cmdline::Parser cmdParser{argc, argv};
    try {
      cmdArgs_ = cmdParser.parse();
    } catch (cmdline::Error const& ex) {
      fmt::print("\n{}\n", ex.what());
      cmdParser.printUsage();
      throw;
    }
  }

  void Application::init() {
    DUMAGEVIEW_LOG_TRACE(log_);

    // set global opengl state
    auto surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setVersion(2, 1);
    surfaceFormat.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    setApplicationName("Dumageview");

    controller_ = std::make_unique<AppController>(cmdArgs_.value());
  }
}
