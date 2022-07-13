//////////////////////////////////////////////////////////////////////////
// 线程
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_THREAD_H__
#define __EV_THREAD_H__

#include "base/ev_types.h"
#include "base/ev_atomic.h"
#include "base/ev_noncopyable.h"

#include <string>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_thread
//////////////////////////////////////////////////////////////////////////
class ev_thread : public ev_noncopyable
{
public:
	// 线程入口函数的类型
	typedef void (*ev_thread_callback)(void*);

public:
	/**
	 * @brief 获取当前正在运行的线程数量
	 * @param
	 * @return
	 */
	static int32 get_running_thread_count(void) {return ev_atomic_get_value(&running_thread_count_);}

public:
	/**
	 * @brief 平淡无奇的构造函数
	 *        只负责构造线程对象，并不负责创建线程
	 * @param
	 * @return
	 */
	ev_thread(void);

	/**
	 * @brief 略微特殊的析构函数
	 *        在调用start_thread()函数成功返回后，如果并没有调用join_thread()去等待线程结束
	 *        那么在线程对象析构的时候，会自动的调用detach_thread()将该线程设置为分离线程
	 *        如果显式调用过join_thread()或者detach_thread()，那么析构函数将什么也不做
	 * @param
	 * @return
	 */
	~ev_thread(void);

	/**
	 * @brief 创建线程
	 *        成功调用该函数后，就会创建一个线程运行用户提供的线程入口函数
	 *        默认创建的线程为【joinable的非分离线程】
	 * @param func          线程运行的入口函数
	 * @param arg           回传参数
	 * @param thread_name   线程名字，默认为：worker thread
	 * @param stack_size_kb 线程堆栈大小，0代表默认，以KB为单位
	 * @return 线程创建成功返回true，失败返回false
	 */
	bool start_thread(ev_thread_callback func, void* arg, const std::string& name = "worker thread", uint32 stack_size_kb = 0);

	/**
	 * @brief 分离线程
	 * @param
	 * @return 成功返回true，失败返回false
	 */
	bool detach_thread(void);

	/**
	 * @brief 等待线程运行结束
	 * @param
	 * @return 成功返回true，失败返回false
	 */
	bool join_thread(void);

private:
	/**
	 * @brief 子线程的执行入口函数
	 * @param
	 * @return
	 */
	static void*
#if defined(__EV_WINDOWS__)
		__stdcall
#endif // defined(__EV_WINDOWS__)
		thread_entry(void* arg);

	/**
	 * @brief 获取线程是否可以被join
	 *        注意：线程可以被join需要满足以下条件
	 *            1. 已成功调用start_thread()函数，并且没有调用detach_thread()函数
	 *            2. 当前为第一次调用join_thread()函数，也就是说，最多只允许被join一次
	 *            3. 线程不能自己join自己，必须由别的线程调用join_thread()
	 * @param
	 * @return 线程可以被join返回true，否则返回false
	 */
	bool joinable(void) const;

private:
	ev_thread_callback     thread_entry_;
	void*                  thread_arg_;
	std::string            thread_name_;
	uint64                 thread_identifying_;
	void*                  thread_handle_;
	static ev_atomic_int32 running_thread_count_;
};

namespace this_thread {

// 主线程默认名字
const char* const g_threadconfig_main_thread_name = "main thread";

/**
 * @brief 获取当前调用线程的线程ID
 * @param
 * @return
 */
uint32 thread_id(void);

/**
 * @brief 获取当前调用线程的线程名字
 * @param
 * @return
 */
const char* thread_name(void);

/**
 * @brief 睡眠
 * @param milliseconds 毫秒
 * @return
 */
void sleep_for(uint32 milliseconds);

} // namespace this_thread
} // namespace evpp

#endif // __EV_THREAD_H__