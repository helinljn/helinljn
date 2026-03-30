#ifndef __DATETIME_H__
#define __DATETIME_H__

#include "core_port.h"
#include "timestamp.h"

namespace core {

// Returns true if the given year is a leap year, false otherwise.
inline constexpr bool is_leap_year(int year)
{
    return year >= 0 && ((year % 4) == 0) && ((year % 100) != 0 || (year % 400) == 0);
}

// Returns the number of days in the given month and year. Month is from 1 to 12.
inline constexpr int days_of_month(int year, int month)
{
    if (year < 0 || month < 1 || month > 12)
        return 0;

    constexpr int days_table[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && is_leap_year(year) ? 29 : days_table[month];
}

// Checks if the given date and time is valid(all arguments are within a proper range).
// Returns true if all arguments are valid, false otherwise.
inline constexpr bool is_datetime_valid(int year, int month, int day, int hour, int minute, int second,
                                        int millisecond = 0, int microsecond = 0)
{
    return (year        >= 1970 && year        <= 3000)
        && (month       >= 1    && month       <= 12)
        && (day         >= 1    && day         <= days_of_month(year, month))
        && (hour        >= 0    && hour        <= 23)
        && (minute      >= 0    && minute      <= 59)
        && (second      >= 0    && second      <= 60)  // allow leap seconds
        && (millisecond >= 0    && millisecond <= 999)
        && (microsecond >= 0    && microsecond <= 999);
}

////////////////////////////////////////////////////////////////
// This class represents an instant in local time, expressed in
// years, months, days, hours, minutes, seconds, milliseconds
// and microseconds.
//
// Windows:
// _mktime64 will return -1 cast to type __time64_t if timeptr
// references a date after 23:59:59, December 31, 3000, UTC.
////////////////////////////////////////////////////////////////
class CORE_API datetime final
{
public:
    // Creates a datetime for the current date and time.
    explicit datetime(void);

    // Creates a datetime for the date and time given in a timestamp.
    explicit datetime(const timestamp& ts);

    // Creates a datetime from tm struct.
    explicit datetime(const tm& tmval);

    // Creates a datetime for the given local date and time.
    //   * year is from 1970 to 3000.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60(allowing leap seconds).
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    explicit datetime(int year, int month, int day, int hour, int minute, int second, int millisecond = 0, int microsecond = 0);

    // Copy & Move constructors.
    datetime(const datetime&) = default;
    datetime(datetime&&) = default;

    // Copy & Move assignments.
    datetime& operator =(const datetime&) = default;
    datetime& operator =(datetime&&) = default;

    // Assigns a timestamp.
    datetime& operator =(const timestamp& ts) {return assign(ts);}

    // Assigns a tm struct.
    datetime& operator =(const tm& tmval) {return assign(tmval);}

    // Assigns a timestamp.
    datetime& assign(const timestamp& ts);

    // Assigns a tm struct.
    datetime& assign(const tm& tmval);

    // Assigns a local date and time.
    //   * year is from 1970 to 3000.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60(allowing leap seconds).
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    datetime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Updates the datetime with the current time.
    void update(void);

    // Returns the year(1970 to 3000).
    int year(void) const {return _tmval.tm_year + 1900;}

    // Returns the month(1 to 12).
    int month(void) const {return _tmval.tm_mon + 1;}

    // Returns the day within the month(1 to 31).
    int day(void) const {return _tmval.tm_mday;}

    // Returns the weekday(0 to 6, where 0=Sunday, 1=Monday, ..., 6=Saturday).
    int day_of_week(void) const {return _tmval.tm_wday;}

    // Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc.
    int day_of_year(void) const {return _tmval.tm_yday + 1;}

    // Returns the hour(0 to 23).
    int hour(void) const {return _tmval.tm_hour;}

    // Returns the minute(0 to 59).
    int minute(void) const {return _tmval.tm_min;}

    // Returns the second(0 to 60, including leap second).
    int second(void) const {return _tmval.tm_sec;}

    // Returns the millisecond(0 to 999)
    int millisecond(void) const {return static_cast<int>((epoch_microseconds() % timestamp::resolution) / 1000);}

    // Returns the microsecond(0 to 999)
    int microsecond(void) const {return static_cast<int>((epoch_microseconds() % timestamp::resolution) % 1000);}

    // Returns the time elapsed since the time denoted by the timestamp. Equivalent to datetime() - *this.
    time_t elapsed(void) const {return _ts.elapsed();}

    // Returns true iff the given interval(expressed in microseconds)
    // has passed since the time denoted by the datetime.
    bool is_elapsed(time_t interval) const {return _ts.is_elapsed(interval);}

    // Returns the timestamp expressed in seconds.
    time_t epoch_time(void) const {return _ts.epoch_time();}

    // Returns the timestamp expressed in microseconds.
    time_t epoch_microseconds(void) const {return _ts.epoch_microseconds();}

    // Converts datetime to timestamp.
    timestamp to_timestamp(void) const {return _ts;}

    // Converts datetime to tm struct.
    tm to_tm(void) const {return _tmval;}

    bool operator ==(const datetime& ts) const {return _ts == ts._ts;}
    bool operator !=(const datetime& ts) const {return _ts != ts._ts;}
    bool operator < (const datetime& ts) const {return _ts <  ts._ts;}
    bool operator <=(const datetime& ts) const {return _ts <= ts._ts;}
    bool operator > (const datetime& ts) const {return _ts >  ts._ts;}
    bool operator >=(const datetime& ts) const {return _ts >= ts._ts;}

    datetime  operator + (const timespan& span) const {return datetime(_ts + span);}
    datetime  operator - (const timespan& span) const {return datetime(_ts - span);}
    datetime& operator +=(const timespan& span) {return assign(_ts + span);}
    datetime& operator -=(const timespan& span) {return assign(_ts - span);}

    timespan operator -(const datetime& ts) const {return timespan(_ts - ts._ts);}

private:
    // Updates _tmval with _ts.
    void update_tm(void);

private:
    timestamp _ts;     // Current timestamp with local time
    tm        _tmval;  // Struct tm
};

} // namespace core

#endif // __DATETIME_H__