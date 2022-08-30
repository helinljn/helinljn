#ifndef __COMMON_H__
#define __COMMON_H__

#include "common-platform.h"

#include <ctime>
#include <cstdint>
#include <chrono>
#include <string>

#if defined(NDEBUG)
    #define common_assert(expr) ((void)0)
#else
    #define common_assert(expr) \
        ((void)(!!(expr) || details::assert_internal((#expr), __FILE__, __FUNCTION__, __LINE__, true)))
#endif // defined(NDEBUG)

#define abort_assert(expr) \
    ((void)(!!(expr) || details::assert_internal((#expr), __FILE__, __FUNCTION__, __LINE__, true)))

namespace details {

/**
 * @brief 内部断言实现
 * @param msg      断言信息
 * @param file     文件名
 * @param func     函数名
 * @param line     行号
 * @param is_abort 是否终止进程
 * @return
 */
int assert_internal(const char* msg, const char* file, const char* func, int line, bool is_abort);

} // namespace details

/**
 * @brief 获取系统时间(可回退)
 *        Returns the time as the number of microseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
 * @param
 * @return
 */
template<typename DurationType = std::chrono::microseconds>
inline time_t system_clock_now(void)
{
    std::chrono::system_clock::duration dt = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<DurationType>(dt).count();
}

/**
 * @brief 获取系统启动时间(不可回退)
 *        Clock that cannot be set and represents monotonic time
 * @param
 * @return
 */
template<typename DurationType = std::chrono::microseconds>
inline time_t steady_clock_now(void)
{
    std::chrono::steady_clock::duration dt = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<DurationType>(dt).count();
}

/**
 * @brief 将时间戳转换为包含日期和时间的文本表示形式
 * @param timep 时间戳
 * @return
 */
inline std::string safe_asctime(const tm* stm)
{
    char buf[64] = {0};

#if defined(__COMMON_WINDOWS__)
    asctime_s(buf, stm);
#else
    asctime_r(stm, buf);
#endif // defined(__COMMON_WINDOWS__)

    return std::string(buf);
}

/**
 * @brief 将时间戳转换为包含日期和时间的文本表示形式
 * @param timep 时间戳
 * @return
 */
inline std::string safe_ctime(const time_t* timep)
{
    char buf[64] = {0};

#if defined(__COMMON_WINDOWS__)
    ctime_s(buf, sizeof(buf), timep);
#else
    ctime_r(timep, buf);
#endif // defined(__COMMON_WINDOWS__)

    return std::string(buf);
}

/**
 * @brief 获取UTC时间的tm结构
 * @param timep 时间戳
 * @return
 */
inline tm safe_gmtime(const time_t* timep)
{
    tm result{};

#if defined(__COMMON_WINDOWS__)
    ::gmtime_s(&result, timep);
#else
    ::gmtime_r(timep, &result);
#endif // defined(__COMMON_WINDOWS__)

    return result;
}

/**
 * @brief 获取本地时间的tm结构
 * @param timep 时间戳
 * @return
 */
inline tm safe_localtime(const time_t* timep)
{
    tm result{};

#if defined(__COMMON_WINDOWS__)
    ::localtime_s(&result, timep);
#else
    ::localtime_r(timep, &result);
#endif // defined(__COMMON_WINDOWS__)

    return result;
}

/**
 * @brief 将内存数据转换为16进制字符串(默认为大写形式)
 *        该函数是按字节转换，每个字节都会转换成两个对应的字符
 *        比如：0xF8转换为：'F'和'8'
 * @param mem       需要转换的内存地址
 * @param memlen    需要转换的内存长度
 * @param result    转换的结果
 * @param uppercase 十六进制字符是否大写
 * @return 成功返回true，失败返回false
 */
bool convert_memory_to_hex_string(const void* mem, size_t memlen, std::string& result, bool uppercase = true);

/**
 * @brief 将16进制字符串转换为内存数据
 *        该函数是按双字符转换，每两个字符都会转换成对应的一个字节
 *        比如："F8"转换为：0xF8，hex_string和outbuf_len必须满足以下关系
 *        (0 == strlen(hex_string) % 2) && (outbuf_len >= strlen(hex_string) / 2)
 *        即：字符串的长度必须为偶数，输出缓冲区的长度必须 >= 字符串的长度 / 2
 * @param hex_string     需要转换的16进制字符串
 * @param hex_string_len 需要转换的16进制字符串的长度
 * @param outbuf         输出缓冲区
 * @param outbuf_len     输出缓冲区的长度
 * @return 成功返回true，失败返回false
 */
bool convert_hex_string_to_memory(const char* hex_string, size_t hex_string_len, void* outbuf, size_t outbuf_len);
bool convert_hex_string_to_memory(const std::string& hex_string, void* outbuf, size_t outbuf_len);

#endif // __COMMON_H__