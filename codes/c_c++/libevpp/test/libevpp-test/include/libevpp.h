//////////////////////////////////////////////////////////////////////////
// libevpp配置、初始化和销毁
//////////////////////////////////////////////////////////////////////////
#ifndef __LIBEVPP_H__
#define __LIBEVPP_H__

#include <cstdlib>

namespace libevpp {

/**
 * @brief 初始化libevpp库
 * @param program_name 程序名(合法范围：['a', 'z'], ['A', 'Z'], ['0', '9'], '-'，'_')
 * @return 成功返回true，失败返回false
 */
bool libevpp_init(const char* program_name);

/**
 * @brief 销毁libevpp库
 * @param
 * @return
 */
void libevpp_destroy(void);

/**
 * @brief 内存申请函数：malloc
 * @param
 * @return
 */
void* libevpp_malloc(size_t size);

/**
 * @brief 内存申请函数：realloc 
 * @param
 * @return
 */
void* libevpp_realloc(void* ptr, size_t size);

/**
 * @brief 内存释放函数：free
 * @param
 * @return
 */
void libevpp_free(void* ptr);

/**
 * @brief libevent普通日志回调函数
 * @param severity 日志严重级别
 * @param msg      日志信息
 * @return
 */
void libevpp_log_callback(int severity, const char* msg);

/**
 * @brief libevent致命错误回调函数
 * @param error_code 错误码
 * @return
 */
void libevpp_fatal_callback(int error_code);

} // namespace libevpp

#endif // __LIBEVPP_H__