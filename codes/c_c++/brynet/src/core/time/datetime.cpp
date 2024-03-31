#include "datetime.h"
#include <stdexcept>

namespace core {

datetime::datetime(void)
    : _ts()
    , _tmval()
{
    update_tm();
}

datetime::datetime(const timestamp& ts)
    : _ts(ts)
    , _tmval()
{
    update_tm();
}

datetime::datetime(const tm& tmval)
    : datetime(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec)
{
}

datetime::datetime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _tmval()
{
    assign(year, month, day, hour, minute, second, millisecond, microsecond);
}

datetime& datetime::assign(const timestamp& ts)
{
    if (_ts != ts)
    {
        _ts = ts;
        update_tm();
    }

    return *this;
}

datetime& datetime::assign(const tm& tmval)
{
    return assign(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec);
}

datetime& datetime::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    if (!is_datetime_valid(year, month, day, hour, minute, second, millisecond, microsecond))
        throw std::runtime_error("invalid date time!");

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

        _ts  = mktime(&tmval) * timestamp::resolution;
        _ts += time_t{millisecond} * 1000;
        _ts += microsecond;

        update_tm();
    }

    return *this;
}

void datetime::update(void)
{
    _ts.update();
    update_tm();
}

void datetime::update_tm(void)
{
    const time_t t = _ts.epoch_time();
    if (t < 0)
        throw std::runtime_error("cannot get epoch time!");

#if defined(CORE_PLATFORM_WINDOWS)
    if (localtime_s(&_tmval, &t) != 0)
        throw std::runtime_error("cannot get tm struct of timestamp!");
#elif defined(CORE_PLATFORM_LINUX)
    if (!localtime_r(&t, &_tmval))
        throw std::runtime_error("cannot get tm struct of timestamp!");
#endif

    _tmval.tm_isdst = -1;
}

} // namespace core