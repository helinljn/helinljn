//////////////////////////////////////////////////////////////////////////
// ev_once
// 使用CAS原子操作实现POSIX的pthread_once
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_ONCE_H__
#define __EV_ONCE_H__

#include "base/ev_types.h"
#include "base/ev_atomic.h"
#include "base/ev_noncopyable.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_once
//////////////////////////////////////////////////////////////////////////
class ev_once : public ev_noncopyable
{
public:
	// 初始化函数的类型
	typedef void (*once_function_type)(void* arg);

public:
	ev_once(void);

	/**
	 * @brief 调用初始化函数
	 *        多线程同时调用该函数，有且仅有一个可以成功执行
	 *        其它线程都会等待调用成功的线程初始化完成
	 * @param func 初始化函数
	 * @param arg  函数参数
	 * @return
	 */
	void once_init(once_function_type func, void* arg);

private:
	ev_atomic_int32 ran_;
	ev_atomic_int32 once_func_called_;
};

} // namespace evpp

#endif // __EV_ONCE_H__