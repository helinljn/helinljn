#pragma once

#ifndef MD5_H
#define MD5_H

#include "core_port.h"
#include <string>

namespace core {

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param data      待签名的数据(允许为空，空字符串会返回其有效MD5)
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
CORE_API std::string md5_string(const std::string& data, bool uppercase = true);

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param filename  待签名的文件路径
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串(如文件不存在或无法打开)
 */
CORE_API std::string md5_file(const std::string& filename, bool uppercase = true);

} // namespace core

#endif // MD5_H