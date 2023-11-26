#ifndef __TIMESTAMP_EX_H__
#define __TIMESTAMP_EX_H__

#include "util/types.h"
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/DateTime.h"

namespace common {

////////////////////////////////////////////////////////////////
// A timestamp_ex stores a monotonic* time value
// with (theoretical) microseconds resolution.
// Timestamps can be compared with each other
// and simple arithmetic is supported.
//
// The internal reference time is the Unix epoch,
// midnight, January 1, 1970.
////////////////////////////////////////////////////////////////
class COMMON_API timestamp_ex final
{
public:
    // Creates a timestamp_ex with the current local time.
    explicit timestamp_ex(void);

    // Copy constructor. Creates the timestamp_ex from another one.
    explicit timestamp_ex(const timestamp_ex& ts);

    // Creates a timestamp_ex from Poco::DateTime.
    explicit timestamp_ex(const Poco::DateTime& dtime);

    // Creates a timestamp_ex from Poco::Timestamp.
    explicit timestamp_ex(const Poco::Timestamp& ts);

    // Creates a timestamp_ex from Unix epoch.
    explicit timestamp_ex(const time_t epoch_time);

    // Creates a timestamp_ex from tm struct.
    explicit timestamp_ex(const tm& tmval);

    // Creates a timestamp_ex for the given Gregorian local date and time.
    //   * year is from 1970 to 9999.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60.
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    explicit timestamp_ex(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Assigns another timestamp_ex.
    timestamp_ex& operator =(const timestamp_ex& ts) {return assign(ts);}

    // Assigns a Poco::DateTime.
    timestamp_ex& operator =(const Poco::DateTime& dtime) {return assign(dtime);}

    // Assigns a Poco::Timestamp.
    timestamp_ex& operator =(const Poco::Timestamp& ts) {return assign(ts);}

    // Assigns a Unix epoch.
    timestamp_ex& operator =(const time_t epoch_time) {return assign(epoch_time);}

    // Assigns a tm struct.
    timestamp_ex& operator =(const tm& tmval) {return assign(tmval);}

    // Assigns another timestamp_ex.
    timestamp_ex& assign(const timestamp_ex& ts);

    // Assigns a Poco::DateTime.
    timestamp_ex& assign(const Poco::DateTime& dtime);

    // Assigns a Poco::Timestamp.
    timestamp_ex& assign(const Poco::Timestamp& ts);

    // Assigns a Unix epoch.
    timestamp_ex& assign(const time_t epoch_time);

    // Assigns a tm struct.
    timestamp_ex& assign(const tm& tmval);

    // Assigns a Gregorian local date and time.
    //   * year is from 1970 to 9999.
    //   * month is from 1 to 12.
    //   * day is from 1 to 31.
    //   * hour is from 0 to 23.
    //   * minute is from 0 to 59.
    //   * second is from 0 to 60.
    //   * millisecond is from 0 to 999.
    //   * microsecond is from 0 to 999.
    timestamp_ex& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    // Updates the timestamp_ex with the current time.
    void update(void);

    // Returns the year (1970 to 9999).
    int year(void) const {return _tmval.tm_year + 1900;}

    // Returns the month (1 to 12).
    int month(void) const {return _tmval.tm_mon + 1;}

    // Returns the day within the month (1 to 31).
    int day(void) const {return _tmval.tm_mday;}

    // Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int day_of_week(void) const {return _tmval.tm_wday;}

    // Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc.
    int day_of_year(void) const {return _tmval.tm_yday + 1;}

    // Returns the hour (0 to 23).
    int hour(void) const {return _tmval.tm_hour;}

    // Returns the minute (0 to 59).
    int minute(void) const {return _tmval.tm_min;}

    // Returns the second (0 to 59).
    int second(void) const {return _tmval.tm_sec;}

    // Returns the millisecond (0 to 999)
    int millisecond(void) const {return static_cast<int>((epoch_microseconds() % Poco::Timestamp::resolution()) / 1000);}

    // Returns the microsecond (0 to 999)
    int microsecond(void) const {return static_cast<int>((epoch_microseconds() % Poco::Timestamp::resolution()) % 1000);}

    // Returns the time elapsed since the time denoted by the timestamp. Equivalent to timestamp_ex() - *this.
    time_t elapsed(void) const {return _ts.elapsed();}

    // Returns true iff the given interval has passed since the time denoted by the timestamp.
    bool is_elapsed(const time_t interval) const {return _ts.isElapsed(interval);}

    // Returns the timestamp expressed in time_t.
    time_t epoch_time(void) const {return _ts.epochTime();}

    // Returns the timestamp expressed in microseconds.
    time_t epoch_microseconds(void) const {return _ts.epochMicroseconds();}

    // Returns the date and time expressed as a Poco::Timestamp.
    Poco::Timestamp timestamp(void) const {return _ts;}

    // Converts timestamp_ex to tm struct.
    tm make_tm(void) const {return _tmval;}

    bool operator ==(const timestamp_ex& ts) const {return _ts == ts._ts;}
    bool operator !=(const timestamp_ex& ts) const {return _ts != ts._ts;}
    bool operator < (const timestamp_ex& ts) const {return _ts <  ts._ts;}
    bool operator <=(const timestamp_ex& ts) const {return _ts <= ts._ts;}
    bool operator > (const timestamp_ex& ts) const {return _ts >  ts._ts;}
    bool operator >=(const timestamp_ex& ts) const {return _ts >= ts._ts;}

    timestamp_ex  operator + (const Poco::Timespan& span) const {return timestamp_ex(_ts + span);}
    timestamp_ex  operator - (const Poco::Timespan& span) const {return timestamp_ex(_ts - span);}
    timestamp_ex& operator +=(const Poco::Timespan& span) {return assign(_ts + span);}
    timestamp_ex& operator -=(const Poco::Timespan& span) {return assign(_ts - span);}

    Poco::Timespan operator -(const Poco::Timestamp& ts) const {return Poco::Timespan(_ts - ts);}
    Poco::Timespan operator -(const timestamp_ex&    ts) const {return Poco::Timespan(_ts - ts._ts);}

private:
    // Updates _tmval with _ts.
    void update_tm(void);

private:
    Poco::Timestamp _ts;
    tm              _tmval;
};

} // namespace common

#endif // __TIMESTAMP_EX_H__