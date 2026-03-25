#ifndef __COMMON_H__
#define __COMMON_H__

#include "core_port.h"
#include <string>
#include <vector>
#include <cstdlib>

namespace core {

/**
 * @brief 创建目录
 * @param dirname 目录名
 * @param mode    目录权限
 * @return 成功返回true，失败返回false
 */
bool mkdir(const char* dirname, uint32_t mode = 0755);

/**
 * @brief 删除目录(必须为空目录才能成功删除)
 * @param dirname 空目录名
 * @return 成功返回true，失败返回false
 */
bool rmdir(const char* dirname);

/**
 * @brief 切换目录
 * @param path 切换的目录
 * @return 成功返回true，失败返回false
 */
bool chdir(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否存在
 * @param path 需要判断的目录或者是文件
 * @return 存在返回true，否则返回false
 */
bool access_exists(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可读
 * @param path 需要判断的目录或者是文件
 * @return 可读返回true，否则返回false
 */
bool access_read(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可写
 * @param path 需要判断的目录或者是文件
 * @return 可写返回true，否则返回false
 */
bool access_write(const char* path);

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
 *        比如：0xF8转换为：'F'和'8'，memlen和outbuf_len必须满足以下关系
 *        outbuf_len >= memlen * 2 + 1，因为字符串最后会自动以'\0'结束
 * @param mem        需要转换的内存地址
 * @param memlen     内存长度
 * @param outbuf     输出缓冲区
 * @param outbuf_len 输出缓冲区的长度
 * @param uppercase  十六进制字符是否大写
 * @return 成功返回true，失败返回false
 */
bool memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outbuf_len, bool uppercase = true);

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

} // namespace core

#endif // __COMMON_H__