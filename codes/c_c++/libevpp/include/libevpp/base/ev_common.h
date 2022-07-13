//////////////////////////////////////////////////////////////////////////
// 常用工具函数
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_COMMON_H__
#define __EV_COMMON_H__

#include "base/ev_types.h"

#include <string>
#include <vector>
#include <cstdarg>
#include <cstdlib>

namespace evpp {

// 控制台输出颜色
enum output_color
{
	OC_GREEN  = 0,  // 绿色
	OC_WHITE  = 1,  // 白色
	OC_YELLOW = 2,  // 黄色
	OC_RED    = 3,  // 红色
	OC_BLUE   = 4,  // 蓝色
};

/**
 * @brief 设置控制台彩色字体输出(目前只支持Windows，其它平台默认返回true)
 * @param color 控制台输出颜色(如果枚举值非法，则默认为白色)
 * @return 成功返回true，失败返回false
 */
bool ev_set_console_output_color(output_color color);

/**
 * @brief 创建目录
 * @param dirname 目录名
 * @param mode    目录权限
 * @return 成功返回true，失败返回false
 */
bool ev_mkdir(const char* dirname, uint32 mode = 0755);

/**
 * @brief 删除目录(必须为空目录才能成功删除)
 * @param dirname 空目录名
 * @return 成功返回true，失败返回false
 */
bool ev_rmdir(const char* dirname);

/**
 * @brief 切换目录
 * @param path 切换的目录
 * @return 成功返回true，失败返回false
 */
bool ev_chdir(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否存在
 * @param path 需要判断的目录或者是文件
 * @return 存在返回true，否则返回false
 */
bool ev_access_exists(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可读
 * @param path 需要判断的目录或者是文件
 * @return 可读返回true，否则返回false
 */
bool ev_access_read(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可写
 * @param path 需要判断的目录或者是文件
 * @return 可写返回true，否则返回false
 */
bool ev_access_write(const char* path);

/**
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\\test\\test.exe
 *        如：/home/shmilyl/test/a.out
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
 */
bool        ev_get_exepath(char* buf, uint32* buflen);
std::string ev_get_exepath(void);

/**
 * @brief 获取当前执行文件所在目录的绝对路径(【不包含】执行文件名)
 *        如：C:\\test
 *        如：/home/shmilyl/test
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
 */
bool        ev_get_exedir(char* buf, uint32* buflen);
std::string ev_get_exedir(void);

/**
 * @brief 获取主机名称
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放主机名称的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者主机名)，失败返回false(或者空字符串)
 */
bool        ev_get_hostname(char* buf, uint32* buflen);
std::string ev_get_hostname(void);

/**
 * @brief 获取当前空闲内存(以KB为单位)
 * @param
 * @return
 */
uint32 ev_get_free_memory(void);

/**
 * @brief 获取系统总内存(以KB为单位)
 * @param
 * @return
 */
uint32 ev_get_total_memory(void);

/**
 * @brief 获取进程Id
 * @param
 * @return
 */
uint32 ev_get_process_id(void);

/**
 * @brief 格式化输出至缓冲区
 *        用法与snprintf相同，注意当缓冲区不足时，写入截断的返回值问题
 * @param buf        缓冲区
 * @param buflen     缓冲区长度
 * @param format_str 格式化字符串
 * @return 成功返回写入的字节数，失败返回-1
 */
int32 ev_snprintf(char* buf, size_t buflen, const char* format_str, ...);

/**
 * @brief 格式化输出至缓冲区
 *        用法与vsnprintf相同，注意当缓冲区不足时，写入截断的返回值问题
 * @param buf        缓冲区
 * @param buflen     缓冲区长度
 * @param format_str 格式化字符串
 * @param ap         参数列表
 * @return 成功返回写入的字节数，失败返回-1
 */
int32 ev_vsnprintf(char* buf, size_t buflen, const char* format_str, va_list ap);

/**
 * @brief 忽略大小写比较两个字符串
 * @param str1 比较的字符串1
 * @param str2 比较的字符串2
 * @return str1 == str2：返回0
 *         str1  > str2：返回值 > 0
 *         str1  < str2：返回值 < 0
 */
int32 ev_strcasecmp(const char* str1, const char* str2);

/**
 * @brief 忽略大小写比较两个字符串的前n个字符
 * @param str1 比较的字符串1
 * @param str2 比较的字符串2
 * @param n    比较的字符个数
 * @return str1 == str2：返回0
 *         str1  > str2：返回值 > 0
 *         str1  < str2：返回值 < 0
 */
int32 ev_strncasecmp(const char* str1, const char* str2, size_t n);

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
bool ev_memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outbuf_len, bool uppercase = true);

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
bool ev_hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len);

/**
 * @brief 以多分隔符为基准拆分一个字符串，并将拆分结果放入result中
 * @param src_str    待拆分的字符串
 * @param separator  多分隔符字符串
 * @param out_result 存放最终的拆分结果
 * @return
 */
void ev_split_string(const char* src_str, const char* separator, std::vector<std::string>& out_result);

/**
 * @brief 判断字符是否为英文字母(大小写都算)
 * @param ch 待测试字符
 * @return 
 */
bool ev_isalpha(char ch);

/**
 * @brief 判断字符是否为十进制数字字符
 * @param ch 待测试字符
 * @return 
 */
bool ev_isdigit(char ch);

/**
 * @brief 判断字符是是否为英文字母(大小写都算)或是否为十进制数字字符
 *        相当于 ev_isalpha(ch) || ev_isdigit(ch)
 * @param ch 待测试字符
 * @return 
 */
bool ev_isalnum(char ch);

/**
 * @brief 判断字符是否为空格字符(' ', '\t', '\r', '\n', '\v', '\f')
 * @param ch 待测试字符
 * @return 
 */
bool ev_isspace(char ch);

/**
 * @brief 判断字符是否为可打印字符(包含空格字符)
 * @param ch 待测试字符
 * @return 
 */
bool ev_isprint(char ch);

/**
 * @brief 判断字符是否为小写字母
 * @param ch 待测试字符
 * @return 
 */
bool ev_islower(char ch);

/**
 * @brief 判断字符是否为大写字母
 * @param ch 待测试字符
 * @return 
 */
bool ev_isupper(char ch);

/**
 * @brief 如果待转换字符为大写字母，则将其转换为对应的小写字母，否则原样返回
 * @param ch 待转换字符
 * @return 
 */
char ev_tolower(char ch);

/**
 * @brief 如果待转换字符为小写字母，则将其转换为对应的大写字母，否则原样返回
 * @param ch 待转换字符
 * @return 
 */
char ev_toupper(char ch);

/**
 * @brief 字符串转换为int32
 * @param str 转换字符串
 * @return 
 */
int32 ev_strtoint32(const char* str);

/**
 * @brief 字符串转换为int64
 * @param str 转换字符串
 * @return 
 */
int64 ev_strtoint64(const char* str);

/**
 * @brief 字符串转换为float
 * @param str 转换字符串
 * @return 
 */
float ev_strtof(const char* str);

/**
 * @brief 字符串转换为double
 * @param str 转换字符串
 * @return 
 */
double ev_strtod(const char* str);

/**
 * @brief 判断给定字符串的字符集是否为GBK(如果全部是ASCII，那么也算是GBK)
 * @param str 待判定字符串
 * @param len 字符串长度
 * @return 是返回true，否返回false
 */
bool ev_is_gbk(const std::string& str);
bool ev_is_gbk(const char* str, size_t len);

/**
 * @brief 判断给定字符串的字符集是否为UTF-8(如果全部是ASCII，那么也算是UTF-8)
 * @param str 待判定字符串
 * @param len 字符串长度
 * @return 是返回true，否返回false
 */
bool ev_is_utf8(const std::string& str);
bool ev_is_utf8(const char* str, size_t len);

/**
 * @brief GBK转UTF-8
 * @param gbk_str GBK字符串
 * @return 成功返回UTF-8字符串，失败返回空字符串
 */
std::string ev_gbk_to_utf8(const char* gbk_str);

/**
 * @brief UTF-8转GBK
 * @param utf8_str UTF-8字符串
 * @return 成功返回GBK字符串，失败返回空字符串
 */
std::string ev_utf8_to_gbk(const char* utf8_str);

/**
 * @brief 随机uint8整数，生成[0, UINT8_MAX]之间的uint8
 * @param
 * @return
 */
uint8 ev_random_uint8(void);

/**
 * @brief 随机uint16整数，生成[0, UINT16_MAX]之间的uint16
 * @param
 * @return
 */
uint16 ev_random_uint16(void);

/**
 * @brief 随机uint32整数，生成[0, UINT32_MAX]之间的uint32
 * @param
 * @return
 */
uint32 ev_random_uint32(void);

/**
 * @brief 随机uint64整数，生成[0, UINT64_MAX]之间的uint64
 * @param
 * @return
 */
uint64 ev_random_uint64(void);

/**
 * @brief 生成[0, upper_bound)之间的随机数，不包括upper_bound
 * @param upper_bound 随机数上边界值(取值范围必须满足 0 < upper_bound <= 0x7FFFFFFF)
 * @return
 */
int32 ev_random_range(int32 upper_bound);

/**
 * @brief 随机浮点数，生成[0, 1]之间的float
 * @param
 * @return
 */
float ev_random_float(void);

/**
 * @brief 随机浮点数，生成[0, 1]之间的double
 * @param
 * @return
 */
double ev_random_double(void);

} // namespace evpp

#endif // __EV_COMMON_H__