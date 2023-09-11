#ifndef __BASE_32_H__
#define __BASE_32_H__

#include <string>
#include <string_view>

namespace common {

/**
 * @brief Base32编码
 * @param data 待编码的数据
 * @return 成功返回编码后的字符串，失败返回空字符串
 */
std::string base32_encode(const std::string_view data);

/**
 * @brief Base32解码
 * @param data 待解码的数据
 * @return 成功返回解码后的字符串，失败返回空字符串
 */
std::string base32_decode(const std::string_view data);

} // namespace common

#endif // __BASE_32_H__