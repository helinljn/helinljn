#pragma once

#ifndef BASE64_H
#define BASE64_H

#include "core_port.h"
#include <string>
#include <string_view>

namespace core {

/**
 * @brief Base64转码
 * @param data 待转码的数据
 * @return 成功返回一个转码后的字符串，失败返回空字符串
 */
CORE_API std::string base64_encode(std::string_view data);

/**
 * @brief Base64解码
 * @param data 待解码的数据（长度必须是 4 的倍数，仅包含合法的 Base64 字符及填充符 '='）
 * @return 成功返回一个解码后的字符串，失败（输入为空或非法）返回空字符串
 */
CORE_API std::string base64_decode(std::string_view data);

/**
 * @brief Base64 URL 转码（使用 '-' 和 '_' 替代 '+' 和 '/'，且不含填充符 '='）
 * @param data 待转码的数据
 * @return 成功返回一个转码后的字符串，失败返回空字符串
 */
CORE_API std::string base64_url_encode(std::string_view data);

/**
 * @brief Base64 URL 解码
 * @param data 待解码的数据（URL 安全格式，可不含填充符 '='）
 * @return 成功返回一个解码后的字符串，失败（输入为空或非法）返回空字符串
 */
CORE_API std::string base64_url_decode(std::string_view data);

} // namespace core

#endif // BASE64_H