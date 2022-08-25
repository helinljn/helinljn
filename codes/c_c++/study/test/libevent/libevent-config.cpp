#include "libevent-config.h"

#include "event2/event.h"
#include "event2/thread.h"
#include "util/common.h"

namespace libevent {

void libevent_init(void)
{
    //////////////////////////////////////////////////////////////////////////
    // libevent初始化
    // 可以定制自己的内存分配、日志记录、锁
    // 这些定制都应该放在代码的最前面，不能在使用libevent的event、event_base这些结构体之后
    // 因为这些结构体会使用到内存分配、日志记录、锁
    // 而这三者的定制顺序应该是：内存分配->日志记录->锁
    //////////////////////////////////////////////////////////////////////////

    // 定制内存分配
    event_set_mem_functions(libevent_malloc, libevent_realloc, libevent_free);

    // 定制日志记录
    event_set_log_callback(libevent_log_callback);
    event_set_fatal_callback(libevent_fatal_callback);

    // 定制锁
#if defined(__COMMON_WINDOWS__)
    if (evthread_use_windows_threads() != 0)
#else
    if (evthread_use_pthreads() != 0)
#endif // defined(__COMMON_WINDOWS__)
    {
        abort_assert(false && "evthread_use_x_threads() failed!");
        return;
    }

    //////////////////////////////////////////////////////////////////////////
    // 获取当前平台支持的方法
    //////////////////////////////////////////////////////////////////////////
    fprintf(stdout, "-- !!! The libevent initialized successfully !!! --\n");
    fprintf(stdout, "Libevent version: %s\n", event_get_version());
    fprintf(stdout, "Libevent supported methods:\n");

    uint32_t     idx               = 1;
    const char** supported_methods = event_get_supported_methods();
    if (NULL == supported_methods)
    {
        abort_assert(false && "event_get_supported_methods() failed!");
        return;
    }

    while (*supported_methods != NULL)
    {
        fprintf(stdout, "    [%u]. %s\n", idx++, *supported_methods++);
    }

    fprintf(stdout, "---------------------------------------------------\n");
}

void libevent_destroy(void)
{
    // 释放libevent申请的全局资源
    libevent_global_shutdown();
}

void* libevent_malloc(size_t size)
{
    return ::malloc(size);
}

void* libevent_calloc(size_t count, size_t size)
{
    return ::calloc(count, size);
}

void* libevent_realloc(void* ptr, size_t size)
{
    return ::realloc(ptr, size);
}

void libevent_free(void* ptr)
{
    return ::free(ptr);
}

void libevent_log_callback(int severity, const char* msg)
{
    const char* severity_str = nullptr;
    switch (severity)
    {
    case EVENT_LOG_DEBUG:
        severity_str = "debug";
        break;
    case EVENT_LOG_MSG:
        severity_str = "msg";
        break;
    case EVENT_LOG_WARN:
        severity_str = "warn";
        break;
    case EVENT_LOG_ERR:
        severity_str = "err";
        break;
    default:
        severity_str = "???";
        break;
    }

    fprintf(stdout, "[%s] %s\n", severity_str, msg);
}

void libevent_fatal_callback(int error_code)
{
    fprintf(stdout, "Libevent fatal error! Please check you code, error_code = %d", error_code);

    abort_assert(false && "libevent_fatal_callback()!!!");
}

} // namespace libevent