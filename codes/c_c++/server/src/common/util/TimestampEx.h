#ifndef __TIMESTAMP_EX_H__
#define __TIMESTAMP_EX_H__

#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/DateTime.h"

namespace Poco {

class Foundation_API TimestampEx final
    /// A TimestampEx stores a monotonic* time value
    /// with (theoretical) microseconds resolution.
    /// Timestamps can be compared with each other
    /// and simple arithmetic is supported.
    ///
    /// The internal reference time is the Unix epoch,
    /// midnight, January 1, 1970.
{
public:
    explicit TimestampEx(void);
        /// Creates a TimestampEx with the current local time.

    explicit TimestampEx(const TimestampEx& timestamp);
        /// Copy constructor. Creates the TimestampEx from another one.

    explicit TimestampEx(const DateTime& dateTime);
        /// Creates a TimestampEx from DateTime.

    explicit TimestampEx(const Timestamp& timestamp);
        /// Creates a TimestampEx from Timestamp.

    explicit TimestampEx(const time_t epochTime);
        /// Creates a TimestampEx from Unix epoch.

    explicit TimestampEx(const tm& tmStruct);
        /// Creates a TimestampEx from tm struct.

    explicit TimestampEx(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Creates a TimestampEx for the given Gregorian local date and time.
        ///   * year is from 1970 to 9999.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 60.
        ///   * millisecond is from 0 to 999.
        ///   * microsecond is from 0 to 999.

    TimestampEx& operator =(const TimestampEx& timestamp) {return assign(timestamp);}
        /// Assigns another TimestampEx.

    TimestampEx& operator =(const DateTime& dateTime) {return assign(dateTime);}
        /// Assigns a DateTime.

    TimestampEx& operator =(const Timestamp& timestamp) {return assign(timestamp);}
        /// Assigns a Timestamp.

    TimestampEx& operator =(const time_t epochTime) {return assign(epochTime);}
        /// Assigns a Unix epoch.

    TimestampEx& operator =(const tm& tmStruct) {return assign(tmStruct);}
        /// Assigns a tm struct.

    TimestampEx& assign(const TimestampEx& timestamp);
        /// Assigns another TimestampEx.

    TimestampEx& assign(const DateTime& dateTime);
        /// Assigns a DateTime.

    TimestampEx& assign(const Timestamp& timestamp);
        /// Assigns a Timestamp.

    TimestampEx& assign(const time_t epochTime);
        /// Assigns a Unix epoch.

    TimestampEx& assign(const tm& tmStruct);
        /// Assigns a tm struct.

    TimestampEx& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Assigns a Gregorian local date and time.
        ///   * year is from 1970 to 9999.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 60.
        ///   * millisecond is from 0 to 999.
        ///   * microsecond is from 0 to 999.

    void update(void);
        /// Updates the TimestampEx with the current time.

    int year(void) const {return _tmStruct.tm_year + 1900;}
        /// Returns the year (1970 to 9999).

    int month(void) const {return _tmStruct.tm_mon + 1;}
        /// Returns the month (1 to 12).

    int day(void) const {return _tmStruct.tm_mday;}
        /// Returns the day within the month (1 to 31).

    int dayOfWeek(void) const {return _tmStruct.tm_wday;}
        /// Returns the weekday (0 to 6, where
        /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).

    int dayOfYear(void) const {return _tmStruct.tm_yday + 1;}
        /// Returns the number of the day in the year.
        /// January 1 is 1, February 1 is 32, etc.

    int hour(void) const {return _tmStruct.tm_hour;}
        /// Returns the hour (0 to 23).

    int minute(void) const {return _tmStruct.tm_min;}
        /// Returns the minute (0 to 59).

    int second(void) const {return _tmStruct.tm_sec;}
        /// Returns the second (0 to 59).

    int millisecond(void) const {return static_cast<int>((epochMicroseconds() % Timestamp::resolution()) / 1000);}
        /// Returns the millisecond (0 to 999)

    int microsecond(void) const {return static_cast<int>((epochMicroseconds() % Timestamp::resolution()) % 1000);}
        /// Returns the microsecond (0 to 999)

    time_t elapsed(void) const {return _ts.elapsed();}
        /// Returns the time elapsed since the time denoted by
        /// the timestamp. Equivalent to TimestampEx() - *this.

    bool isElapsed(const time_t interval) const {return _ts.isElapsed(interval);}
        /// Returns true iff the given interval has passed
        /// since the time denoted by the timestamp.

    time_t epochTime(void) const {return _ts.epochTime();}
        /// Returns the timestamp expressed in time_t.

    time_t epochMicroseconds(void) const {return _ts.epochMicroseconds();}
        /// Returns the timestamp expressed in microseconds.

    Timestamp timestamp(void) const {return _ts;}
        /// Returns the date and time expressed as a Timestamp.

    tm makeTM(void) const {return _tmStruct;}
        /// Converts TimestampEx to tm struct.

    bool operator ==(const TimestampEx& timestamp) const {return _ts == timestamp._ts;}
    bool operator !=(const TimestampEx& timestamp) const {return _ts != timestamp._ts;}
    bool operator < (const TimestampEx& timestamp) const {return _ts <  timestamp._ts;}
    bool operator <=(const TimestampEx& timestamp) const {return _ts <= timestamp._ts;}
    bool operator > (const TimestampEx& timestamp) const {return _ts >  timestamp._ts;}
    bool operator >=(const TimestampEx& timestamp) const {return _ts >= timestamp._ts;}

    TimestampEx  operator + (const Timespan& span) const {return TimestampEx(_ts + span);}
    TimestampEx  operator - (const Timespan& span) const {return TimestampEx(_ts - span);}
    TimestampEx& operator +=(const Timespan& span) {return assign(_ts + span);}
    TimestampEx& operator -=(const Timespan& span) {return assign(_ts - span);}

    Timespan operator -(const Timestamp&   timestamp) const {return Timespan(_ts - timestamp);}
    Timespan operator -(const TimestampEx& timestamp) const {return Timespan(_ts - timestamp._ts);}

private:
    void updateTM(void);
        /// Updates _tmStruct with _ts.

private:
    Poco::Timestamp _ts;
    tm              _tmStruct;
};

} // namespace Poco

#endif // __TIMESTAMP_EX_H__