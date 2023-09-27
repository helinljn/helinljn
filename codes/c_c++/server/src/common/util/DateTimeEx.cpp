#include "util/DateTimeEx.h"
#include "Poco/Timezone.h"

namespace Poco {

bool DateTimeEx::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    return DateTime::isValid(year, month, day, hour, minute, second, millisecond, microsecond);
}

int DateTimeEx::tzd(void)
{
    static int tzd = Timezone::tzd();
    return tzd;
}

DateTimeEx::DateTimeEx(void)
    : _ts()
    , _dt(_ts + Timespan(tzd(), 0))
{
}

DateTimeEx::DateTimeEx(const DateTimeEx& dateTime)
    : _ts(dateTime._ts)
    , _dt(dateTime._dt)
{
}

DateTimeEx::DateTimeEx(const DateTime& dateTime)
    : _ts(0)
    , _dt(dateTime.utcTime(), Timespan(tzd(), 0).totalMicroseconds())
{
    _ts = _dt.timestamp() - Timespan(tzd(), 0);
}

DateTimeEx::DateTimeEx(const Timestamp& timestamp)
    : _ts(timestamp)
    , _dt(_ts + Timespan(tzd(), 0))
{
}

DateTimeEx::DateTimeEx(const tm& tmStruct)
    : _ts(0)
    , _dt(tmStruct)
{
    _ts = _dt.timestamp() - Timespan(tzd(), 0);
}

DateTimeEx::DateTimeEx(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _dt(year, month, day, hour, minute, second, millisecond, microsecond)
{
    _ts = _dt.timestamp() - Timespan(tzd(), 0);
}

DateTimeEx& DateTimeEx::assign(const DateTimeEx& dateTime)
{
    if (this != &dateTime)
    {
        _ts = dateTime._ts;
        _dt = dateTime._dt;
    }

    return *this;
}

DateTimeEx& DateTimeEx::assign(const DateTime& dateTime)
{
    if (utcTime() != dateTime.utcTime())
    {
        Timespan tzdSpan(tzd(), 0);

        _dt  = dateTime;
        _dt += tzdSpan;
        _ts  = _dt.timestamp() - tzdSpan;
    }

    return *this;
}

DateTimeEx& DateTimeEx::assign(const Timestamp& timestamp)
{
    if (_ts != timestamp)
    {
        _ts = timestamp;
        _dt = _ts + Timespan(tzd(), 0);
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
        _ts = _dt.timestamp() - Timespan(tzd(), 0);
    }

    return *this;
}

void DateTimeEx::update(void)
{
    _ts.update();
    _dt = _ts + Timespan(tzd(), 0);
}

} // namespace Poco