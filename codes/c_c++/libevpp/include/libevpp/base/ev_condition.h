//////////////////////////////////////////////////////////////////////////
// 条件变量
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_CONDITION_H__
#define __EV_CONDITION_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"

namespace evpp {

class ev_recursive_lock;

//////////////////////////////////////////////////////////////////////////
// ev_condition
//////////////////////////////////////////////////////////////////////////
class ev_condition : public ev_noncopyable
{
public:
	ev_condition(ev_recursive_lock& lock_base);
	~ev_condition(void);

	/**
	 * @brief 等待条件变量的条件成立，一直等待
	 * @param
	 * @return
	 */
	void cond_wait(void);

	/**
	 * @brief 等待条件变量的条件成立，超时等待
	 * @param milliseconds_timeout 一个相对的等待时间，比如等待5000毫秒(5秒)
	 *                             如果该参数为-1，那么与调用cond_wait()意义相同
	 * @return
	 */
	void cond_timedwait(int32 milliseconds_timeout);

	/**
	 * @brief 唤醒一个等待在此条件变量上的线程
	 * @param
	 * @return
	 */
	void cond_signal(void);

	/**
	 * @brief 唤醒所有等待在此条件变量上的线程
	 * @param
	 * @return
	 */
	void cond_broadcast(void);

private:
	typedef struct { char buf[256]; } condition_type;

private:
	condition_type     condition_;
	ev_recursive_lock& lock_base_;
};

} // namespace evpp

#endif // __EV_CONDITION_H__