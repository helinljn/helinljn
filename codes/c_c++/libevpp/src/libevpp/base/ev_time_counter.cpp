#include "base/ev_time_counter.h"
#include "event2/util.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_time_counter
//////////////////////////////////////////////////////////////////////////
ev_time_counter::ev_time_counter(void)
	 : monotonic_timer_()
	 , start_time_()
	 , end_time_()
	 , microseconds_(0)
{
	evutil_configure_monotonic_time(reinterpret_cast<evutil_monotonic_timer*>(&monotonic_timer_), 0);
	reset();
}

void ev_time_counter::start(void)
{
	evutil_gettime_monotonic(reinterpret_cast<evutil_monotonic_timer*>(&monotonic_timer_), reinterpret_cast<timeval*>(&start_time_));
}

void ev_time_counter::end(void)
{
	evutil_gettime_monotonic(reinterpret_cast<evutil_monotonic_timer*>(&monotonic_timer_), reinterpret_cast<timeval*>(&end_time_));
	
	// 计算微秒时间差
	microseconds_ = calc_total_usec();
}

void ev_time_counter::reset(void)
{
	timerclear(reinterpret_cast<timeval*>(&start_time_));
	timerclear(reinterpret_cast<timeval*>(&end_time_));

	microseconds_ = 0;
}

uint64 ev_time_counter::calc_total_usec(void) const
{
	const timeval* st = reinterpret_cast<const timeval*>(&start_time_);
	const timeval* et = reinterpret_cast<const timeval*>(&end_time_);
	if ((0 == st->tv_sec && 0 == st->tv_usec) || (0 == et->tv_sec && 0 == et->tv_usec))
	{
		return 0;
	}

	int64 start_microseconds = st->tv_sec * static_cast<int64>(TC_MICROSECONDS_CONVERSION) + st->tv_usec;
	int64 end_microseconds   = et->tv_sec * static_cast<int64>(TC_MICROSECONDS_CONVERSION) + et->tv_usec;
	int64 result             = end_microseconds - start_microseconds;

	return static_cast<uint64>(result <= 0 ? 0 : result);
}

} // namespace evpp