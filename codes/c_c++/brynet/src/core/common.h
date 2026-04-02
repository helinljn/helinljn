#ifndef __COMMON_H__
#define __COMMON_H__

#include "core_port.h"
#include <vector>
#include <string>
#include <string_view>
#include <type_traits>

namespace core {

/**
 * @brief 获取当前空闲内存(以KB为单位)
 * @param
 * @return
 */
CORE_API uint32_t get_free_memory(void);

/**
 * @brief 获取系统总内存(以KB为单位)
 * @param
 * @return
 */
CORE_API uint32_t get_total_memory(void);

/**
 * @brief 获取CPU逻辑单元数量
 * @param
 * @return
 */
CORE_API uint32_t get_cpu_logic_count(void);

/**
 * @brief 获取当前程序已经运行的时间（毫秒）
 * @param
 * @return 程序运行时间，单位为毫秒
 */
CORE_API uint64_t get_program_running_time(void);

/**
 * @brief 获取进程Id
 * @param
 * @return
 */
CORE_API uint32_t get_process_id(void);

/**
 * @brief 获取当前线程Id
 * @param
 * @return
 */
CORE_API uint32_t get_thread_id(void);

/**
 * @brief 将内存数据转换为16进制字符串(默认为大写形式)
 *        该函数是按字节转换，每个字节都会转换成两个对应的字符
 *        比如：0xF8转换为：'F'和'8'，memlen和outlen必须满足以下关系
 *        outlen >= memlen * 2 + 1，因为字符串最后会自动以'\0'结束
 * @param mem       需要转换的内存地址
 * @param memlen    内存长度
 * @param outstr    输出的十六进制字符串
 * @param outbuf    输出缓冲区
 * @param outlen    输出缓冲区的长度
 * @param uppercase 十六进制字符是否大写
 * @return 成功返回true，失败返回false
 */
CORE_API bool memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outlen, bool uppercase = true);
CORE_API bool to_hex_string(const void* mem, size_t memlen, std::string& outstr, bool uppercase = true);

/**
 * @brief 将16进制字符串转换为内存数据
 *        该函数是按双字符转换，每两个字符都会转换成对应的一个字节
 *        比如："F8"转换为：0xF8，hexstr和outlen必须满足以下关系
 *        (0 == strlen(hex_string) % 2) && (outlen >= strlen(hex_string) / 2)
 *        即：字符串的长度必须为偶数，输出缓冲区的长度必须 >= 字符串的长度 / 2
 * @param hexstr 需要转换的16进制字符串
 * @param outbuf 输出缓冲区
 * @param outlen 输出缓冲区的长度
 * @return 成功返回true，失败返回false
 */
CORE_API bool hex_string_to_memory(std::string_view hexstr, void* outbuf, size_t outlen);
CORE_API bool from_hex_string(std::string_view hexstr, void* outbuf, size_t outlen);

/**
 * @brief 将任意算术类型或聚合类型转换为十六进制字符串
 * @param t         要转换的值
 * @param outstr    输出的十六进制字符串
 * @param uppercase 十六进制字符是否大写，默认为true
 * @return 成功返回true，失败返回false
 */
template <typename T>
bool to_hex_string(const T& t, std::string& outstr, const bool uppercase = true)
{
    static_assert(std::is_arithmetic_v<T> || std::is_aggregate_v<T>, "Invalid type!");
    static_assert(std::is_trivial_v<T>, "Type must be trivial!");
    return to_hex_string(static_cast<const void*>(&t), sizeof(t), outstr, uppercase);
}

/**
 * @brief 将十六进制字符串转换为任意算术类型或聚合类型
 * @param hexstr 输入的十六进制字符串
 * @param t      输出的值
 * @return 成功返回true，失败返回false
 */
template <typename T>
bool from_hex_string(std::string_view hexstr, T& t)
{
    static_assert(std::is_arithmetic_v<T> || std::is_aggregate_v<T>, "Invalid type!");
    static_assert(std::is_trivial_v<T>, "Type must be trivial!");
    return from_hex_string(hexstr, static_cast<void*>(&t), sizeof(t));
}

/**
 * @brief 去除字符串两端的空白字符
 * @param str 待处理的字符串
 * @return 处理后的字符串
 */
CORE_API std::string trim(std::string_view str);

