#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <string>
#include <string_view>

namespace common {

/**
 * @brief Base64编码
 * @param data 待编码的数据
 * @return 成功返回编码后的字符串，失败返回空字符串
 */
std::string base64_encode(const std::string_view& data);

/**
 * @brief Base64解码
 * @param data 待解码的数据
 * @return 成功返回解码后的字符串，失败返回空字符串
 */
std::string base64_decode(const std::string_view& data);

} // namespace common

#endif // __BASE_64_H__