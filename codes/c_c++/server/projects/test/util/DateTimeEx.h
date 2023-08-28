#ifndef __DATE_TIME_EX_H__
#define __DATE_TIME_EX_H__

#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/DateTime.h"

namespace Poco {

class Foundation_API DateTimeEx final
    /// This class represents an instant in [local time]
    /// (as opposed to UTC), expressed in years, months, days,
    /// hours, minutes, seconds and milliseconds based on the
    /// Gregorian calendar.
{
public:
    static bool isLeapYear(int year) {return DateTime::isLeapYear(year);}
        /// Returns true if the given year is a leap year;
        /// false otherwise.

    static int daysOfMonth(int year, int month) {return DateTime::daysOfMonth(year, month);}
        /// Returns the number of days in the given month
        /// and year. Month is from 1 to 12.

    static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0)
    {return DateTime::isValid(year, month, day, hour, minute, second, millisecond, microsecond);}
        /// Checks if the given date and time is valid
        /// (all arguments are within a proper range).
        ///
        /// Returns true if all arguments are valid, false otherwise.

public:
    explicit DateTimeEx(void);
        /// Creates a DateTimeEx for the current date and time.

    explicit DateTimeEx(const DateTimeEx& dateTime);
        /// Copy constructor. Creates the DateTimeEx from another one.

    explicit DateTimeEx(const DateTime& dateTime);
        /// Creates a DateTimeEx from the UTC time given in dateTime,
        /// using the time zone differential of the current time zone.

    explicit DateTimeEx(const Timestamp& timestamp);
        /// Creates a DateTimeEx for the date and time given in
        /// a Timestamp.

    explicit DateTimeEx(const tm& tmStruct);
        /// Creates a DateTimeEx from tm struct.

    explicit DateTimeEx(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Creates a DateTimeEx for the given Gregorian local date and time.
        ///   * year is from 0 to 9999.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 60.
        ///   * millisecond is from 0 to 999.
        ///   * microsecond is from 0 to 999.
        ///
        /// Throws an InvalidArgumentException if an argument date is out of range.

    DateTimeEx& operator =(const DateTimeEx& dateTime) {return this != &dateTime ? assign(dateTime) : *this;}
        /// Assigns another DateTimeEx.

    DateTimeEx& operator =(const DateTime& dateTime) {return assign(dateTime);}
        /// Assigns a DateTime.

    DateTimeEx& operator =(const Timestamp& timestamp) {return assign(timestamp);}
        /// Assigns a Timestamp.

    DateTimeEx& operator =(const tm& tmStruct) {return assign(tmStruct);}
        /// Assigns a tm struct.

    DateTimeEx& assign(const DateTimeEx& dateTime);
        /// Assigns another DateTimeEx.

    DateTimeEx& assign(const DateTime& dateTime);
        /// Assigns a DateTime.

    DateTimeEx& assign(const Timestamp& timestamp);
        /// Assigns a Timestamp.

    DateTimeEx& assign(const tm& tmStruct);
        /// Assigns a tm struct.

    DateTimeEx& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Assigns a Gregorian local date and time.
        ///   * year is from 0 to 9999.
        ///   * month is from 1 to 12.
        ///   * day is from 1 to 31.
        ///   * hour is from 0 to 23.
        ///   * minute is from 0 to 59.
        ///   * second is from 0 to 60 (allowing leap seconds).
        ///   * millisecond is from 0 to 999.
        ///   * microsecond is from 0 to 999.

    int year(void) const {return _dt.year();}
        /// Returns the year (0 to 9999).

    int month(void) const {return _dt.month();}
        /// Returns the month (1 to 12).

    int week(int firstDayOfWeek = DateTime::MONDAY) const {return _dt.week(firstDayOfWeek);}
        /// Returns the week number within the year.
        /// FirstDayOfWeek should be either DateTime::SUNDAY (0) or DateTime::MONDAY (1).
        /// The returned week number will be from 0 to 53. Week number 1 is the week
        /// containing January 4. This is in accordance to ISO 8601.
        ///
        /// The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
        /// on a Saturday, week 1 will be the week starting on Monday, January 3.
        /// January 1 and 2 will fall within week 0 (or the last week of the previous year).
        ///
        /// For 2007, which starts on a Monday, week 1 will be the week starting on Monday, January 1.
        /// There will be no week 0 in 2007.

    int day(void) const {return _dt.day();}
        /// Returns the day within the month (1 to 31).

    int dayOfWeek(void) const {return _dt.dayOfWeek();}
        /// Returns the weekday (0 to 6, where
        /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).

