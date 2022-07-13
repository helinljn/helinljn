#include "base/ev_once.h"
#include "base/ev_thread.h"

#include <cassert>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_once
//////////////////////////////////////////////////////////////////////////
ev_once::ev_once(void)
	: ran_(0)
	, once_func_called_(0)
{
}

void ev_once::once_init(once_function_type func, void* arg)
{
	// 多线程竞争，有且只有其中一个线程可以竞争成功
	if (ev_atomic_test_zero_and_increment(&ran_))
	{// 竞争成功
		func(arg);

		int32 result = ev_atomic_fetch_and_increment(&once_func_called_);
		assert(0 == result && "Libevpp fatal error! Invalid result!");
		EV_UNUSED_VARIABLE(result);
	}
	else
	{// 竞争失败，等待初始化调用结束
		while (true)
		{
			if (1 == ev_atomic_get_value(&once_func_called_))
			{
				break;
			}
			else
			{
				this_thread::sleep_for(1);
			}
		}
	}
}

} // namespace evpp