/**
 * @brief 去除字符串左侧的空白字符
 * @param str 待处理的字符串
 * @return 处理后的字符串
 */
CORE_API std::string ltrim(std::string_view str);

/**
 * @brief 去除字符串右侧的空白字符
 * @param str 待处理的字符串
 * @return 处理后的字符串
 */
CORE_API std::string rtrim(std::string_view str);

/**
 * @brief 左对齐字符串，使字符串总宽度为total_width
 * @param str         待处理的字符串
 * @param total_width 目标宽度
 * @param fill_char   填充字符
 * @return 左对齐后的字符串
 */
CORE_API std::string pad_left(std::string_view str, size_t total_width, char fill_char = ' ');

/**
 * @brief 右对齐字符串，使字符串总宽度为total_width
 * @param str         待处理的字符串
 * @param total_width 目标宽度
 * @param fill_char   填充字符
 * @return 右对齐后的字符串
 */
CORE_API std::string pad_right(std::string_view str, size_t total_width, char fill_char = ' ');

/**
 * @brief 居中对齐字符串，使字符串总宽度为total_width
 * @param str         待处理的字符串
 * @param total_width 目标宽度
 * @param fill_char   填充字符
 * @return 居中对齐后的字符串
 */
CORE_API std::string center(std::string_view str, size_t total_width, char fill_char = ' ');

/**
 * @brief 将字符串转换为大写
 * @param str 待处理的字符串
 * @return 转换后的字符串
 */
CORE_API std::string to_upper(std::string_view str);

/**
 * @brief 将字符串转换为小写
 * @param str 待处理的字符串
 * @return 转换后的字符串
 */
CORE_API std::string to_lower(std::string_view str);

/**
 * @brief 替换字符串中的子串
 * @param str      待处理的字符串
 * @param old_str  要替换的子串
 * @param new_str  替换后的子串
 * @return 替换后的字符串
 */
CORE_API std::string replace(std::string_view str, std::string_view old_str, std::string_view new_str);

/**
 * @brief 连接两个字符串
 * @param str1 第一个字符串
 * @param str2 第二个字符串
 * @return 连接后的字符串
 */
CORE_API std::string concat(std::string_view str1, std::string_view str2);

/**
 * @brief 用分隔符连接字符串列表
 * @param parts     字符串列表
 * @param delimiter 分隔符
 * @return 连接后的字符串
 */
CORE_API std::string join(const std::vector<std::string>& parts, std::string_view delimiter);

/**
 * @brief 以多分隔符为基准拆分一个字符串，并将拆分结果放入result中
 * @param src_str    待拆分的字符串
 * @param separator  多分隔符字符串
 * @param out_result 存放最终的拆分结果
 * @return
 */
CORE_API void split(std::string_view src_str, std::string_view separator, std::vector<std::string>& out_result);

/**
 * @brief 判断字符串是否以指定前缀开头
 * @param str    待检查的字符串
 * @param prefix 前缀字符串
 * @return 是返回true，否返回false
 */
CORE_API bool starts_with(std::string_view str, std::string_view prefix);

/**
 * @brief 判断字符串是否以指定后缀结尾
 * @param str    待检查的字符串
 * @param suffix 后缀字符串
 * @return 是返回true，否返回false
 */
CORE_API bool ends_with(std::string_view str, std::string_view suffix);

/**
 * @brief 判断字符串是否包含指定子串
 * @param str    待检查的字符串
 * @param substr 子串
 * @return 包含返回true，不包含返回false
 */
CORE_API bool contains(std::string_view str, std::string_view substr);

/**
 * @brief 判断字符串是否只包含空白字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_blank(std::string_view str);

/**
 * @brief 判断字符串是否只包含数字字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_digit(std::string_view str);

/**
 * @brief 判断字符串是否只包含字母字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_alpha(std::string_view str);

/**
 * @brief 判断字符串是否只包含字母数字字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_alnum(std::string_view str);

/**
 * @brief 判断字符串是否只包含十六进制数字字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_hexdigit(std::string_view str);

/**
 * @brief 判断字符串是否只包含数字字符
 * @param str 待检查的字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_number(std::string_view str);

/**
 * @brief 比较两个字符串是否相等(区分大小写)
 * @param str1 第一个字符串
 * @param str2 第二个字符串
 * @return 相等返回0，不相等返回非0
 */
