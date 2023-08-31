#include "TimestampEx.h"
#include "DateTimeEx.h"
#include "Poco/Exception.h"

namespace Poco {

TimestampEx::TimestampEx(void)
    : _ts()
    , _tmStruct()
{
    updateTM();
}

TimestampEx::TimestampEx(const TimestampEx& timestamp)
    : _ts(timestamp._ts)
    , _tmStruct(timestamp._tmStruct)
{
}

TimestampEx::TimestampEx(const Timestamp& timestamp)
    : _ts(timestamp)
    , _tmStruct()
{
    updateTM();
}

TimestampEx::TimestampEx(const time_t epochTime)
    : _ts(0)
    , _tmStruct()
{
    assign(epochTime);
}

TimestampEx::TimestampEx(const tm& tmStruct)
    : TimestampEx(tmStruct.tm_year + 1900, tmStruct.tm_mon + 1, tmStruct.tm_mday, tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec)
{
}

TimestampEx::TimestampEx(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : _ts(0)
    , _tmStruct()
{
    assign(year, month, day, hour, minute, second, millisecond, microsecond);
}

TimestampEx& TimestampEx::assign(const TimestampEx& timestamp)
{
    if (this != &timestamp)
    {
        _ts       = timestamp._ts;
        _tmStruct = timestamp._tmStruct;
    }

    return *this;
}

TimestampEx& TimestampEx::assign(const Timestamp& timestamp)
{
    if (epochMicroseconds() != timestamp.epochMicroseconds())
    {
        _ts = timestamp;
        updateTM();
    }

    return *this;
}

TimestampEx& TimestampEx::assign(const time_t epochTime)
{
    if (this->epochTime() != epochTime)
    {
        _ts = Timestamp::fromEpochTime(epochTime);
        updateTM();
    }

    return *this;
}

TimestampEx& TimestampEx::assign(const tm& tmStruct)
{
    return assign(tmStruct.tm_year + 1900, tmStruct.tm_mon + 1, tmStruct.tm_mday, tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec);
}

TimestampEx& TimestampEx::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
    poco_assert(year >= 1970 && year <= 9999);
    poco_assert(month >= 1 && month <= 12);
    poco_assert(day >= 1 && day <= DateTimeEx::daysOfMonth(year, month));
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
        tm tmStruct{};
        tmStruct.tm_year  = year - 1900;
        tmStruct.tm_mon   = month - 1;
        tmStruct.tm_mday  = day;
        tmStruct.tm_hour  = hour;
        tmStruct.tm_min   = minute;
        tmStruct.tm_sec   = second;
        tmStruct.tm_isdst = -1;

        _ts  = Timestamp::fromEpochTime(mktime(&tmStruct));
        _ts += Timespan::TimeDiff(millisecond) * Timespan::MILLISECONDS;
        _ts += microsecond;

        updateTM();
    }

    return *this;
}

void TimestampEx::update(void)
{
    _ts.update();
    updateTM();
}

void TimestampEx::updateTM(void)
{
    const time_t t = _ts.epochTime();
    poco_assert(t >= 0);

#if defined(POCO_OS_FAMILY_WINDOWS)
    if (localtime_s(&_tmStruct, &t) != 0)
        throw SystemException("cannot get tm struct of timestamp");
#else
    if (!localtime_r(&t, &_tmStruct))
        throw SystemException("cannot get tm struct of timestamp");
#endif
}

} // namespace Poco