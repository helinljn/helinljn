#include "base/ev_thread.h"

#if defined(__EV_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <process.h>
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(__EV_LINUX__)
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/syscall.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

#include <cstring>

namespace evpp        {
namespace this_thread {

static ev_thread_local uint32 g_thread_id       = 0;
static ev_thread_local char   g_thread_name[64] = {0};

} // namespace this_thread

namespace details {

/**
 * @brief 获取当前调用线程的线程ID
 * @param
 * @return
 */
static inline uint32 ev_get_thread_id_internal(void)
{
#if defined(__EV_WINDOWS__)
	return static_cast<uint32>(GetCurrentThreadId());
#elif defined(__EV_LINUX__)
	return static_cast<uint32>(syscall(__NR_gettid));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_thread
//////////////////////////////////////////////////////////////////////////
ev_atomic_int32 ev_thread::running_thread_count_ = 0;

ev_thread::ev_thread(void)
	: thread_entry_(NULL)
	, thread_arg_(NULL)
	, thread_name_()
	, thread_identifying_(0)
	, thread_handle_(NULL)
{
}

ev_thread::~ev_thread(void)
{
	detach_thread();
}

bool ev_thread::start_thread(ev_thread_callback func, void* arg, const std::string& name, uint32 stack_size_kb)
{
	if (NULL == func || name.empty() || thread_identifying_ != 0 || thread_handle_ != NULL)
	{
		return false;
	}

	thread_entry_ = func;
	thread_arg_   = arg;
	thread_name_  = name;

#if defined(__EV_WINDOWS__)
	unsigned int temp_thread_identifying = 0;
	void*        temp_handle             = NULL;
	temp_handle = (void*)_beginthreadex(
		NULL,                                        // LPSECURITY_ATTRIBUTES ThreadAttributes,
		stack_size_kb,                               // DWORD StackSize,
		(unsigned (__stdcall*)(void*))&thread_entry, // StartAddress,
		this,                                        // LPVOID Parameter,
		0,                                           // CreationFlags,
		&temp_thread_identifying                     // ThreadId
	);

	// 线程创建失败
	if (NULL == temp_handle)
	{
		return false;
	}

	thread_identifying_ = temp_thread_identifying;
	thread_handle_      = temp_handle;

	return true;
#elif defined(__EV_LINUX__)
	pthread_attr_t attr;
	if (pthread_attr_init(&attr) != 0)
	{
		return false;
	}

	// 如果需要设置线程堆栈大小，那么就设置其相关的属性
	if (stack_size_kb)
	{
		// Linux下是以字节为单位
		stack_size_kb *= 1024;
		if (pthread_attr_setstacksize(&attr, stack_size_kb) != 0)
		{
			// 设置属性失败，直接返回
			pthread_attr_destroy(&attr);
			return false;
		}
	}

	// 创建线程
	pthread_t temp_thread_identifying;
	if (pthread_create(&temp_thread_identifying, &attr, &thread_entry, this) != 0)
	{
		// 线程创建失败，直接返回
		pthread_attr_destroy(&attr);
		return false;
	}

	// 创建成功，清理属性，并进行后续操作
	pthread_attr_destroy(&attr);
	
	thread_identifying_ = (uint64)temp_thread_identifying;
	thread_handle_      = (void*)temp_thread_identifying;

	return true;
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_thread::detach_thread(void)
{
	if (0 == thread_identifying_ || NULL == thread_handle_)
	{
		return false;
	}

#if defined(__EV_WINDOWS__)
	CloseHandle(thread_handle_);
#elif defined(__EV_LINUX__)
	pthread_detach((pthread_t)thread_handle_);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	thread_identifying_ = 0;
	thread_handle_      = NULL;

	return true;
}

bool ev_thread::join_thread(void)
{
	if (!joinable())
	{
		return false;
	}

#if defined(__EV_WINDOWS__)
	WaitForSingleObject(thread_handle_, INFINITE);
	CloseHandle(thread_handle_);
#elif defined(__EV_LINUX__) 
	pthread_join((pthread_t)thread_handle_, NULL);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	thread_identifying_ = 0;
	thread_handle_      = NULL;

	return true;
}

void*
#if defined(__EV_WINDOWS__)
	__stdcall
#endif // defined(__EV_WINDOWS__)
	ev_thread::thread_entry(void* arg)
{
	ev_thread* th = reinterpret_cast<ev_thread*>(arg);

	// 记录当前线程ID
	this_thread::g_thread_id = details::ev_get_thread_id_internal();

	// 记录当前线程名字
	strncpy(this_thread::g_thread_name, th->thread_name_.c_str(), sizeof(this_thread::g_thread_name) - 1);

	// 维护当前线程运行数量
	ev_atomic_fetch_and_increment(&running_thread_count_);

	if (th->thread_entry_)
	{
		th->thread_entry_(th->thread_arg_);
	}

	// 维护当前线程运行数量
	ev_atomic_fetch_and_decrement(&running_thread_count_);

	return NULL;
}

bool ev_thread::joinable(void) const
{
	if (0 == thread_identifying_ || NULL == thread_handle_)
	{
		return false;
	}

#if defined(__EV_WINDOWS__)
	uint64 temp_thread_identifying = (uint64)GetCurrentThreadId();
#elif defined(__EV_LINUX__)
	uint64 temp_thread_identifying = (uint64)pthread_self();
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	if (temp_thread_identifying == thread_identifying_)
	{
		return false;
	}

	return true;
}

namespace this_thread {

uint32 thread_id(void)
{
	// 当线程成功创建并运行的时候，线程ID是一定存在的
	// 只有一种例外情况，就是在主线程中调用获取线程ID
	if (g_thread_id != 0)
	{
		return g_thread_id;
	}

	// 当前一定是在主线程调用的
	g_thread_id = details::ev_get_thread_id_internal();

	return g_thread_id;
}

const char* thread_name(void)
{
	// 当线程成功创建并运行的时候，线程名字是一定存在的
	// 只有一种例外情况，就是在主线程中调用获取线程名字
	if (g_thread_name[0] != '\0')
	{
		return g_thread_name;
	}

	// 当前一定是在主线程调用的
	strncpy(g_thread_name, g_threadconfig_main_thread_name, sizeof(g_thread_name) - 1);

	return g_thread_name;
}

void sleep_for(uint32 milliseconds)
{
#if defined(__EV_WINDOWS__)
	Sleep(milliseconds);
#elif defined(__EV_LINUX__)
	usleep(milliseconds * 1000);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

} // namespace this_thread
} // namespace evpp