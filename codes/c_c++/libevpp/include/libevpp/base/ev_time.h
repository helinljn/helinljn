//////////////////////////////////////////////////////////////////////////
// 时间相关操作
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_TIME_H__
#define __EV_TIME_H__

#include "base/ev_types.h"

#include <ctime>
#include <string>

#if defined(__EV_WINDOWS__)
    #include <WinSock2.h>
#elif defined(__EV_LINUX__)
    #include <sys/time.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// time_utils
//////////////////////////////////////////////////////////////////////////
namespace time_utils {

/**
 * @brief 获取精准时间点(当地时区)
 * @param tv 存储时间点的结构体
 * @return 成功返回0，失败返回-1
 */
int32 ev_gettimeofday(timeval* tv);

/**
 * @brief gmtime线程安全封装
 * @param timep  时间戳
 * @param result 存储转换的结果
 * @return 成功返回0，失败返回-1
 */
int32 ev_gmtime(const time_t* timep, tm* result);

/**
 * @brief localtime线程安全封装
 * @param timep  时间戳
 * @param result 存储转换的结果
 * @return 成功返回0，失败返回-1
 */
int32 ev_localtime(const time_t* timep, tm* result);

} // namespace time_utils

//////////////////////////////////////////////////////////////////////////
// ev_time
//////////////////////////////////////////////////////////////////////////
class ev_time
{
public:
	// 时间转换的样式
	enum time_style
	{
		TS_YMDHMS_0,  /* 2008-08-08 08:08:08 */
		TS_YMD_0,     /* 2008-08-08 */
		TS_HMS_0,     /* 08:08:08 */
		TS_YMDHMS,    /* 20080808080808 */
		TS_YMD,       /* 20080808 */
		TS_HMS,       /* 080808 */
		TS_YMDWHMS,   /* 2008080805080808 */
	};

	// 时间字符串buf
	struct timebuf 
	{
		char  buf[32];
		int32 len;
	};

	// 时间缓存
	struct time_cache
	{
	public:
		time_cache(void)
			: cache_microsecond(0)
			, cache_timestamp(0)
			, cache_minutes(0)
		{
		}

	public:
		int64 cache_microsecond;      // 微秒缓存
		int64 cache_timestamp;        // 时间戳缓存
		int64 cache_minutes;          // 分钟缓存
		char  cache_format_time[32];  // "年-月-日 时:分:秒.微秒"的字符串缓存
		                              // 格式："%04d-%02d-%02d %02d:%02d:%02d.%06d"
		                              // 长度：固定的26字节
	};

public:
	/**
	 * @brief 将年-月-日转换为如：20080808 这样格式的数字
	 * @param time ev_time时间类
	 * @return 成功则返回日期的对应数字，失败返回0
	 */
	static int32 convert_date_to_number(const ev_time& time);

	/**
	 * @brief 将日期按照time_style中的格式来转换为相对应的字符串
	 * @param time  ev_time时间类
	 * @param style 转换的样式，默认为time_style::TS_YMDHMS_0
	 * @return 成功则返回转换后的结果，失败返回空字符串
	 */
	static timebuf     convert_date_to_timebuf(const ev_time& time, time_style style = TS_YMDHMS_0);
	static std::string convert_date_to_string(const ev_time& time, time_style style = TS_YMDHMS_0);

	/**
	 * @brief 更新时间字符串缓存
	 *        写入格式：
	 *            "年-月-日 时:分:秒.微秒" -> "%04d-%02d-%02d %02d:%02d:%02d.%06d"
	 *        当每秒钟需要上百万次格式化时间字符串的时候，采用常规写法，先获取当前时间ev_time cur_time;
	 *        然后再使用ev_snprintf()写入到缓冲区的办法效率太低，因此才有此优化版本。
	 *        使用该优化函数，将会大大的缩小因为格式化时间字符串而导致的性能开销
	 *        当前应用场景：记录日志时，日志头的时间字符串
	 * @param tc 时间缓存结构体
	 * @return 成功返回true，失败返回false
	 */
	static bool update_time_cache(time_cache* tc);

