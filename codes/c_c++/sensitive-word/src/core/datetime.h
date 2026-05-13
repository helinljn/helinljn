#pragma once

#ifndef DATETIME_H
#define DATETIME_H

#include "core.h"
#include "timestamp.h"
#include <ctime>
#include <string>
#include <string_view>

namespace core {

// 如果给定年份是闰年则返回 true，否则返回 false
inline constexpr bool is_leap_year(int year)
{
    return year >= 0 && ((year % 4) == 0) && ((year % 100) != 0 || (year % 400) == 0);
}

// 返回给定年份和月份对应的天数，月份范围为 1 到 12
inline constexpr int days_of_month(int year, int month)
{
    if (year < 0 || month < 1 || month > 12)
        return 0;

    constexpr int days_table[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && is_leap_year(year) ? 29 : days_table[month];
}

// 检查给定的日期和时间是否有效(所有参数都处于合法范围内)
// 如果所有参数都有效则返回 true，否则返回 false
inline constexpr bool is_datetime_valid(int year, int month, int day, int hour, int minute, int second,
                                        int millisecond = 0, int microsecond = 0)
{
    return (year        >= 1970 && year        <= 3000)
        && (month       >= 1    && month       <= 12)
        && (day         >= 1    && day         <= days_of_month(year, month))
        && (hour        >= 0    && hour        <= 23)
        && (minute      >= 0    && minute      <= 59)
        && (second      >= 0    && second      <= 60)  // 允许闰秒
        && (millisecond >= 0    && millisecond <= 999)
        && (microsecond >= 0    && microsecond <= 999);
}

////////////////////////////////////////////////////////////////
// 该类表示一个本地时间时刻，使用年、月、日、时、分、秒、毫秒和微秒来表示
//
// Windows：
// 如果 timeptr 引用的日期晚于 UTC 时间3000 年 12 月 31 日 23:59:59，
// _mktime64 将返回转换为 __time64_t 类型的 -1
////////////////////////////////////////////////////////////////
class CORE_API datetime final
{
public:
    // 创建一个表示当前日期和时间的 datetime
    explicit datetime();

    // 根据时间戳中的日期和时间创建 datetime
    explicit datetime(const timestamp& ts);

    // 根据 tm 结构体创建 datetime
    explicit datetime(const tm& tmval);

    // 根据给定的本地日期和时间创建 datetime
    //   * year 的范围为 1970 到 3000
    //   * month 的范围为 1 到 12
    //   * day 的范围为 1 到 31
    //   * hour 的范围为 0 到 23
    //   * minute 的范围为 0 到 59
    //   * second 的范围为 0 到 60(允许闰秒)
    //   * millisecond 的范围为 0 到 999
    //   * microsecond 的范围为 0 到 999
    explicit datetime(int year, int month, int day, int hour, int minute, int second, int millisecond = 0, int microsecond = 0);

    datetime(const datetime&) = default;
    datetime(datetime&&) = default;

    datetime& operator =(const datetime&) = default;
    datetime& operator =(datetime&&) = default;

    // 赋值为一个时间戳
    datetime& operator =(const timestamp& ts) {return assign(ts);}

    // 赋值为一个 tm 结构体
    datetime& operator =(const tm& tmval) {return assign(tmval);}

    // 设置一个时间戳
    datetime& assign(const timestamp& ts);

    // 设置一个 tm 结构体
    datetime& assign(const tm& tmval);

    // 设置本地日期和时间
    //   * year 的范围为 1970 到 3000
    //   * month 的范围为 1 到 12
    //   * day 的范围为 1 到 31
    //   * hour 的范围为 0 到 23
    //   * minute 的范围为 0 到 59
    //   * second 的范围为 0 到 60(允许闰秒)
    //   * millisecond 的范围为 0 到 999
    //   * microsecond 的范围为 0 到 999
    datetime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // 使用当前时间更新 datetime
    void update();

    // 返回年份(1970 到 3000)
    int year() const {return tmval_.tm_year + 1900;}

    // 返回月份(1 到 12)
    int month() const {return tmval_.tm_mon + 1;}

    // 返回月中的日期(1 到 31)
    int day() const {return tmval_.tm_mday;}

    // 返回星期几(0 到 6，其中 0=星期日，1=星期一，...，6=星期六)
    int day_of_week() const {return tmval_.tm_wday;}

    // 返回该日期是一年中的第几天，1 月 1 日为 1，2 月 1 日为 32，以此类推
    int day_of_year() const {return tmval_.tm_yday + 1;}

    // 返回小时(0 到 23)
    int hour() const {return tmval_.tm_hour;}

    // 返回分钟(0 到 59)
    int minute() const {return tmval_.tm_min;}

    // 返回秒数(0 到 60，包含闰秒)
    int second() const {return tmval_.tm_sec;}

    // 返回毫秒数(0 到 999)
    int millisecond() const {return static_cast<int>((epoch_microseconds() % timestamp::resolution) / 1000);}

    // 返回微秒数(0 到 999)
    int microsecond() const {return static_cast<int>((epoch_microseconds() % timestamp::resolution) % 1000);}

    // 返回自该 datetime 所表示时刻起已经过去的时间，等价于 datetime() - *this
    time_t elapsed() const {return ts_.elapsed();}

    // 当且仅当自该 datetime 所表示时刻起，
    // 已经过了给定的时间间隔(以微秒表示)时返回 true
    bool is_elapsed(time_t interval) const {return ts_.is_elapsed(interval);}

    // 返回以秒表示的时间戳
    time_t epoch_time() const {return ts_.epoch_time();}

    // 返回以微秒表示的时间戳
    time_t epoch_microseconds() const {return ts_.epoch_microseconds();}

    // 将 datetime 转换为 timestamp
    timestamp to_timestamp() const {return ts_;}

    // 将 datetime 转换为 tm 结构体
    tm to_tm() const {return tmval_;}

    // 使用给定的格式字符串将 datetime 格式化为字符串
    // 支持所有 strftime() 的格式说明符，另外还支持：
    //   {ms} - 毫秒(000-999)
    //   {us} - 微秒(000-999)
    // 示例：dt.format("%Y-%m-%d %H:%M:%S.{ms}{us}")
    // 返回格式化后的字符串；失败时返回空字符串
    std::string format(std::string_view fmt) const;

    bool operator ==(const datetime& ts) const {return ts_ == ts.ts_;}
    bool operator !=(const datetime& ts) const {return ts_ != ts.ts_;}
    bool operator < (const datetime& ts) const {return ts_ <  ts.ts_;}
    bool operator <=(const datetime& ts) const {return ts_ <= ts.ts_;}
    bool operator > (const datetime& ts) const {return ts_ >  ts.ts_;}
    bool operator >=(const datetime& ts) const {return ts_ >= ts.ts_;}

    datetime  operator + (const timespan& span) const {return datetime(ts_ + span);}
    datetime  operator - (const timespan& span) const {return datetime(ts_ - span);}
    datetime& operator +=(const timespan& span) {return assign(ts_ + span);}
    datetime& operator -=(const timespan& span) {return assign(ts_ - span);}

    timespan operator -(const datetime& ts) const {return timespan(ts_ - ts.ts_);}

private:
    // 使用 ts_ 更新 tmval_
    void update_tm();

private:
    timestamp ts_;     // 当前本地时间对应的时间戳
    tm        tmval_;  // tm 结构体
};

} // namespace core

#endif // DATETIME_H
