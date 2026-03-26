#ifndef __FILE_H__
#define __FILE_H__

#include "core_port.h"
#include <string>

namespace core {

/**
 * @brief 创建目录
 * @param dirname 目录名
 * @param mode    目录权限
 * @return 成功返回true，失败返回false
 */
CORE_API bool mkdir(const char* dirname, uint32_t mode = 0755);

/**
 * @brief 删除目录(必须为空目录才能成功删除)
 * @param dirname 空目录名
 * @return 成功返回true，失败返回false
 */
CORE_API bool rmdir(const char* dirname);

/**
 * @brief 切换目录
 * @param path 切换的目录
 * @return 成功返回true，失败返回false
 */
CORE_API bool chdir(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否存在
 * @param path 需要判断的目录或者是文件
 * @return 存在返回true，否则返回false
 */
CORE_API bool access_exists(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可读
 * @param path 需要判断的目录或者是文件
 * @return 可读返回true，否则返回false
 */
CORE_API bool access_read(const char* path);

/**
 * @brief 判断给定文件(可以是目录)是否可写
 * @param path 需要判断的目录或者是文件
 * @return 可写返回true，否则返回false
 */
CORE_API bool access_write(const char* path);

/**
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\test\test.exe
 *        如：/home/shmilyl/test/a.out
 *        自动追加'\0'字符串结束符，buflen不包含'\0'
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
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
 * @return 成功返回true(或者其路径)，失败返回false(或者空字符串)
 */
CORE_API bool        get_exedir(char* buf, uint32_t* buflen);
CORE_API std::string get_exedir(void);

} // namespace core

#endif // __FILE_H__