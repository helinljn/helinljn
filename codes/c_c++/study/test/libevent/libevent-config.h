//////////////////////////////////////////////////////////////////////////
// libevent配置、初始化和销毁
//////////////////////////////////////////////////////////////////////////
#ifndef __LIBEVENT_H__
#define __LIBEVENT_H__

#include <cstdlib>
#include <cstdint>

namespace libevent {

/**
 * @brief 初始化libevent
 * @param
 * @return
 */
void libevent_init(void);

/**
 * @brief 销毁libevent
 * @param
 * @return
 */
void libevent_destroy(void);

/**
 * @brief 内存申请函数：malloc
 * @param
 * @return
 */
void* libevent_malloc(size_t size);

/**
 * @brief 内存申请函数：calloc
 * @param
 * @return
 */
void* libevent_calloc(size_t count, size_t size);

/**
 * @brief 内存申请函数：realloc 
 * @param
 * @return
 */
void* libevent_realloc(void* ptr, size_t size);

/**
 * @brief 内存释放函数：free
 * @param
 * @return
 */
void libevent_free(void* ptr);

/**
 * @brief libevent普通日志回调函数
 * @param severity 日志严重级别
 * @param msg      日志信息
 * @return
 */
void libevent_log_callback(int severity, const char* msg);

/**
 * @brief libevent致命错误回调函数
 * @param error_code 错误码
 * @return
 */
void libevent_fatal_callback(int error_code);

} // namespace libevent

#endif // __LIBEVENT_H__