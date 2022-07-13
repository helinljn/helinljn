#include "base/ev_time.h"
#include "base/ev_common.h"
#include "event2/util.h"

#include <cstdio>
#include <cstring>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// time_utils
//////////////////////////////////////////////////////////////////////////
namespace time_utils {

int32 ev_gettimeofday(timeval* tv)
{
	return evutil_gettimeofday(tv, NULL);
}

int32 ev_gmtime(const time_t* timep, tm* result)
{
	if (NULL == timep || NULL == result)
	{
		return -1;
	}

#if defined(__EV_WINDOWS__)
	gmtime_s(result, timep);
#elif defined(__EV_LINUX__)
	gmtime_r(timep, result);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	return 0;
}

int32 ev_localtime(const time_t* timep, tm* result)
{
	if (NULL == timep || NULL == result)
	{
		return -1;
	}

#if defined(__EV_WINDOWS__)
	localtime_s(result, timep);
#elif defined(__EV_LINUX__)
	localtime_r(timep, result);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	return 0;
}

} // namespace time_utils

//////////////////////////////////////////////////////////////////////////
// ev_time
//////////////////////////////////////////////////////////////////////////
int32 ev_time::convert_date_to_number(const ev_time& time)
{
	timebuf buf = convert_date_to_timebuf(time, TS_YMD);
	return (('\0' == buf.buf[0] || 0 == buf.len) ? 0 : ev_strtoint32(buf.buf));
}

ev_time::timebuf ev_time::convert_date_to_timebuf(const ev_time& time, time_style style)
{
	timebuf buf;
	buf.buf[0] = '\0';
	buf.len    = 0;

	int32 year = time.year();
	int32 mon  = time.month();
	int32 mday = time.month_day();
	int32 wday = time.week_day();
	int32 hour = time.hour();
	int32 min  = time.minute();
	int32 sec  = time.second();

	switch (style)
	{
	case TS_YMDHMS_0:
		/* 2008-08-08 08:08:08 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%04d-%02d-%02d %02d:%02d:%02d", year, mon, mday, hour, min, sec);
		if (buf.len != 19)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_YMD_0:
		/* 2008-08-08 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%04d-%02d-%02d", year, mon, mday);
		if (buf.len != 10)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_HMS_0:
		/* 08:08:08 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%02d:%02d:%02d", hour, min, sec);
		if (buf.len != 8)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_YMDHMS:
		/* 20080808080808 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%04d%02d%02d%02d%02d%02d", year, mon, mday, hour, min, sec);
		if (buf.len != 14)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_YMD:
		/* 20080808 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%04d%02d%02d", year, mon, mday);
		if (buf.len != 8)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_HMS:
		/* 080808 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%02d%02d%02d", hour, min, sec);
		if (buf.len != 6)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	case TS_YMDWHMS:
		/* 2008080805080808 */
		buf.len = ev_snprintf(buf.buf, sizeof(buf.buf), "%04d%02d%02d%02d%02d%02d%02d", year, mon, mday, wday, hour, min, sec);
		if (buf.len != 16)
		{
			buf.buf[0] = '\0';
			buf.len    = 0;
		}

		return buf;
	default:
		return buf;
	}
}

std::string ev_time::convert_date_to_string(const ev_time& time, time_style style)
{
	timebuf buf = convert_date_to_timebuf(time, style);
	return (('\0' == buf.buf[0] || 0 == buf.len) ? std::string() : std::string(buf.buf, buf.len));
}

bool ev_time::update_time_cache(time_cache* tc)
{
	if (NULL == tc)
	{
		return false;
	}

	static const char numbers[]       = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	int64             cur_timestamp   = 0;
	int64             cur_microsecond = 0;
	{
		timeval curtime;
		time_utils::ev_gettimeofday(&curtime);

		cur_timestamp   = static_cast<int64>(curtime.tv_sec);
		cur_microsecond = static_cast<int64>(curtime.tv_usec);
	}

	// 每分钟更新一次"年-月-日 时:分:秒.微秒"的字符串缓存
	if (cur_timestamp / 60 != tc->cache_minutes)
	{
		int32 year    = 0;
		int32 month   = 0;
		int32 mday    = 0;
		int32 hour    = 0;
		int32 minute  = 0;
		int32 second  = 0;
		int32 msecond = static_cast<int32>(cur_microsecond);
		{
			time_t cur_sec = static_cast<time_t>(cur_timestamp);
			tm     result;

			time_utils::ev_localtime(&cur_sec, &result);

			year   = result.tm_year + 1900;
			month  = result.tm_mon  + 1;
			mday   = result.tm_mday;
			hour   = result.tm_hour;
			minute = result.tm_min;
			second = result.tm_sec;
		}

		int32 ret = ev_snprintf(tc->cache_format_time, sizeof(tc->cache_format_time), "%04d-%02d-%02d %02d:%02d:%02d.%06d",
			year, month, mday, hour, minute, second, msecond);
		if (ret != 26)
		{
			return false;
		}

		tc->cache_minutes     = cur_timestamp / 60;
		tc->cache_timestamp   = cur_timestamp;
		tc->cache_microsecond = cur_microsecond;

		return true;
	}

	// 每秒钟更新一次秒
	if (cur_timestamp != tc->cache_timestamp)
	{
		// 更新秒
		int32 seconds = static_cast<int32>(cur_timestamp % 60);

		tc->cache_format_time[17] = numbers[seconds / 10];
		tc->cache_format_time[18] = numbers[seconds % 10];

		tc->cache_timestamp = cur_timestamp;
	}

	// 每微秒更新一次微秒
	if (cur_microsecond != tc->cache_microsecond)
	{
		// 更新微秒
		int32 microseconds = static_cast<int32>(cur_microsecond);

		tc->cache_format_time[19] = '.';

		tc->cache_format_time[20] = numbers[microseconds / 100000];
		microseconds %= 100000;

		tc->cache_format_time[21] = numbers[microseconds / 10000];
		microseconds %= 10000;

		tc->cache_format_time[22] = numbers[microseconds / 1000];
		microseconds %= 1000;

		tc->cache_format_time[23] = numbers[microseconds / 100];
		microseconds %= 100;

		tc->cache_format_time[24] = numbers[microseconds / 10];
		microseconds %= 10;

		tc->cache_format_time[25] = numbers[microseconds];
		tc->cache_format_time[26] = '\0';

		tc->cache_microsecond = cur_microsecond;
	}

	return true;
}

ev_time::ev_time(void)
	: sec_(0)
	, usec_(0)
	, tm_()
{
	memset(this, 0, sizeof(ev_time));

	timeval curtime;
	time_utils::ev_gettimeofday(&curtime);

	sec_  = static_cast<time_t>(curtime.tv_sec);
	usec_ = static_cast<int32>(curtime.tv_usec);

	time_utils::ev_localtime(&sec_, &tm_);
}

ev_time::ev_time(const ev_time& t)
	: sec_(t.sec_)
	, usec_(t.usec_)
	, tm_(t.tm_)
{
}

ev_time::ev_time(time_t sec)
	: sec_(sec)
	, usec_(0)
	, tm_()
{
	time_utils::ev_localtime(&sec_, &tm_);
}

ev_time& ev_time::operator =(const ev_time& t)
{
	if (&t != this)
	{
		memcpy(this, &t, sizeof(ev_time));
	}
	
	return *this;
}

ev_time& ev_time::operator =(time_t sec)
{
	sec_  = sec;
	usec_ = 0;

	time_utils::ev_localtime(&sec_, &tm_);

	return *this;
}

} // namespace evpp