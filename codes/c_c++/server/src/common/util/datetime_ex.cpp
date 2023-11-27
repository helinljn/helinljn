#include "datetime_ex.h"
#include "Poco/Timezone.h"

namespace common {

bool datetime_ex::is_leap_year(const int year)
{
    return Poco::DateTime::isLeapYear(year);
}

int datetime_ex::days_of_month(const int year, const int month)
{
    return Poco::DateTime::daysOfMonth(year, month);
}

bool datetime_ex::is_valid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    return Poco::DateTime::isValid(year, month, day, hour, minute, second, millisecond, microsecond);
}

int datetime_ex::tzd(void)
{
    static int tzd = Poco::Timezone::tzd();
    return tzd;
}

datetime_ex::datetime_ex(void)
    : _ts()
    , _dt(_ts + Poco::Timespan(tzd(), 0))
{
}

datetime_ex::datetime_ex(const datetime_ex& dtime)
    : _ts(dtime._ts)
    , _dt(dtime._dt)
{
}

datetime_ex::datetime_ex(const Poco::DateTime& dtime)
    : _ts(0)
    , _dt(dtime.utcTime(), Poco::Timespan(tzd(), 0).totalMicroseconds())
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

datetime_ex::datetime_ex(const Poco::Timestamp& ts)
    : _ts(ts)
    , _dt(_ts + Poco::Timespan(tzd(), 0))
{
}

datetime_ex::datetime_ex(const tm& tmval)
    : _ts(0)
    , _dt(tmval)
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

datetime_ex::datetime_ex(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _dt(year, month, day, hour, minute, second, millisecond, microsecond)
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

datetime_ex& datetime_ex::assign(const datetime_ex& dtime)
{
    if (this != &dtime)
    {
        _ts = dtime._ts;
        _dt = dtime._dt;
    }

    return *this;
}

datetime_ex& datetime_ex::assign(const Poco::DateTime& dtime)
{
    if (utc_time() != dtime.utcTime())
    {
        Poco::Timespan tzd_span(tzd(), 0);

        _dt  = dtime;
        _dt += tzd_span;
        _ts  = _dt.timestamp() - tzd_span;
    }

    return *this;
}

datetime_ex& datetime_ex::assign(const Poco::Timestamp& ts)
{
    if (_ts != ts)
    {
        _ts = ts;
        _dt = _ts + Poco::Timespan(tzd(), 0);
    }

    return *this;
}

datetime_ex& datetime_ex::assign(const tm& tmval)
{
    return assign(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec);
}

datetime_ex& datetime_ex::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
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
        _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
    }

    return *this;
}

void datetime_ex::update(void)
{
    _ts.update();
    _dt = _ts + Poco::Timespan(tzd(), 0);
}

} // namespace common