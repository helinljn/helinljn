//////////////////////////////////////////////////////////////////////////
// 断言
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_ASSERT_H__
#define __EV_ASSERT_H__

#include "base/ev_types.h"

namespace evpp    {
namespace details {

/**
 * @brief 内部断言实现，请勿直接使用
 * @param msg  断言信息
 * @param file 文件名
 * @param func 函数名
 * @param line 行号
 * @return
 */
int ev_assert_internal(const char* msg, const char* file, const char* func, int line);

} // namespace details

/**
 * @brief 重定向断言输出至文件，如果没有调用该函数，那么默认会输出至stdout
 *
 *        该函数会在执行文件所在的目录下创建一个log_files目录(与日志文件同目录)
 *        然后在该目录下创建固定格式的文件，格式如下：
 *        program_name-pid-20080808-080808-assertion-failed.log
 * @param program_name 程序名(合法范围：['a', 'z'], ['A', 'Z'], ['0', '9'], '-'，'_')
 * @return 成功返回true，失败返回false
 */
bool ev_redirect_assertion(const char* program_name);

} // namespace evpp

/**
 * @brief assert断言实现
 *        Debug版本会调用abort()，Release版本则不会，两者都会将断言信息输出至相应文件中
 *        实现ev_assert，目的是为了在定义NDEBUG宏时，表达式不会被忽略掉
 *        比如以下用法：
 *            ev_assert(ev_send(sock, "a", 1, 0) != -1);
 * @param
 * @return
 */
#define ev_assert(expr) \
    ((void)((!!(expr)) || evpp::details::ev_assert_internal((#expr), __EV_FILE__, __EV_FUNC__, __EV_LINE__)))

/**
 * @brief assert断言实现
 *        Debug版本会调用abort()，Release版本则不会，两者都会将断言信息输出至相应文件中
 *        实现ev_assert_ex，目的是为了在定义NDEBUG宏时，表达式不会被忽略掉，同时输出更多的调试信息
 *        比如以下用法：
 *            int32 ret = ev_send(sock, "a", 1, 0);
 *            if (ret < 1)
 *            {
 *                char tempbuf[256];
 *                ev_snprintf(tempbuf, sizeof(tempbuf), "send() failed! ret = %d", ret);
 *                ev_assert_ex(false, tempbuf);
 *            }
 * @param
 * @return
 */
#define ev_assert_ex(expr, msg) \
    ((void)((!!(expr)) || evpp::details::ev_assert_internal((msg), __EV_FILE__, __EV_FUNC__, __EV_LINE__)))

/**
 * @brief assert静态断言实现
 *        不能应用于全局作用域，只能位于块内
 *        断言失败的编译错误为：下标不能为负
 * @param
 * @return
 */
#define ev_static_assert(expr)                                                   \
    do                                                                           \
    {                                                                            \
        typedef char ev_internal_used_for_static_assert_failed[1 - 2 * !(expr)]; \
        (void)sizeof(ev_internal_used_for_static_assert_failed);                 \
    } while (false)

#endif // __EV_ASSERT_H__