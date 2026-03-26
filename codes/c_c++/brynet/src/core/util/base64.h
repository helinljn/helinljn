#ifndef __BASE_64_H__
#define __BASE_64_H__

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
 * @param data 待解码的数据
 * @return 成功返回一个解码后的字符串，失败返回空字符串
 */
CORE_API std::string base64_decode(std::string_view data);

} // namespace core

#endif // __BASE_64_H__