#ifndef __COMMON_H__
#define __COMMON_H__

#include "common-platform.h"

#include <ctime>
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>

namespace common {

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
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\\test\\test.exe
 *        如：/home/shmilyl/test/a.out
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
 */
bool        get_exepath(char* buf, uint32_t* buflen);
std::string get_exepath(void);

/**
 * @brief 获取当前执行文件所在目录的绝对路径(【不包含】执行文件名)
 *        如：C:\\test
 *        如：/home/shmilyl/test
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
 */
bool        get_exedir(char* buf, uint32_t* buflen);
std::string get_exedir(void);

/**
 * @brief 获取主机名称
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放主机名称的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者主机名)，失败返回false(或者空字符串)
 */
bool        get_hostname(char* buf, uint32_t* buflen);
std::string get_hostname(void);

/**
 * @brief 获取当前空闲内存(以KB为单位)
 * @param
 * @return
 */
uint32_t get_free_memory(void);

/**
 * @brief 获取系统总内存(以KB为单位)
 * @param
 * @return
 */
uint32_t get_total_memory(void);

/**
 * @brief 获取进程Id
 * @param
 * @return
 */
uint32_t get_process_id(void);

/**
 * @brief 将内存数据转换为16进制字符串(默认为大写形式)
 *        该函数是按字节转换，每个字节都会转换成两个对应的字符
 *        比如：0xF8转换为：'F'和'8'
 * @param mem       需要转换的内存地址
 * @param memlen    需要转换的内存长度
 * @param uppercase 十六进制字符是否大写
 * @return 成功返回转换后的字符串，失败返回空字符串
 */
std::string memory_to_hex_string(const void* mem, size_t memlen, bool uppercase = true);

/**
 * @brief 将16进制字符串转换为内存数据
 *        该函数是按双字符转换，每两个字符都会转换成对应的一个字节
 *        比如："F8"转换为：0xF8，hex_string和outbuf_len必须满足以下关系
 *        (0 == strlen(hex_string) % 2) && (outbuf_len >= strlen(hex_string) / 2)
 *        即：字符串的长度必须为偶数，输出缓冲区的长度必须 >= 字符串的长度 / 2
 * @param hex_string 需要转换的16进制字符串
 * @param outbuf     输出缓冲区
 * @param outbuf_len 输出缓冲区的长度
 * @return 成功返回true，失败返回false
 */
bool hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len);

/**
 * @brief 以多分隔符为基准拆分一个字符串，并将拆分结果放入result中
 * @param src_str    待拆分的字符串
 * @param separator  多分隔符字符串
 * @param out_result 存放最终的拆分结果
 * @return
 */
void split_string(const char* src_str, const char* separator, std::vector<std::string>& out_result);

/**
 * @brief 判断给定字符串的字符集是否为GBK(如果全部是ASCII，那么也算是GBK)
 * @param str 待判定字符串
 * @param len 字符串长度
 * @return 是返回true，否返回false
 */
bool is_gbk(const std::string& str);
bool is_gbk(const char* str, size_t len);

/**
 * @brief 判断给定字符串的字符集是否为UTF-8(如果全部是ASCII，那么也算是UTF-8)
 * @param str 待判定字符串
 * @param len 字符串长度
 * @return 是返回true，否返回false
 */
bool is_utf8(const std::string& str);
bool is_utf8(const char* str, size_t len);

/**
 * @brief GBK转UTF-8
 * @param gbk_str GBK字符串
 * @return 成功返回UTF-8字符串，失败返回空字符串
 */
std::string gbk_to_utf8(const char* gbk_str);

/**
 * @brief UTF-8转GBK
 * @param utf8_str UTF-8字符串
 * @return 成功返回GBK字符串，失败返回空字符串
 */
std::string utf8_to_gbk(const char* utf8_str);

} // namespace common

#endif // __COMMON_H__