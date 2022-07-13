//////////////////////////////////////////////////////////////////////////
// 计时器(使用monotonic time实现，时间不会随着系统时间的改变而改变)
// 用于统计某个执行过程的时间
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_TIME_COUNTER_H__
#define __EV_TIME_COUNTER_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_time_counter
//////////////////////////////////////////////////////////////////////////
class ev_time_counter : public ev_noncopyable
{
public:
	// 时间单位转换
	enum time_convert
	{
		TC_MILLISECONDS_CONVERSION = 1000,
		TC_MICROSECONDS_CONVERSION = 1000000,
	};

public:
	ev_time_counter(void);

	/**
	 * @brief 开始计时，必须与end()成对调用
	 * @param
	 * @return
	 */
	void start(void);

	/**
	 * @brief 结束计时，必须与start()成对调用
	 * @param
	 * @return
	 */
	void end(void);

	/**
	 * @brief 重置计时器
	 * @param
	 * @return
	 */
	void reset(void);

	/**
	 * @brief 获取计时结束后，经过了多少秒
	 * @param
	 * @return
	 */
	uint64 seconds(void) const {return (microseconds_ / TC_MICROSECONDS_CONVERSION);}

	/**
	 * @brief 获取计时结束后，经过了多少毫秒
	 * @param
	 * @return
	 */
	uint64 milliseconds(void) const {return (microseconds_ / TC_MILLISECONDS_CONVERSION);}

	/**
	 * @brief 获取计时结束后，经过了多少微秒
	 * @param
	 * @return
	 */
	uint64 microseconds(void) const {return microseconds_;}

private:
	/**
	 * @brief 将计时器的结束时间和开始时间的时间差换算为微秒
	 * @param
	 * @return 微秒时间差
	 */
	uint64 calc_total_usec(void) const;

private:
	typedef struct { char buf[16];  } timeval_type;
	typedef struct { char buf[128]; } monotonic_timer;

private:
	monotonic_timer monotonic_timer_;
	timeval_type    start_time_;
	timeval_type    end_time_;
	uint64          microseconds_;
};

} // namespace evpp

#endif // __EV_TIME_COUNTER_H__