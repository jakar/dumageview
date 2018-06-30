#include "dumageview/application.h"
#include "dumageview/cmdline.h"
#include "dumageview/log.h"

int main(int argc, char** argv)
{
  using namespace dumageview;
  using application::ExitCode;

  try
  {
    Application app{argc, argv};
    app.init();
    auto exitCode = app.exec();
    DUMAGEVIEW_LOG_TRACE(app.log(), "App exited cleanly.");
    return exitCode;
  }
  catch (cmdline::Error&)
  {
    // exit cleanly after parser errors
    return ExitCode::cliError;
  }

  return ExitCode::unexpected;
}
