#include "dumageview/log.h"

#include "dumageview/assert.h"

namespace dumageview::log
{
  namespace
  {
    // TODO: make these runtime options
    constexpr char defaultPattern[] = "%^[%n] [%l] %v%$";
    constexpr Level defaultFlushLevel = Level::critical;

#if defined(SPDLOG_TRACE_ON)
    constexpr Level defaultVisibleLevel = Level::trace;
#elif defined(SPDLOG_DEBUG_ON)
    constexpr Level defaultVisibleLevel = Level::debug;
#else
    constexpr Level defaultVisibleLevel = Level::info;
#endif
  }

  LoggerPtr initAppLogger()
  {
    DUMAGEVIEW_ASSERT(!spdlog::get(appLoggerName()));

    auto logger = spdlog::stdout_color_mt(appLoggerName());
    DUMAGEVIEW_ASSERT(logger);

    logger->set_pattern(defaultPattern);
    logger->set_level(defaultVisibleLevel);
    logger->flush_on(defaultFlushLevel);

    return logger;
  }
}
