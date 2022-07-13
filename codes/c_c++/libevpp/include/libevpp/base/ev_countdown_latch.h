//////////////////////////////////////////////////////////////////////////
// 倒计数锁
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_COUNTDOWN_LATCH_H__
#define __EV_COUNTDOWN_LATCH_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_lock.h"
#include "base/ev_condition.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_countdown_latch
//////////////////////////////////////////////////////////////////////////
class ev_countdown_latch : public ev_noncopyable
{
public:
	ev_countdown_latch(int32 start_count);
	~ev_countdown_latch(void);

	/**
	 * @brief 等待计数到达0，在计数没有到达0之前，调用线程阻塞
	 * @param
	 * @return
	 */
	void wait(void);

	/**
	 * @brief 计数减一，如果减小之后计数为0，那么唤醒所有等待在该计数上的线程
	 * @param
	 * @return
	 */
	void countdown(void);

	/**
	 * @brief 获取当前计数
	 * @param
	 * @return
	 */
	int32 get_count(void) const;

	/**
	 * @brief 重置当前计数，以便重复使用
	 *        当且仅当(new_count > 0 && count_ <=0)时，该调用才能成功，因为要保证没有线程等待在该倒计数锁上
	 * @param new_count 需要重置的新的计数
	 * @return 成功返回true，失败返回false
	 */
	bool reset_count(int32 new_count);

private:
	mutable ev_lock lock_;
	ev_condition    condition_;
	int32           count_;
};

} // namespace evpp

#endif // __EV_COUNTDOWN_LATCH_H__