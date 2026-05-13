#include "datetime.h"
#include <ctime>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace core {

datetime::datetime()
    : ts_()
    , tmval_()
{
    update_tm();
}

datetime::datetime(const timestamp& ts)
    : ts_(ts)
    , tmval_()
{
    update_tm();
}

datetime::datetime(const tm& tmval)
    : datetime(tmval.tm_year + 1900, tmval.tm_mon + 1, tmval.tm_mday, tmval.tm_hour, tmval.tm_min, tmval.tm_sec)
{
}

datetime::datetime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : ts_(0)
    , tmval_()
{
    assign(year, month, day, hour, minute, second, millisecond, microsecond);
}

datetime& datetime::assign(const timestamp& ts)
{
    if (ts_ != ts)
    {
        ts_ = ts;
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

        ts_  = mktime(&tmval) * timestamp::resolution;
        ts_ += time_t{millisecond} * 1000;
        ts_ += microsecond;

        update_tm();
    }

    return *this;
}

void datetime::update()
{
    ts_.update();
    update_tm();
}

std::string datetime::format(std::string_view fmt) const
{
    if (fmt.empty())
        return std::string{};

    std::string processed_fmt(fmt);

    char ms_str[12], us_str[12];
    std::snprintf(ms_str, sizeof(ms_str), "%03d", millisecond());
    std::snprintf(us_str, sizeof(us_str), "%03d", microsecond());

    const auto ms_len = std::strlen(ms_str);
    const auto us_len = std::strlen(us_str);

    for (std::string::size_type pos = 0; (pos = processed_fmt.find("{ms}", pos)) != std::string::npos; pos += ms_len)
        processed_fmt.replace(pos, 4, ms_str);

    for (std::string::size_type pos = 0; (pos = processed_fmt.find("{us}", pos)) != std::string::npos; pos += us_len)
        processed_fmt.replace(pos, 4, us_str);

    char buf[1024];
    if (std::strftime(buf, sizeof(buf), processed_fmt.c_str(), &tmval_) == 0)
        return std::string{};

    return std::string(buf);
}

void datetime::update_tm()
{
    const auto t = ts_.epoch_time();
    if (t < 0)
        throw std::runtime_error("cannot get epoch time!");

#if defined(CORE_PLATFORM_WINDOWS)
    if (localtime_s(&tmval_, &t) != 0)
        throw std::runtime_error("cannot get tm struct of timestamp!");
#elif defined(CORE_PLATFORM_LINUX)
    if (!localtime_r(&t, &tmval_))
        throw std::runtime_error("cannot get tm struct of timestamp!");
#endif

    tmval_.tm_isdst = -1;
}

} // namespace core
