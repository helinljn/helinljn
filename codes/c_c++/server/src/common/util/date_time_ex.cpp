#include "date_time_ex.h"
#include "Poco/Timezone.h"

namespace common {

bool date_time_ex::is_leap_year(int year)
{
    return Poco::DateTime::isLeapYear(year);
}

int date_time_ex::days_of_month(int year, int month)
{
    return Poco::DateTime::daysOfMonth(year, month);
}

bool date_time_ex::is_valid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    return Poco::DateTime::isValid(year, month, day, hour, minute, second, millisecond, microsecond);
}

int date_time_ex::tzd(void)
{
    static int tzd = Poco::Timezone::tzd();
    return tzd;
}

date_time_ex::date_time_ex(void)
    : _ts()
    , _dt(_ts + Poco::Timespan(tzd(), 0))
{
}

date_time_ex::date_time_ex(const date_time_ex& dtime)
    : _ts(dtime._ts)
    , _dt(dtime._dt)
{
}

date_time_ex::date_time_ex(const Poco::DateTime& dtime)
    : _ts(0)
    , _dt(dtime.utcTime(), Poco::Timespan(tzd(), 0).totalMicroseconds())
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

date_time_ex::date_time_ex(const Poco::Timestamp& ts)
    : _ts(ts)
    , _dt(_ts + Poco::Timespan(tzd(), 0))
{
}

date_time_ex::date_time_ex(const tm& tmval)
    : _ts(0)
    , _dt(tmval)
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

date_time_ex::date_time_ex(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _dt(year, month, day, hour, minute, second, millisecond, microsecond)
{
    _ts = _dt.timestamp() - Poco::Timespan(tzd(), 0);
}

date_time_ex& date_time_ex::assign(const date_time_ex& dtime)
{
    if (this != &dtime)
    {
        _ts = dtime._ts;
        _dt = dtime._dt;
    }

    return *this;
}

date_time_ex& date_time_ex::assign(const Poco::DateTime& dtime)
{
    if (utc_time() != dtime.utcTime())
    {
        Poco::Timespan tzdSpan(tzd(), 0);

        _dt  = dtime;
        _dt += tzdSpan;
        _ts  = _dt.timestamp() - tzdSpan;
    }

    return *this;
}

date_time_ex& date_time_ex::assign(const Poco::Timestamp& ts)
{
    if (_ts != ts)
    {
        _ts = ts;
        _dt = _ts + Poco::Timespan(tzd(), 0);
    }

    return *this;
}

date_time_ex& date_time_ex::assign(const tm& tmval)
{
    return assign(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec);
}

date_time_ex& date_time_ex::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
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

void date_time_ex::update(void)
{
    _ts.update();
    _dt = _ts + Poco::Timespan(tzd(), 0);
}

} // namespace common