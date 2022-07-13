//////////////////////////////////////////////////////////////////////////
// 重入锁，不可重入锁，原子锁
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_LOCK_H__
#define __EV_LOCK_H__

#include "base/ev_types.h"
#include "base/ev_atomic.h"
#include "base/ev_noncopyable.h"

namespace evpp {

class ev_condition;

//////////////////////////////////////////////////////////////////////////
// ev_lock_base
// 基础锁接口
//////////////////////////////////////////////////////////////////////////
class ev_lock_base : public ev_noncopyable
{
public:
	ev_lock_base(void);
	virtual ~ev_lock_base(void);

	/**
	 * @brief 加锁
	 * @param
	 * @return
	 */
	virtual void lock(void) = 0;

	/**
	 * @brief 解锁
	 * @param
	 * @return
	 */
	virtual void unlock(void) = 0;

	/**
	 * @brief 判断当前锁是否被调用线程加锁
	 * @param
	 * @return 当前锁没有被加锁或者没有被调用线程加锁则返回false，否则返回true
	 */
	bool is_locked_by_this_thread(void) const;

protected:
	/**
	 * @brief 分配持有该锁的线程ID
	 * @param
	 * @return
	 */
	void assign_holder_thread_id(void);

	/**
	 * @brief 解除持有该锁的线程ID
	 * @param
	 * @return 成功返回true，失败返回false(原因为lock()与unlock()调用次数不匹配)
	 */
	bool unassign_holder_thread_id(void);

protected:
	uint32 holder_thread_id_;
	int32  lock_count_;
};

//////////////////////////////////////////////////////////////////////////
// ev_recursive_lock
// 实现为递归锁，即可重入锁
//////////////////////////////////////////////////////////////////////////
class ev_recursive_lock : public ev_lock_base
{
public:
	ev_recursive_lock(void);
	virtual ~ev_recursive_lock(void);

	/**
	 * @brief 加锁
	 * @param
	 * @return
	 */
	virtual void lock(void);

	/**
	 * @brief 解锁
	 * @param
	 * @return
	 */
	virtual void unlock(void);

protected:
	class unassign_guard : public ev_noncopyable
	{
	public:
		unassign_guard(ev_recursive_lock& owner)
			: owner_(owner)
		{
			owner_.unassign_holder_thread_id();
		}

		~unassign_guard(void)
		{
			owner_.assign_holder_thread_id();
		}

	private:
		ev_recursive_lock& owner_;
	};

protected:
	typedef struct { char buf[256]; } lock_type;
	friend  class                     ev_condition;

protected:
	lock_type lock_;
};

//////////////////////////////////////////////////////////////////////////
// ev_lock
// 实现为非递归锁，即不可重入锁
//////////////////////////////////////////////////////////////////////////
class ev_lock : public ev_recursive_lock
{
public:
	ev_lock(void);
	virtual ~ev_lock(void);

	/**
	 * @brief 加锁
	 * @param
	 * @return
	 */
	virtual void lock(void);
};

//////////////////////////////////////////////////////////////////////////
// ev_atomic_lock
// 原子锁实现(不可重入，多次加锁会导致死锁)
// 当前版本实现为忙等待，即多线程大量抢占资源时会造成高CPU占用率
//////////////////////////////////////////////////////////////////////////
class ev_atomic_lock : public ev_lock_base
{
public:
	ev_atomic_lock(void);
	virtual ~ev_atomic_lock(void);

	/**
	 * @brief 加锁
	 * @param
	 * @return
	 */
	virtual void lock(void);

	/**
	 * @brief 解锁
	 * @param
	 * @return
	 */
	virtual void unlock(void);

private:
	ev_atomic_int32 atomic_lock_;
};

//////////////////////////////////////////////////////////////////////////
// ev_lock_guard
//////////////////////////////////////////////////////////////////////////
class ev_lock_guard : public ev_noncopyable
{
public:
	ev_lock_guard(ev_lock_base& lock)
		: lock_(lock)
	{
		lock_.lock();
	}

	ev_lock_guard(ev_lock_base* lock)
		: lock_(*lock)
	{
		lock_.lock();
	}

	~ev_lock_guard(void)
	{
		lock_.unlock();
	}

private:
	ev_lock_base& lock_;
};

} // namespace evpp

// 该宏是为了防止如下方式的误用，构造一个临时对象，并不能长时间的持有锁:
// ev_lock_guard(lock);
#define ev_lock_guard(x) (error "Missing guard object name!")

#endif // __EV_LOCK_H__