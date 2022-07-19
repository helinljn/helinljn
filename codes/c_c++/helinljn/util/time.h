#ifndef __TIME_H__
#define __TIME_H__

#include <ctime>
#include <chrono>
#include <string>

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

inline std::string safe_asctime(const tm* stm)
{
    char buf[64] = {0};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
    asctime_s(buf, stm);
#else
    asctime_r(stm, buf);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)

    return std::string(buf);
}

inline std::string safe_ctime(const time_t* timep)
{
    char buf[64] = {0};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
    ctime_s(buf, sizeof(buf), timep);
#else
    ctime_r(timep, buf);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)

    return std::string(buf);
}

inline tm safe_gmtime(const time_t* timep)
{
    tm result{};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
    ::gmtime_s(&result, timep);
#else
    ::gmtime_r(timep, &result);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)

    return result;
}

inline tm safe_localtime(const time_t* timep)
{
    tm result{};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
    ::localtime_s(&result, timep);
#else
    ::localtime_r(timep, &result);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)

    return result;
}

} // namespace ljn

#endif // __TIME_H__