#ifndef DUMAGEVIEW_APPLICATION_H_
#define DUMAGEVIEW_APPLICATION_H_

#include "dumageview/appcontroller.h"
#include "dumageview/classtools.h"
#include "dumageview/cmdline.h"
#include "dumageview/log.h"

#include <QApplication>

#include <memory>
#include <optional>

namespace dumageview {
  class AppController;
}

namespace dumageview::application {
  namespace exitcode {
    enum ExitCode : int {
      success = 0,
      cliError = 1,
      glInitError = 2,
      unexpected = 15,
    };
  }

  using exitcode::ExitCode;

  class Application : public QApplication,
                      public classtools::MutableSingleton<Application> {
    Q_OBJECT;

   public:
    Application(int& argc, char** argv);

    virtual ~Application() = default;

    void init();

    log::LoggerPtr const& getLog() const {
      return log_;
    }

   private:
    log::LoggerPtr log_;
    std::optional<cmdline::Args> cmdArgs_;
    std::unique_ptr<AppController> controller_;
  };
}

namespace dumageview {
  using application::Application;
}

#endif  // DUMAGEVIEW_APPLICATION_H_
