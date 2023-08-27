#ifndef __DATE_TIME_EX_H__
#define __DATE_TIME_EX_H__

#include "Poco/Timespan.h"
#include "Poco/Timezone.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTime.h"

namespace Poco {

class Foundation_API DateTimeEx final
    /// This class represents an instant in local time
    /// (as opposed to UTC), expressed in years, months, days,
    /// hours, minutes, seconds and milliseconds based on the
    /// Gregorian calendar.
{
public:
    static bool isLeapYear(int year);
        /// Returns true if the given year is a leap year;
        /// false otherwise.

    static int daysOfMonth(int year, int month);
        /// Returns the number of days in the given month
        /// and year. Month is from 1 to 12.

    static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Checks if the given date and time is valid
        /// (all arguments are within a proper range).
        ///
        /// Returns true if all arguments are valid, false otherwise.

public:
    explicit DateTimeEx(void);
        /// Creates a DateTimeEx for the current date and time.

    explicit DateTimeEx(const DateTimeEx& dateTime);
        /// Copy constructor. Creates the DateTimeEx from another one.

    explicit DateTimeEx(const Timestamp& timestamp);
        /// Creates a DateTimeEx for the date and time given in
        /// a Timestamp.

    explicit DateTimeEx(const tm& tmStruct);
        /// Creates a DateTimeEx from tm struct.

    explicit DateTimeEx(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
        /// Creates a DateTimeEx for the given Gregorian date and time.
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

    DateTimeEx& operator =(const DateTimeEx& dateTime);
        /// Assigns another DateTimeEx.

    DateTimeEx& operator =(const Timestamp& timestamp);
        /// Assigns a Timestamp.

    DateTimeEx& operator =(const tm& tmStruct);
        /// Assigns a tm struct.

//private:
    int             _tzd;  /// Time zone differential for the current timezone
    Poco::Timestamp _ts;   /// Current timestamp + _tzd
    Poco::DateTime  _dt;   /// UTC datetime
};

} // namespace Poco

#endif // __DATE_TIME_EX_H__