#include "timestamp_ex.h"
#include "util/datetime_ex.h"
#include "Poco/Exception.h"

namespace common {

timestamp_ex::timestamp_ex(void)
    : _ts()
    , _tmval()
{
    update_tm();
}

timestamp_ex::timestamp_ex(const timestamp_ex& ts)
    : _ts(ts._ts)
    , _tmval(ts._tmval)
{
}

timestamp_ex::timestamp_ex(const Poco::DateTime& dtime)
    : _ts(0)
    , _tmval()
{
    assign(dtime);
}

timestamp_ex::timestamp_ex(const Poco::Timestamp& ts)
    : _ts(ts)
    , _tmval()
{
    update_tm();
}

timestamp_ex::timestamp_ex(const time_t epoch_time)
    : _ts(0)
    , _tmval()
{
    assign(epoch_time);
}

timestamp_ex::timestamp_ex(const tm& tmval)
    : timestamp_ex(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec)
{
}

timestamp_ex::timestamp_ex(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _tmval()
{
    assign(year, month, day, hour, minute, second, millisecond, microsecond);
}

timestamp_ex& timestamp_ex::assign(const timestamp_ex& ts)
{
    if (this != &ts)
    {
        _ts    = ts._ts;
        _tmval = ts._tmval;
    }

    return *this;
}

timestamp_ex& timestamp_ex::assign(const Poco::DateTime& dtime)
{
    if (Poco::Timestamp tempts = dtime.timestamp(); _ts != tempts)
    {
        _ts = tempts;
        update_tm();
    }

    return *this;
}

timestamp_ex& timestamp_ex::assign(const Poco::Timestamp& ts)
{
    if (epoch_microseconds() != ts.epochMicroseconds())
    {
        _ts = ts;
        update_tm();
    }

    return *this;
}

timestamp_ex& timestamp_ex::assign(const time_t epoch_time)
{
    if (this->epoch_time() != epoch_time)
    {
        _ts = Poco::Timestamp::fromEpochTime(epoch_time);
        update_tm();
    }

    return *this;
}

timestamp_ex& timestamp_ex::assign(const tm& tmval)
{
    return assign(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec);
}

timestamp_ex& timestamp_ex::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    poco_assert(year >= 1970 && year <= 9999);
    poco_assert(month >= 1 && month <= 12);
    poco_assert(day >= 1 && day <= datetime_ex::days_of_month(year, month));
    poco_assert(hour >= 0 && hour <= 23);
    poco_assert(minute >= 0 && minute <= 59);
    poco_assert(second >= 0 && second <= 60);
    poco_assert(millisecond >= 0 && millisecond <= 999);
    poco_assert(microsecond >= 0 && microsecond <= 999);

    if (   this->year()        != year
        || this->month()       != month
        || this->day()         != day
        || this->hour()        != hour
        || this->minute()      != minute
        || this->second()      != second
        || this->millisecond() != millisecond
        || this->microsecond() != microsecond)
    {
        tm tmval{};
        tmval.tm_year  = year - 1900;
        tmval.tm_mon   = month - 1;
        tmval.tm_mday  = day;
        tmval.tm_hour  = hour;
        tmval.tm_min   = minute;
        tmval.tm_sec   = second;
        tmval.tm_isdst = -1;

        _ts  = Poco::Timestamp::fromEpochTime(mktime(&tmval));
        _ts += Poco::Timespan::TimeDiff(millisecond) * Poco::Timespan::MILLISECONDS;
        _ts += microsecond;

        update_tm();
    }

    return *this;
}

void timestamp_ex::update(void)
{
    _ts.update();
    update_tm();
}

void timestamp_ex::update_tm(void)
{
    const time_t t = _ts.epochTime();
    poco_assert(t >= 0);

#if POCO_OS == POCO_OS_WINDOWS_NT
    if (localtime_s(&_tmval, &t) != 0)
        throw Poco::SystemException("cannot get tm struct of timestamp");
#else
    if (!localtime_r(&t, &_tmval))
        throw Poco::SystemException("cannot get tm struct of timestamp");
#endif

    _tmval.tm_isdst = -1;
}

} // namespace common