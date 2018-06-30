#ifndef DUMAGEVIEW_LOG_H_
#define DUMAGEVIEW_LOG_H_

#include "dumageview/assert.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <boost/current_function.hpp>

#include <memory>
#include <string>
#include <utility>
#include <type_traits>

namespace dumageview::log
{
  using namespace std::literals;

  using LoggerPtr = std::shared_ptr<spdlog::logger>;
  using Level = spdlog::level::level_enum;

  LoggerPtr initAppLogger();

  inline char const* appLoggerName()
  {
    return "dumageview";
  }

  inline LoggerPtr appLogger()
  {
    auto logger = spdlog::get(appLoggerName());
    DUMAGEVIEW_ASSERT(logger);
    return logger;
  }

  namespace detail
  {
    template<class A>
    auto convertArg(A&& arg)
    {
      if constexpr (std::is_pointer_v<std::decay_t<A>>)
      {
        return static_cast<void*>(arg);
      }
      else
      {
        return std::forward<A>(arg);
      }
    }

    inline auto convertArg(char const* cstr)
    {
      return cstr;
    }
  }

  template<class... As>
  void log(Level level,
           LoggerPtr const& logger,
           char const* message,
           As&&... args)
  {
    DUMAGEVIEW_ASSERT(logger);
    logger->log(level, message,
                detail::convertArg(std::forward<As>(args))...);
  }

  template<class... As>
  void log(Level level,
           char const* message,
           As&&... args)
  {
    log(level, appLogger(), message, std::forward<As>(args)...);
  }

  template<class... As>
  void trace(As&&... args)
  {
    log(Level::trace, std::forward<As>(args)...);
  }

  template<class... As>
  void debug(As&&... args)
  {
    log(Level::debug, std::forward<As>(args)...);
  }

  template<class... As>
  void info(As&&... args)
  {
    log(Level::info, std::forward<As>(args)...);
  }

  template<class... As>
  void warn(As&&... args)
  {
    log(Level::warn, std::forward<As>(args)...);
  }

  template<class... As>
  void error(As&&... args)
  {
    log(Level::err, std::forward<As>(args)...);
  }

  template<class... As>
  void critical(As&&... args)
  {
    log(Level::critical, std::forward<As>(args)...);
  }

  namespace detail
  {
    template<class... As>
    void traceMacroHelper(char const* func,
                          char const* file,
                          long line,
                          LoggerPtr const& logger,
                          char const* msg,
                          As&&... args)
    {
      auto format = "[{}] [{}:{}] "s + msg;
      trace(logger, format.c_str(),
            func, file, line, std::forward<As>(args)...);
    }

    template<class... As>
    void traceMacroHelper(char const* func,
                          char const* file,
                          long line,
                          char const* msg,
                          As&&... args)
    {
      traceMacroHelper(func, file, line, appLogger(), msg,
                       std::forward<As>(args)...);
    }

    template<class... As>
    void traceMacroHelper(char const* func,
                          char const* file,
                          long line,
                          LoggerPtr const& logger)
    {
      traceMacroHelper(func, file, line, logger, "");
    }
  }
}

# if defined(SPDLOG_TRACE_ON)
#   define DUMAGEVIEW_LOG_TRACE(...) \
      ::dumageview::log::detail::traceMacroHelper( \
        BOOST_CURRENT_FUNCTION, __FILE__, __LINE__, __VA_ARGS__)
# else
#   define DUMAGEVIEW_LOG_TRACE(...)
# endif

# if defined(SPDLOG_DEBUG_ON) || defined(SPDLOG_TRACE_ON)
#   define DUMAGEVIEW_LOG_DEBUG(...) \
      ::dumageview::log::debug(__VA_ARGS__)
# else
#   define DUMAGEVIEW_LOG_DEBUG(...)
# endif

#endif // DUMAGEVIEW_LOG_H_
