#include "libevpp.h"

#include "base/ev_assert.h"
#include "base/ev_common.h"
#include "base/ev_thread.h"
#include "base/ev_async_logging.h"
#include "net/ev_socket.h"

#include "event2/event.h"
#include "event2/thread.h"

namespace libevpp {

bool libevpp_init(const char* program_name)
{
	//////////////////////////////////////////////////////////////////////////
	// 基础类型长度检测
	//////////////////////////////////////////////////////////////////////////
	ev_static_assert(4 == sizeof(float));
	ev_static_assert(8 == sizeof(double));

	ev_static_assert(1 == sizeof(char));
	ev_static_assert(2 == sizeof(short));
	ev_static_assert(4 == sizeof(int));
	ev_static_assert(8 == sizeof(long long int));

	ev_static_assert(1 == sizeof(evpp::int8));
	ev_static_assert(2 == sizeof(evpp::int16));
	ev_static_assert(4 == sizeof(evpp::int32));
	ev_static_assert(8 == sizeof(evpp::int64));

	ev_static_assert(1 == sizeof(evpp::uint8));
	ev_static_assert(2 == sizeof(evpp::uint16));
	ev_static_assert(4 == sizeof(evpp::uint32));
	ev_static_assert(8 == sizeof(evpp::uint64));

#if defined(__EV_WINDOWS__)
	ev_static_assert(4 == sizeof(long int));
	ev_static_assert(4 == sizeof(off_t));
#else
	#if defined(__EV_WORDSIZE32__)
		ev_static_assert(4 == sizeof(long int));
		ev_static_assert(4 == sizeof(off_t));
	#else
		ev_static_assert(8 == sizeof(long int));
		ev_static_assert(8 == sizeof(off_t));
	#endif // defined(__EV_WORDSIZE32__)
#endif // defined(__EV_WINDOWS__)

#if defined(__EV_WORDSIZE32__)
	ev_static_assert(4 == sizeof(size_t));
	ev_static_assert(4 == sizeof(void*));
#else
	ev_static_assert(8 == sizeof(size_t));
	ev_static_assert(8 == sizeof(void*));
#endif // defined(__EV_WORDSIZE32__)

	//////////////////////////////////////////////////////////////////////////
	// libevent初始化
	// 可以定制自己的内存分配、日志记录、锁
	// 这些定制都应该放在代码的最前面，不能在使用libevent的event、event_base这些结构体之后
	// 因为这些结构体会使用到内存分配、日志记录、锁
	// 而这三者的定制顺序应该是：内存分配->日志记录->锁
	//////////////////////////////////////////////////////////////////////////

	// 定制内存分配
	event_set_mem_functions(libevpp_malloc, libevpp_realloc, libevpp_free);

	// 定制日志记录
	event_set_log_callback(libevpp_log_callback);
	event_set_fatal_callback(libevpp_fatal_callback);

	// 定制锁
#if defined(__EV_WINDOWS__)
	bool result = (0 == evthread_use_windows_threads());
#else
	bool result = (0 == evthread_use_pthreads());
#endif // defined(__EV_WINDOWS__)
	if (!result)
	{
		ev_assert(false && "evthread_use_x_threads() failed!");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 重定向断言输出至固定的文件
	// 如果没有调用该函数，那么默认会输出至stdout
	//////////////////////////////////////////////////////////////////////////
	result = evpp::ev_redirect_assertion(program_name);
	if (!result)
	{
		ev_assert(false && "ev_redirect_assertion() failed!");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 启动日志
	//////////////////////////////////////////////////////////////////////////
	result = ev_log_instance().start(program_name);
	if (!result)
	{
		ev_assert(false && "ev_log_instance().start() failed!");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 初始化socket
	//////////////////////////////////////////////////////////////////////////
	result = evpp::socket_utils::ev_socket_init();
	if (!result)
	{
		ev_assert(false && "ev_socket_init() failed!");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 获取当前平台支持的方法
	//////////////////////////////////////////////////////////////////////////
	ev_log_instance().set_output_to_screen(true);
	{
		ev_log_info("-- !!! The libevpp initialized successfully !!! --");
		ev_log_info("Libevent version: %s", event_get_version());
		ev_log_info("Libevent supported methods:");

		evpp::int32  idx               = 1;
		const char** supported_methods = event_get_supported_methods();
		if (NULL == supported_methods)
		{
			ev_assert(false && "event_get_supported_methods() failed!");
			return false;
		}
	
		while (*supported_methods != NULL)
		{
			ev_log_info("    [%d]. %s", idx++, *supported_methods++);
		}

		ev_log_info("--------------------------------------------------");
	}
	ev_log_instance().set_output_to_screen(false);

	return true;
}

void libevpp_destroy(void)
{
	// 销毁socket
	evpp::socket_utils::ev_socket_destroy();

	// 停止日志
	ev_log_instance().stop();

	// 释放libevent申请的全局资源
	libevent_global_shutdown();
}

void* libevpp_malloc(size_t size)
{
	return ::malloc(size);
}

void* libevpp_realloc(void* ptr, size_t size)
{
	return ::realloc(ptr, size);
}

void libevpp_free(void* ptr)
{
	return ::free(ptr);
}

void libevpp_log_callback(int severity, const char* msg)
{
	return ev_log_write(static_cast<evpp::ev_async_logging::log_level>(severity), msg);
}

void libevpp_fatal_callback(int error_code)
{
	char error_msg[128];
	evpp::ev_snprintf(error_msg, sizeof(error_msg), "Libevent fatal error! Please check you code, error_code = %d", error_code);
	ev_assert_ex(false, error_msg);

	while (true)
	{
		// !!!libevent fatal error!!!
		evpp::this_thread::sleep_for(1);
	}
}

} // namespace libevpp