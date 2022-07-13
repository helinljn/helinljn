#ifndef __ASIO2_STUDY_H__
#define __ASIO2_STUDY_H__

#include "asio2/asio2.hpp"
#include "spdlog/spdlog.h"

#include <ctime>
#include <chrono>

template<typename DurationType = std::chrono::nanoseconds>
inline time_t get_time_system(void)
{
	std::chrono::system_clock::duration dt = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<DurationType>(dt).count();
}

template<typename DurationType = std::chrono::nanoseconds>
inline time_t get_time_steady(void)
{
	std::chrono::steady_clock::duration dt = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<DurationType>(dt).count();
}

inline void safe_gmtime(const time_t* timep, tm* result)
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
	::gmtime_s(result, timep);
#else
	::gmtime_r(timep, result);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
}

inline void safe_localtime(const time_t* timep, tm* result)
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
	::localtime_s(result, timep);
#else
	::localtime_r(timep, result);
#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_)
}

#endif // __ASIO2_STUDY_H__