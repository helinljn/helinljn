#ifndef __DATE_TIME_EX_H__
#define __DATE_TIME_EX_H__

#include "util/types.h"
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/DateTime.h"

namespace common {

////////////////////////////////////////////////////////////////
// This class represents an instant in [local time]
// (as opposed to UTC), expressed in years, months, days,
// hours, minutes, seconds and milliseconds based on the
// Gregorian calendar.
////////////////////////////////////////////////////////////////
class COMMON_API date_time_ex final
{
public:
    // Returns true if the given year is a leap year, false otherwise.
    static bool is_leap_year(int year);

    // Returns the number of days in the given month and year. Month is from 1 to 12.
    static int days_of_month(int year, int month);

    // Checks if the given date and time is valid(all arguments are within a proper range).
    // Returns true if all arguments are valid, false otherwise.
    static bool is_valid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Returns the time zone differential for the current timezone.
    // The timezone differential is computed as utcOffset() + dst() and is expressed in seconds.
    static int tzd(void);

public:
    // Creates a date_time_ex for the current date and time.
    explicit date_time_ex(void);

    // Copy constructor. Creates the date_time_ex from another one.
    explicit date_time_ex(const date_time_ex& dtime);

    // Creates a date_time_ex from the UTC time given in dtime,
    // using the time zone differential of the current time zone.
    explicit date_time_ex(const Poco::DateTime& dtime);

    // Creates a date_time_ex for the date and time given in a Poco::Timestamp.
    explicit date_time_ex(const Poco::Timestamp& ts);

    // Creates a date_time_ex from tm struct.
    explicit date_time_ex(const tm& tmval);

