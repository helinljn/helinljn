#include "util/DateTimeEx.h"
#include "Poco/Timezone.h"

namespace Poco {

DateTimeEx::DateTimeEx(void)
    : _tzd(Timezone::tzd())
    , _ts()
    , _dt(_ts + Timespan(_tzd, 0))
{
}

DateTimeEx::DateTimeEx(const DateTimeEx& dateTime)
    : _tzd(dateTime._tzd)
    , _ts(dateTime._ts)
    , _dt(dateTime._dt)
{
}

DateTimeEx::DateTimeEx(const DateTime& dateTime)
    : _tzd(Timezone::tzd())
    , _ts(0)
    , _dt(dateTime.utcTime(), Timestamp::TimeDiff(_tzd) * Timespan::SECONDS)
{
    _ts = _dt.timestamp() - Timespan(_tzd, 0);
}

DateTimeEx::DateTimeEx(const Timestamp& timestamp)
    : _tzd(Timezone::tzd())
    , _ts(timestamp)
    , _dt(_ts + Timespan(_tzd, 0))
{
}

DateTimeEx::DateTimeEx(const tm& tmStruct)
    : _tzd(Timezone::tzd())
    , _ts(0)
    , _dt(tmStruct)
{
    _ts = _dt.timestamp() - Timespan(_tzd, 0);
}

DateTimeEx::DateTimeEx(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _tzd(Timezone::tzd())
    , _ts(0)
    , _dt(year, month, day, hour, minute, second, millisecond, microsecond)
{
    _ts = _dt.timestamp() - Timespan(_tzd, 0);
}

DateTimeEx& DateTimeEx::assign(const DateTimeEx& dateTime)
{
    if (this != &dateTime)
    {
        _tzd = dateTime._tzd;
        _ts  = dateTime._ts;
        _dt  = dateTime._dt;
    }

    return *this;
}

DateTimeEx& DateTimeEx::assign(const DateTime& dateTime)
{
    if (utcTime() != dateTime.utcTime())
    {
        _dt  = dateTime;
        _dt += Timespan(_tzd, 0);
        _ts  = _dt.timestamp() - Timespan(_tzd, 0);
    }

    return *this;
}

DateTimeEx& DateTimeEx::assign(const Timestamp& timestamp)
{
    if (_ts != timestamp)
    {
        _ts = timestamp;
        _dt = _ts + Timespan(_tzd, 0);
    }

    return *this;
}

DateTimeEx& DateTimeEx::assign(const tm& tmStruct)
{
    return assign(tmStruct.tm_year + 1900, tmStruct.tm_mon + 1, tmStruct.tm_mday, tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec);
}

DateTimeEx& DateTimeEx::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    if (   this->year()        != year
        || this->month()       != month
        || this->day()         != day
        || this->hour()        != hour
        || this->minute()      != minute
        || this->second()      != second
        || this->millisecond() != millisecond
        || this->microsecond() != microsecond)
    {
        _dt.assign(year, month, day, hour, minute, second, millisecond, microsecond);
        _ts = _dt.timestamp() - Timespan(_tzd, 0);
    }

    return *this;
}

} // namespace Poco