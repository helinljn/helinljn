#pragma once

#ifndef TIMESPAN_H
#define TIMESPAN_H

#include <chrono>

namespace core    {
namespace details {

inline constexpr time_t milliseconds = 1000;                 // 一毫秒包含的微秒数
inline constexpr time_t seconds      = 1000 * milliseconds;  // 一秒包含的微秒数
inline constexpr time_t minutes      =   60 * seconds;       // 一分钟包含的微秒数
inline constexpr time_t hours        =   60 * minutes;       // 一小时包含的微秒数
inline constexpr time_t days         =   24 * hours;         // 一天包含的微秒数

} // namespace details

////////////////////////////////////////////////////////////////
// 表示时间跨度的类，精度可达微秒
////////////////////////////////////////////////////////////////
class timespan final
{
public:
    static_assert(sizeof(time_t) == 8 && std::is_signed_v<time_t>, "Invalid time_t!");

public:
    // 创建一个零时间跨度
    constexpr timespan() : span_(0) {}

    // 创建一个时间跨度
    constexpr timespan(time_t microseconds) : span_(microseconds) {}

    // 创建一个时间跨度
    constexpr timespan(int days, int hours, int minutes, int seconds, int microseconds)
        : span_(0)
    {
        assign(days, hours, minutes, seconds, microseconds);
    }

    // 根据 std::chrono::duration 创建时间跨度
    template <class T, class Period>
    constexpr timespan(const std::chrono::duration<T, Period>& dtime)
        : span_(static_cast<time_t>(std::chrono::duration_cast<std::chrono::microseconds>(dtime).count()))
    {
    }

    // 拷贝构造与移动构造函数
    timespan(const timespan&) = default;
    timespan(timespan&&) = default;

    // 拷贝赋值与移动赋值运算符
    timespan& operator =(const timespan&) = default;
    timespan& operator =(timespan&&) = default;

    // 赋值为新的时间跨度
    timespan& operator =(time_t microseconds) {span_ = microseconds; return *this;}

    // 设置新的时间跨度
    constexpr timespan& assign(int days, int hours, int minutes, int seconds, int microseconds)
    {
        span_  = 0;
        span_ += microseconds;
        span_ += details::seconds * seconds;
        span_ += details::minutes * minutes;
        span_ += details::hours   * hours;
        span_ += details::days    * days;

        return *this;
    }

    constexpr bool operator ==(const timespan& ts) const {return span_ == ts.span_;}
    constexpr bool operator !=(const timespan& ts) const {return span_ != ts.span_;}
    constexpr bool operator < (const timespan& ts) const {return span_ <  ts.span_;}
    constexpr bool operator <=(const timespan& ts) const {return span_ <= ts.span_;}
    constexpr bool operator > (const timespan& ts) const {return span_ >  ts.span_;}
    constexpr bool operator >=(const timespan& ts) const {return span_ >= ts.span_;}

    constexpr bool operator ==(time_t microseconds) const {return span_ == microseconds;}
    constexpr bool operator !=(time_t microseconds) const {return span_ != microseconds;}
    constexpr bool operator < (time_t microseconds) const {return span_ <  microseconds;}
    constexpr bool operator <=(time_t microseconds) const {return span_ <= microseconds;}
    constexpr bool operator > (time_t microseconds) const {return span_ >  microseconds;}
    constexpr bool operator >=(time_t microseconds) const {return span_ >= microseconds;}

    constexpr timespan operator +(const timespan& ts) const {return timespan(span_ + ts.span_);}
    constexpr timespan operator -(const timespan& ts) const {return timespan(span_ - ts.span_);}
    timespan& operator +=(const timespan& ts) {span_ += ts.span_; return *this;}
    timespan& operator -=(const timespan& ts) {span_ -= ts.span_; return *this;}

    constexpr timespan operator +(time_t microseconds) const {return timespan(span_ + microseconds);}
    constexpr timespan operator -(time_t microseconds) const {return timespan(span_ - microseconds);}
    timespan& operator +=(time_t microseconds) {span_ += microseconds; return *this;}
    timespan& operator -=(time_t microseconds) {span_ -= microseconds; return *this;}

    // 返回天数
    constexpr int days() const {return static_cast<int>(span_ / details::days);}

    // 返回小时数(0 到 23)
    constexpr int hours() const {return static_cast<int>(span_ / details::hours % 24);}

    // 返回分钟数(0 到 59)
    constexpr int minutes() const {return static_cast<int>(span_ / details::minutes % 60);}

    // 返回秒数(0 到 59)
    constexpr int seconds() const {return static_cast<int>(span_ / details::seconds % 60);}

    // 返回毫秒数(0 到 999)
    constexpr int milliseconds() const {return static_cast<int>(span_ / details::milliseconds % 1000);}

    // 返回不足一毫秒的微秒部分(0 到 999)
    constexpr int microseconds() const {return static_cast<int>(span_ % 1000);}

    // 返回总天数
    constexpr int total_days() const {return days();}

    // 返回总小时数
    constexpr int total_hours() const {return static_cast<int>(span_ / details::hours);}

    // 返回总分钟数
    constexpr int total_minutes() const {return static_cast<int>(span_ / details::minutes);}

    // 返回总秒数
    constexpr int total_seconds() const {return static_cast<int>(span_ / details::seconds);}

    // 返回总毫秒数
    constexpr time_t total_milliseconds() const {return span_ / details::milliseconds;}

    // 返回总微秒数
    constexpr time_t total_microseconds() const {return span_;}

private:
    time_t span_;  // 以微秒表示
};

} // namespace core

#endif // TIMESPAN_H