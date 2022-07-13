//////////////////////////////////////////////////////////////////////////
// Base64转码&解码
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_BASE_64_H__
#define __EV_BASE_64_H__

#include "base/ev_types.h"

#include <string>

namespace evpp {

/**
 * @brief Base64转码
 * @param data      待转码的数据
 * @param datalen   数据的长度
 * @return 成功返回一个转码后的字符串，失败返回空字符串
 */
std::string ev_base64_encode(const void* data, size_t datalen);

/**
 * @brief Base64解码
 * @param data      待解码的数据
 * @param datalen   数据的长度
 * @return 成功返回一个解码后的字符串，失败返回空字符串
 */
std::string ev_base64_decode(const void* data, size_t datalen);

} // namespace evpp

#endif // __EV_BASE_64_H__