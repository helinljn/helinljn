#ifndef __COMMON_H__
#define __COMMON_H__

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "spdlog/spdlog.h"

namespace ljn {

template<typename DurationType = std::chrono::nanoseconds>
inline time_t system_clock_now(void)
{
    std::chrono::system_clock::duration dt = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<DurationType>(dt).count();
}

template<typename DurationType = std::chrono::nanoseconds>
inline time_t steady_clock_now(void)
{
    std::chrono::steady_clock::duration dt = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<DurationType>(dt).count();
}

} // namespace ljn

#endif // __COMMON_H__