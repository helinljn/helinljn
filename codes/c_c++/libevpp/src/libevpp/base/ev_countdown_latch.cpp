#include "base/ev_countdown_latch.h"

#include <cassert>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_countdown_latch
//////////////////////////////////////////////////////////////////////////
ev_countdown_latch::ev_countdown_latch(int32 start_count)
	: lock_()
	, condition_(lock_)
	, count_(start_count)
{
	assert(start_count > 0 && "Libevpp fatal error! Invalid parameters!");
}

ev_countdown_latch::~ev_countdown_latch(void)
{
}

void ev_countdown_latch::wait(void)
{
	// 注意此处一定要使用while循环，防止虚假唤醒
	ev_lock_guard lock_guard(lock_);
	while (count_ > 0)
	{
		condition_.cond_wait();
	}
}

void ev_countdown_latch::countdown(void)
{
	ev_lock_guard lock_guard(lock_);
	--count_;
	if (0 == count_)
	{
		condition_.cond_broadcast();
	}
}

int32 ev_countdown_latch::get_count(void) const
{
	ev_lock_guard lock_guard(lock_);
	return count_;
}

bool ev_countdown_latch::reset_count(int32 new_count)
{
	assert(new_count > 0 && "Libevpp fatal error! Invalid parameters!");

	ev_lock_guard lock_guard(lock_);
	if (count_ > 0)
	{
		return false;
	}

	count_ = new_count;
	return true;
}

} // namespace evpp