CORE_API int stringcmp(std::string_view str1, std::string_view str2);

/**
 * @brief 比较两个字符串是否相等(忽略大小写)
 * @param str1 第一个字符串
 * @param str2 第二个字符串
 * @return 相等返回0，不相等返回非0
 */
CORE_API int stringicmp(std::string_view str1, std::string_view str2);

/**
 * @brief 统计字符串中指定子串出现的次数
 * @param str 待统计的字符串
 * @param substr 待统计的字符串
 * @return 子串出现的次数
 */
CORE_API size_t count_str(std::string_view str, std::string_view substr);

/**
 * @brief 统计字符串中指定字符出现的次数
 * @param str 待统计的字符串
 * @param ch 待统计的字符
 * @return 字符出现的次数
 */
CORE_API size_t count_char(std::string_view str, char ch);

/**
 * @brief 判断给定字符串的字符集是否为GBK(如果全部是ASCII，那么也算是GBK)
 * @param str 待判定字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_gbk(std::string_view str);

/**
 * @brief 判断给定字符串的字符集是否为UTF-8(如果全部是ASCII，那么也算是UTF-8)
 * @param str 待判定字符串
 * @return 是返回true，否返回false
 */
CORE_API bool is_utf8(std::string_view str);

/**
 * @brief GBK转UTF-8
 * @param gbk_str GBK字符串
 * @return 成功返回UTF-8字符串，失败返回空字符串
 */
CORE_API std::string gbk_to_utf8(std::string_view gbk_str);

/**
 * @brief UTF-8转GBK
 * @param utf8_str UTF-8字符串
 * @return 成功返回GBK字符串，失败返回空字符串
 */
CORE_API std::string utf8_to_gbk(std::string_view utf8_str);

/**
 * @brief 随机uint32_t整数，生成[0, UINT32_MAX]之间的uint32_t
 * @param
 * @return
 */
CORE_API uint32_t random_uint32(void);

/**
 * @brief 随机uint64_t整数，生成[0, UINT64_MAX]之间的uint64_t
 * @param
 * @return
 */
CORE_API uint64_t random_uint64(void);

/**
 * @brief 随机浮点数，生成[0, 1]之间的float
 * @param
 * @return
 */
CORE_API float random_float(void);

/**
 * @brief 随机浮点数，生成[0, 1]之间的double
 * @param
 * @return
 */
CORE_API double random_double(void);

/**
 * @brief 生成[0, upper_bound)之间的随机数，不包括upper_bound
 * @param upper_bound 随机数上边界值(取值范围必须满足 0 < upper_bound <= 0x7FFFFFFF)
 * @return
 */
CORE_API int32_t random_range(int32_t upper_bound);

/**
 * @brief 判断是否存在指定的环境变量
 * @param name 环境变量名称
 * @return 存在返回true，不存在返回false
 */
CORE_API bool env_has(std::string_view name);

/**
 * @brief 获取指定环境变量的值
 * @param name 环境变量名称
 * @return 环境变量的值，如果不存在返回空字符串
 */
CORE_API std::string env_get(std::string_view name);

/**
 * @brief 设置指定环境变量的值
 * @param name 环境变量名称
 * @param value 环境变量的值
 * @return 设置成功返回true，失败返回false
 */
CORE_API bool env_set(std::string_view name, std::string_view value);

/**
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\test\test.exe
 *        如：/home/shmilyl/test/a.out
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true，失败返回false
 */
CORE_API bool        get_exepath(char* buf, uint32_t* buflen);
CORE_API std::string get_exepath(void);

/**
 * @brief 获取当前执行文件所在目录的绝对路径(【不包含】执行文件名)
 *        如：C:\test
 *        如：/home/shmilyl/test
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true，失败返回false
 */
CORE_API bool        get_exedir(char* buf, uint32_t* buflen);
CORE_API std::string get_exedir(void);

/**
 * @brief 获取当前栈跟踪信息
 * @param with_snippets 是否包含代码片段
 * @param skip          跳过的栈帧数
 * @param max_depth     最大栈帧数
 * @return
 */
CORE_API std::string current_stacktrace(bool with_snippets, size_t skip = 1, size_t max_depth = 128);

} // namespace core

#endif // __COMMON_H__