	/**
	 * @brief 判断是否为闰年
	 * @param year 当前的年份，比如：2008，请自行保证参数的有效性
	 * @return 是闰年则返回true，否则返回false
	 */
	static bool is_leap_year(int32 year)
	{
		return ((0 == year % 100 && 0 == year % 400) || (0 != year % 100 && 0 == year % 4));
	}

public:
	ev_time(void);

	ev_time(const ev_time& t);
	ev_time(time_t sec);

	ev_time& operator =(const ev_time& t);
	ev_time& operator =(time_t sec);

	/**
	 * @brief 获取从1970-01-01 00:00:00以来所有的秒数(按照当地时区)
	 * @param
	 * @return
	 */
	time_t timestamp(void) const {return sec_;}

	/**
	 * @brief 获取微秒数，取值范围[0, 999999]
	 * @param
	 * @return
	 */
	int32 microsecond(void) const {return usec_;}

	/**
	 * @brief 获取毫秒数，取值范围[0, 999]
	 * @param
	 * @return
	 */
	int32 millisecond(void) const {return (usec_ / 1000);}

	/**
	 * @brief 获取秒数，取值范围[0, 59]
	 * @param
	 * @return
	 */
	int32 second(void) const {return tm_.tm_sec;}

	/**
	 * @brief 获取分钟数，取值范围[0, 59]
	 * @param
	 * @return
	 */
	int32 minute(void) const {return tm_.tm_min;}

	/**
	 * @brief 获取小时数，取值范围[0, 23]
	 * @param
	 * @return
	 */
	int32 hour(void) const {return tm_.tm_hour;}

	/**
	 * @brief 获取当前是星期几，取值范围[0, 6]，0代表星期天，1代表星期一，以此类推
	 * @param
	 * @return 
	 */
	int32 week_day(void) const {return tm_.tm_wday;}

	/**
	 * @brief 获取当前是一个月中的第几天，取值范围[1, 31]
	 * @param
	 * @return
	 */
	int32 month_day(void) const {return tm_.tm_mday;}

	/**
	 * @brief 获取当前是一年中的第几天，取值范围[1, 366]，包括闰年
	 * @param
	 * @return
	 */
	int32 year_day(void) const {return tm_.tm_yday + 1;}

	/**
	 * @brief 获取当前的月份，取值范围[1, 12]
	 * @param
	 * @return
	 */
	int32 month(void) const {return tm_.tm_mon + 1;}

	/**
	 * @brief 获取当前的年份，取值范围[1900, 至今]
	 * @param
	 * @return
	 */
	int32 year(void) const {return tm_.tm_year + 1900;}

public:
	friend inline bool operator ==(const ev_time& t1, const ev_time& t2)
	{
		return (t1.sec_ == t2.sec_ && t1.usec_ == t2.usec_);
	}

	friend inline bool operator !=(const ev_time& t1, const ev_time& t2)
	{
		return !(operator ==(t1, t2));
	}

	friend inline bool operator <(const ev_time& t1, const ev_time& t2)
	{
		return (t1.sec_ < t2.sec_ || (t1.sec_ == t2.sec_ && t1.usec_ < t2.usec_));
	}

	friend inline bool operator >(const ev_time& t1, const ev_time& t2)
	{
		return (t1.sec_ > t2.sec_ || (t1.sec_ == t2.sec_ && t1.usec_ > t2.usec_));
	}

	friend inline bool operator <=(const ev_time& t1, const ev_time& t2)
	{
		return (operator <(t1, t2) || operator ==(t1, t2));
	}

	friend inline bool operator >=(const ev_time& t1, const ev_time& t2)
	{
		return (operator >(t1, t2) || operator ==(t1, t2));
	}

private:
	time_t sec_;
	int32  usec_;
	tm     tm_;
};

} // namespace evpp

#endif // __EV_TIME_H__