#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "util/types.h"
#include "fmt/format.h"
#include "Poco/Logger.h"
#include "Poco/Message.h"

namespace common  {
namespace details {

template <Poco::Message::Priority PRIO, typename... T>
void logger_internal(Poco::Logger& logger, const char* file, const int line, const char* fstr, T&&... args)
{
    static_assert(PRIO >= Poco::Message::PRIO_FATAL && PRIO <= Poco::Message::PRIO_TRACE, "Invalid message priority!!!");
    if constexpr (PRIO == Poco::Message::PRIO_FATAL)
    {
        if (logger.fatal())
            logger.fatal(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_CRITICAL)
    {
        if (logger.critical())
            logger.critical(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_ERROR)
    {
        if (logger.error())
            logger.error(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_WARNING)
    {
        if (logger.warning())
            logger.warning(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_NOTICE)
    {
        if (logger.notice())
            logger.notice(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_INFORMATION)
    {
        if (logger.information())
            logger.information(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_DEBUG)
    {
        if (logger.debug())
            logger.debug(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
    else if constexpr (PRIO == Poco::Message::PRIO_TRACE)
    {
        if (logger.trace())
            logger.trace(fmt::format(fstr, std::forward<T>(args)...), file, line);
    }
}

} // namespace details
} // namespace common

#undef clog_fatal
#undef clog_critical
#undef clog_error
#undef clog_warning
#undef clog_notice
#undef clog_info
#undef clog_debug
#undef clog_trace

#define clog_fatal(logger, fstr, ...)    \
    common::details::logger_internal<Poco::Message::PRIO_FATAL>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_critical(logger, fstr, ...) \
    common::details::logger_internal<Poco::Message::PRIO_CRITICAL>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_error(logger, fstr, ...)    \
    common::details::logger_internal<Poco::Message::PRIO_ERROR>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_warning(logger, fstr, ...)  \
    common::details::logger_internal<Poco::Message::PRIO_WARNING>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_notice(logger, fstr, ...)   \
    common::details::logger_internal<Poco::Message::PRIO_NOTICE>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_info(logger, fstr, ...)     \
    common::details::logger_internal<Poco::Message::PRIO_INFORMATION>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_debug(logger, fstr, ...)    \
    common::details::logger_internal<Poco::Message::PRIO_DEBUG>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#define clog_trace(logger, fstr, ...)    \
    common::details::logger_internal<Poco::Message::PRIO_TRACE>(logger, __FILE__, __LINE__, fstr, __VA_ARGS__)

#endif // __LOGGER_H__