    // Creates a date_time_ex for the given Gregorian local date and time.
    //   * year is from 0 to 9999.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60.
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    explicit date_time_ex(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Assigns another date_time_ex.
    date_time_ex& operator =(const date_time_ex& dtime) {return assign(dtime);}

    // Assigns a Poco::DateTime.
    date_time_ex& operator =(const Poco::DateTime& dtime) {return assign(dtime);}

    // Assigns a Poco::Timestamp.
    date_time_ex& operator =(const Poco::Timestamp& ts) {return assign(ts);}

    // Assigns a tm struct.
    date_time_ex& operator =(const tm& tmval) {return assign(tmval);}

    // Assigns another date_time_ex.
    date_time_ex& assign(const date_time_ex& dtime);

    // Assigns a Poco::DateTime.
    date_time_ex& assign(const Poco::DateTime& dtime);

    // Assigns a Poco::Timestamp.
    date_time_ex& assign(const Poco::Timestamp& ts);

    // Assigns a tm struct.
    date_time_ex& assign(const tm& tmval);

    // Assigns a Gregorian local date and time.
    //   * year is from 0 to 9999.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60 (allowing leap seconds).
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    date_time_ex& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Updates the date_time_ex with the current time.
    void update(void);

    // Returns the year (0 to 9999).
    int year(void) const {return _dt.year();}

    // Returns the month (1 to 12).
    int month(void) const {return _dt.month();}

    // Returns the week number within the year.
    // FirstDayOfWeek should be either DateTime::SUNDAY (0) or DateTime::MONDAY (1).
    // The returned week number will be from 0 to 53. Week number 1 is the week
    // containing January 4. This is in accordance to ISO 8601.
    //
    // The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
    // on a Saturday, week 1 will be the week starting on Monday, January 3.
    // January 1 and 2 will fall within week 0 (or the last week of the previous year).
    //
    // For 2007, which starts on a Monday, week 1 will be the week starting on Monday, January 1.
    // There will be no week 0 in 2007.
    int week(int firstDayOfWeek = Poco::DateTime::MONDAY) const {return _dt.week(firstDayOfWeek);}

    // Returns the day within the month (1 to 31).
    int day(void) const {return _dt.day();}

    // Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int day_of_week(void) const {return _dt.dayOfWeek();}

    // Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc.
    int day_of_year(void) const {return _dt.dayOfYear();}

    // Returns the hour (0 to 23).
    int hour(void) const {return _dt.hour();}

    // Returns the hour (0 to 12).
    int hour_am_pm(void) const {return _dt.hourAMPM();}

    // Returns true if hour < 12;
    bool is_am(void) const {return _dt.isAM();}

    // Returns true if hour >= 12.
    bool is_pm(void) const {return _dt.isPM();}

    // Returns the minute (0 to 59).
    int minute(void) const {return _dt.minute();}

    // Returns the second (0 to 59).
    int second(void) const {return _dt.second();}

    // Returns the millisecond (0 to 999)
    int millisecond(void) const {return _dt.millisecond();}

    // Returns the microsecond (0 to 999)
    int microsecond(void) const {return _dt.microsecond();}

    // Returns the time elapsed since the time denoted by the timestamp. Equivalent to date_time_ex() - *this.
    time_t elapsed(void) const {return _ts.elapsed();}

    // Returns true iff the given interval has passed since the time denoted by the timestamp.
    bool is_elapsed(const time_t interval) const {return _ts.isElapsed(interval);}

    // Returns the timestamp expressed in time_t.
    time_t epoch_time(void) const {return _ts.epochTime();}

    // Returns the timestamp expressed in microseconds.
    time_t epoch_microseconds(void) const {return _ts.epochMicroseconds();}

    // Returns the date and time expressed as a Poco::Timestamp.
    Poco::Timestamp timestamp(void) const {return _ts;}

    // Returns the UTC equivalent for the local date and time.
    Poco::DateTime utc(void) const {return Poco::DateTime(_dt.utcTime(), -(Poco::Timespan(tzd(), 0).totalMicroseconds()));}

    // Returns the local date and time computed as (_ts + tzd()).
    Poco::DateTime utc_local(void) const {return _dt;}

    // Returns the UTC equivalent for the local date and time.
    Poco::Timestamp::UtcTimeVal utc_time(void) const {return _dt.utcTime() - Poco::Timespan(tzd(), 0).totalMicroseconds() * 10;}

    // Returns the local date and time computed as (_ts + tzd()).
    Poco::Timestamp::UtcTimeVal utc_local_time(void) const {return _dt.utcTime();}

    // Converts date_time_ex to tm struct.
    tm make_tm(void) const {return _dt.makeTM();}

    bool operator ==(const date_time_ex& dtime) const {return _dt == dtime._dt;}
    bool operator !=(const date_time_ex& dtime) const {return _dt != dtime._dt;}
    bool operator < (const date_time_ex& dtime) const {return _dt <  dtime._dt;}
    bool operator <=(const date_time_ex& dtime) const {return _dt <= dtime._dt;}
    bool operator > (const date_time_ex& dtime) const {return _dt >  dtime._dt;}
    bool operator >=(const date_time_ex& dtime) const {return _dt >= dtime._dt;}

    date_time_ex  operator + (const Poco::Timespan& span) const {return date_time_ex(_ts + span);}
    date_time_ex  operator - (const Poco::Timespan& span) const {return date_time_ex(_ts - span);}
    date_time_ex& operator +=(const Poco::Timespan& span) {return assign(_ts + span);}
    date_time_ex& operator -=(const Poco::Timespan& span) {return assign(_ts - span);}

    Poco::Timespan operator -(const Poco::DateTime& dtime) const {return utc() - dtime;}
    Poco::Timespan operator -(const date_time_ex&   dtime) const {return Poco::Timespan((_dt.utcTime() - dtime._dt.utcTime()) / 10);}

private:
    Poco::Timestamp _ts;  // Current timestamp with [local time]
    Poco::DateTime  _dt;  // Local datetime is computed as (_ts + tzd())
};

} // namespace common

#endif // __DATE_TIME_EX_H__