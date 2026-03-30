#ifndef __MD5_H__
#define __MD5_H__

#include "core_port.h"
#include <string>
#include <string_view>

namespace core {

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param data      待签名的数据
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
CORE_API std::string md5_string(std::string_view data, bool uppercase = true);

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param filename  待签名的文件
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
CORE_API std::string md5_file(const char* filename, bool uppercase = true);

} // namespace core

#endif // __MD5_H__