#include "util/DateTimeEx.h"

namespace Poco {

bool DateTimeEx::isLeapYear(int year)
{
    return DateTime::isLeapYear(year);
}

int DateTimeEx::daysOfMonth(int year, int month)
{
    return DateTime::daysOfMonth(year, month);
}

bool DateTimeEx::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    return DateTime::isValid(year, month, day, hour, minute, second, millisecond, microsecond);
}

DateTimeEx::DateTimeEx(void)
    : _tzd(Timezone::utcOffset() + Timezone::dst(_ts))
    , _ts()
    , _dt(_ts)
{
}

DateTimeEx::DateTimeEx(const DateTimeEx& dateTime)
    : _tzd(dateTime._tzd)
    , _ts(dateTime._ts)
    , _dt(dateTime._dt)
{
}

DateTimeEx::DateTimeEx(const Timestamp& timestamp)
    : _tzd(Timezone::utcOffset() + Timezone::dst(_ts))
    , _ts(timestamp)
    , _dt(_ts)
{
}

DateTimeEx::DateTimeEx(const tm& tmStruct)
    : _tzd(Timezone::utcOffset() + Timezone::dst(_ts))
    , _ts(0)
    , _dt(tmStruct)
{
}

DateTimeEx::DateTimeEx(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _tzd(Timezone::utcOffset() + Timezone::dst(_ts))
    , _ts(0)
    , _dt(year, month, day, hour, minute, second, millisecond, microsecond)
{
}

} // namespace Poco