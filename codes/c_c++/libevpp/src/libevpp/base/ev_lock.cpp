#include "base/ev_lock.h"
#include "base/ev_thread.h"

#if defined(__EV_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(__EV_LINUX__)
    #include <pthread.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

#include <cassert>

namespace evpp    {
namespace details {

#if defined(__EV_LINUX__)
//////////////////////////////////////////////////////////////////////////
// lazy_mutex_attr
// 因为我们实现的锁都是以递归锁为基础，所以每当一个mutex被创建的时候
// 都要指定其类型为PTHREAD_MUTEX_RECURSIVE，为了减少函数调用的次数
// 定义一个初始化类，并创建出一个全局对象，用于做该工作
//////////////////////////////////////////////////////////////////////////
class lazy_mutex_attr : public ev_noncopyable
{
public:
	lazy_mutex_attr(void)
	{
		int32 ret = pthread_mutexattr_init(&attr_recursive_);
		assert(0 == ret && "Libevpp fatal error! pthread_mutexattr_init() failed!");

		ret = pthread_mutexattr_settype(&attr_recursive_, PTHREAD_MUTEX_RECURSIVE);
		assert(0 == ret && "Libevpp fatal error! pthread_mutexattr_settype() failed!");

		EV_UNUSED_VARIABLE(ret);
	}

	~lazy_mutex_attr(void)
	{
		int32 ret = pthread_mutexattr_destroy(&attr_recursive_);
		assert(0 == ret && "Libevpp fatal error! pthread_mutexattr_destroy() failed!");
		EV_UNUSED_VARIABLE(ret);
	}

public:
	pthread_mutexattr_t attr_recursive_;
};
#endif // defined(__EV_LINUX__)

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_lock_base
//////////////////////////////////////////////////////////////////////////
ev_lock_base::ev_lock_base(void)
	: holder_thread_id_(0)
	, lock_count_(0)
{
}

ev_lock_base::~ev_lock_base(void)
{
}

bool ev_lock_base::is_locked_by_this_thread(void) const
{
	return (holder_thread_id_ == this_thread::thread_id());
}

void ev_lock_base::assign_holder_thread_id(void)
{
	++lock_count_;
	if (1 == lock_count_)
	{
		holder_thread_id_ = this_thread::thread_id();
	}
}

bool ev_lock_base::unassign_holder_thread_id(void)
{
	assert(lock_count_ >= 1 && holder_thread_id_ != 0 && is_locked_by_this_thread() && "Libevpp fatal error! unassign_holder_thread_id() failed!");
	if (lock_count_ < 1 || 0 == holder_thread_id_ || !is_locked_by_this_thread())
	{
		return false;
	}

	--lock_count_;
	if (0 == lock_count_)
	{
		holder_thread_id_ = 0;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// ev_recursive_lock
//////////////////////////////////////////////////////////////////////////
ev_recursive_lock::ev_recursive_lock(void)
	: ev_lock_base()
	, lock_()
{
#if defined(__EV_WINDOWS__)
	InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&lock_));
#elif defined(__EV_LINUX__)
	static details::lazy_mutex_attr mutex_recursive_attr;

	int32 ret = pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(&lock_), &mutex_recursive_attr.attr_recursive_);
	assert(0 == ret && "Libevpp fatal error! pthread_mutex_init() failed!");
	EV_UNUSED_VARIABLE(ret);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

ev_recursive_lock::~ev_recursive_lock(void)
{
	assert(0 == lock_count_ && 0 == holder_thread_id_ && "Libevpp fatal error! Unpaired lock() and unlock() call, please check you code!");

#if defined(__EV_WINDOWS__)
	DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&lock_));
#elif defined(__EV_LINUX__)
	int32 ret = pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(&lock_));
	assert(0 == ret && "Libevpp fatal error! pthread_mutex_destroy() failed!");
	EV_UNUSED_VARIABLE(ret);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

void ev_recursive_lock::lock(void)
{
#if defined(__EV_WINDOWS__)
	EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&lock_));
#elif defined(__EV_LINUX__)
	int32 ret = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(&lock_));
	assert(0 == ret && "Libevpp fatal error! pthread_mutex_lock() failed!");
	EV_UNUSED_VARIABLE(ret);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	assign_holder_thread_id();
}

void ev_recursive_lock::unlock(void)
{
	if (!unassign_holder_thread_id())
	{
		return;
	}

#if defined(__EV_WINDOWS__)
	LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&lock_));
#elif defined(__EV_LINUX__)
	int32 ret = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(&lock_));
	assert(0 == ret && "Libevpp fatal error! pthread_mutex_unlock() failed!");
	EV_UNUSED_VARIABLE(ret);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

//////////////////////////////////////////////////////////////////////////
// ev_lock
//////////////////////////////////////////////////////////////////////////
ev_lock::ev_lock(void)
	: ev_recursive_lock()
{
}

ev_lock::~ev_lock(void)
{
}

void ev_lock::lock(void)
{
#if defined(__EV_WINDOWS__)
	EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&lock_));
#elif defined(__EV_LINUX__)
	int32 ret = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(&lock_));
	assert(0 == ret && "Libevpp fatal error! pthread_mutex_lock() failed!");
	EV_UNUSED_VARIABLE(ret);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	if (is_locked_by_this_thread())
	{
		assert(false && "Libevpp fatal error! Thread dead lock, please check you code!");
		while (true)
		{
			// !!!Thread dead lock!!!
			this_thread::sleep_for(1);
		}
	}

	assign_holder_thread_id();
}

//////////////////////////////////////////////////////////////////////////
// ev_atomic_lock
//////////////////////////////////////////////////////////////////////////
ev_atomic_lock::ev_atomic_lock(void)
	: ev_lock_base()
	, atomic_lock_(0)
{
}

ev_atomic_lock::~ev_atomic_lock(void)
{
	assert(0 == lock_count_ && 0 == holder_thread_id_ && 0 == ev_atomic_get_value(&atomic_lock_) && "Libevpp fatal error! Unpaired lock() and unlock() call, please check you code!");
}

void ev_atomic_lock::lock(void)
{
	// atomic lock
	while (true)
	{
		if (ev_atomic_test_zero_and_increment(&atomic_lock_))
		{
			break;
		}
		else
		{
			this_thread::sleep_for(0);
		}
	}

	assign_holder_thread_id();

	assert(1 == lock_count_ && holder_thread_id_ != 0 && 1 == ev_atomic_get_value(&atomic_lock_) && "Libevpp fatal error!");
}

void ev_atomic_lock::unlock(void)
{
	assert(1 == lock_count_ && holder_thread_id_ != 0 && 1 == ev_atomic_get_value(&atomic_lock_) && "Libevpp fatal error!");

	if (!unassign_holder_thread_id())
	{
		return;
	}

	ev_atomic_decrement_and_fetch(&atomic_lock_);
}

} // namespace evpp