#include "base/ev_condition.h"
#include "base/ev_lock.h"
#include "event2/util.h"

#if defined(__EV_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(__EV_LINUX__)
    #include <pthread.h>
#else
    #error "Unrecognized os platform!"
#endif // defined(__EV_WINDOWS__)

#include <cerrno>
#include <cassert>

namespace evpp    {
namespace details {

//////////////////////////////////////////////////////////////////////////
// 条件变量跨平台实现
//////////////////////////////////////////////////////////////////////////

/**
 * @brief 初始化条件变量
 * @param cond 条件变量
 * @return 成功返回0，失败返回-1
 */
static inline int32 ev_cond_init_internal(void* cond)
{
#if defined(__EV_WINDOWS__)
	InitializeConditionVariable(reinterpret_cast<PCONDITION_VARIABLE>(cond));
	return 0;
#elif defined(__EV_LINUX__)
	int32 result = pthread_cond_init(reinterpret_cast<pthread_cond_t*>(cond), NULL);
	return (0 == result ? result : -1);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

/**
 * @brief 销毁条件变量
 * @param cond 条件变量
 * @return 成功返回0，失败返回-1
 */
static inline int32 ev_cond_destroy_internal(void* cond)
{
#if defined(__EV_WINDOWS__)
	// do nothing
	return 0;
#elif defined(__EV_LINUX__)
	int32 result = pthread_cond_destroy(reinterpret_cast<pthread_cond_t*>(cond));
	return (0 == result ? result : -1);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

/**
 * @brief 等待条件变量成立
 * @param cond 条件变量 
 * @param lock 条件变量配合使用的锁
 * @param tv   等待的时间，如果为NULL代表无限等待
 * @return 成功返回0，超时返回1，失败返回-1
 */
static inline int32 ev_cond_wait_internal(void* cond, void* lock, const struct timeval *tv)
{
#if defined(__EV_WINDOWS__)
	BOOL  result = FALSE;
	DWORD ms     = INFINITE;

	if (tv)
	{
		ms = ((tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000));
	}

	result = SleepConditionVariableCS(reinterpret_cast<PCONDITION_VARIABLE>(cond), reinterpret_cast<PCRITICAL_SECTION>(lock), ms);
	if (result)
	{
		return 0;
	}
	else
	{
		if (ERROR_TIMEOUT == GetLastError())
			return 1;
		else
			return -1;
	}
#elif defined(__EV_LINUX__)
	int r;

	if (tv)
	{
		struct timeval now, abstime;
		evutil_gettimeofday(&now, NULL);
		timeradd(&now, tv, &abstime);

		struct timespec ts;
		ts.tv_sec  = abstime.tv_sec;
		ts.tv_nsec = abstime.tv_usec*1000;

		r = pthread_cond_timedwait(reinterpret_cast<pthread_cond_t*>(cond), reinterpret_cast<pthread_mutex_t*>(lock), &ts);
		if (r == ETIMEDOUT)
			return 1;
		else if (r)
			return -1;
		else
			return 0;
	}
	else
	{
		r = pthread_cond_wait(reinterpret_cast<pthread_cond_t*>(cond), reinterpret_cast<pthread_mutex_t*>(lock));
		return r ? -1 : 0;
	}
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

/**
 * @brief 唤醒等待在此条件变量上的线程
 * @param cond      条件变量 
 * @param broadcast 当!=0时，代表broadcast
 * @return 成功返回0，失败返回-1
 */
static inline int32 ev_cond_signal_internal(void* cond, int broadcast)
{
#if defined(__EV_WINDOWS__)
	if (broadcast)
		WakeAllConditionVariable(reinterpret_cast<PCONDITION_VARIABLE>(cond));
	else
		WakeConditionVariable(reinterpret_cast<PCONDITION_VARIABLE>(cond));

	return 0;
#elif defined(__EV_LINUX__)
	int r;

	if (broadcast)
		r = pthread_cond_broadcast(reinterpret_cast<pthread_cond_t*>(cond));
	else
		r = pthread_cond_signal(reinterpret_cast<pthread_cond_t*>(cond));

	return (r ? -1 : 0);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_condition
//////////////////////////////////////////////////////////////////////////
ev_condition::ev_condition(ev_recursive_lock& lock_base)
	: condition_()
	, lock_base_(lock_base)
{
	int32 ret = details::ev_cond_init_internal(&condition_);
	assert(ret != -1 && "Libevpp fatal error! details::ev_cond_init_internal() failed!");
	EV_UNUSED_VARIABLE(ret);
}

ev_condition::~ev_condition(void)
{
	int32 ret = details::ev_cond_destroy_internal(&condition_);
	assert(ret != -1 && "Libevpp fatal error! details::ev_cond_destroy_internal() failed!");
	EV_UNUSED_VARIABLE(ret);
}

void ev_condition::cond_wait(void)
{
	ev_lock::unassign_guard ug(lock_base_);

	int32 ret = details::ev_cond_wait_internal(&condition_, &lock_base_.lock_, NULL);
	assert(ret != -1 && "Libevpp fatal error! details::ev_cond_wait_internal() failed!");
	EV_UNUSED_VARIABLE(ret);
}

void ev_condition::cond_timedwait(int32 milliseconds_timeout)
{
	if (milliseconds_timeout < 0)
	{
		ev_lock::unassign_guard ug(lock_base_);

		int32 ret = details::ev_cond_wait_internal(&condition_, &lock_base_.lock_, NULL);
		assert(ret != -1 && "Libevpp fatal error! details::ev_cond_wait_internal() failed!");
		EV_UNUSED_VARIABLE(ret);
	}
	else
	{
		timeval timeout;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 0;

		timeout.tv_sec  = milliseconds_timeout / 1000;
		timeout.tv_usec = (milliseconds_timeout % 1000) * 1000;

		ev_lock::unassign_guard ug(lock_base_);

		int32 ret = details::ev_cond_wait_internal(&condition_, &lock_base_.lock_, &timeout);
		assert(ret != -1 && "Libevpp fatal error! details::ev_cond_wait_internal() failed!");
		EV_UNUSED_VARIABLE(ret);
	}
}

void ev_condition::cond_signal(void)
{
	int32 ret = details::ev_cond_signal_internal(&condition_, 0);
	assert(ret != -1 && "Libevpp fatal error! details::ev_cond_signal_internal(0) failed!");
	EV_UNUSED_VARIABLE(ret);
}

void ev_condition::cond_broadcast(void)
{
	int32 ret = details::ev_cond_signal_internal(&condition_, 1);
	assert(ret != -1 && "Libevpp fatal error! details::ev_cond_signal_internal(1) failed!");
	EV_UNUSED_VARIABLE(ret);
}

} // namespace evpp