    int dayOfYear(void) const {return _dt.dayOfYear();}
        /// Returns the number of the day in the year.
        /// January 1 is 1, February 1 is 32, etc.

    int hour(void) const {return _dt.hour();}
        /// Returns the hour (0 to 23).

    int hourAMPM(void) const {return _dt.hourAMPM();}
        /// Returns the hour (0 to 12).

    bool isAM(void) const {return _dt.isAM();}
        /// Returns true if hour < 12;

    bool isPM(void) const {return _dt.isPM();}
        /// Returns true if hour >= 12.

    int minute(void) const {return _dt.minute();}
        /// Returns the minute (0 to 59).

    int second(void) const {return _dt.second();}
        /// Returns the second (0 to 59).

    int millisecond(void) const {return _dt.millisecond();}
        /// Returns the millisecond (0 to 999)

    int microsecond(void) const {return _dt.microsecond();}
        /// Returns the microsecond (0 to 999)

    int tzd(void) const {return _tzd;}
        /// Returns the time zone differential.

    Timestamp timestamp(void) const {return _ts;}
        /// Returns the date and time expressed as a Timestamp.

    DateTime utc(void) const {return DateTime(_dt.utcTime(), -(Timestamp::TimeDiff(_tzd) * Timespan::SECONDS));}
        /// Returns the UTC equivalent for the local date and time.

    DateTime utcLocal(void) const {return _dt;}
        /// Returns the local date and time computed as (_ts + _tzd).

    Timestamp::UtcTimeVal utcTime(void) const {return _dt.utcTime() - Timestamp::TimeDiff(_tzd) * Timespan::SECONDS * 10;}
        /// Returns the UTC equivalent for the local date and time.

    Timestamp::UtcTimeVal utcLocalTime(void) const {return _dt.utcTime();}
        /// Returns the local date and time computed as (_ts + _tzd).

    tm makeTM(void) const {return _dt.makeTM();}
        /// Converts DateTime to tm struct.

    bool operator ==(const DateTimeEx& dateTime) const {return _dt == dateTime._dt;}
    bool operator !=(const DateTimeEx& dateTime) const {return _dt != dateTime._dt;}
    bool operator < (const DateTimeEx& dateTime) const {return _dt <  dateTime._dt;}
    bool operator <=(const DateTimeEx& dateTime) const {return _dt <= dateTime._dt;}
    bool operator > (const DateTimeEx& dateTime) const {return _dt >  dateTime._dt;}
    bool operator >=(const DateTimeEx& dateTime) const {return _dt >= dateTime._dt;}

    DateTimeEx  operator + (const Timespan& span) const {return DateTimeEx(_ts + span);}
    DateTimeEx  operator - (const Timespan& span) const {return DateTimeEx(_ts - span);}
    DateTimeEx& operator +=(const Timespan& span) {return assign(_ts + span);}
    DateTimeEx& operator -=(const Timespan& span) {return assign(_ts - span);}

    Timespan operator -(const DateTime&   dateTime) const {return utc() - dateTime;}
    Timespan operator -(const DateTimeEx& dateTime) const {return Timespan((_dt.utcTime() - dateTime._dt.utcTime()) / 10);}

private:
    int             _tzd;  /// Time zone differential for the current timezone
    Poco::Timestamp _ts;   /// Current timestamp with [local time]
    Poco::DateTime  _dt;   /// Local datetime is computed as (_ts + _tzd)
};

} // namespace Poco

#endif // __DATE_TIME_EX_H__