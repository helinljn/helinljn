//////////////////////////////////////////////////////////////////////////
// SHA1, SHA256, HmacSHA256
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_SHA1_H__
#define __EV_SHA1_H__

#include "base/ev_types.h"

#include <string>

namespace evpp {

/**
 * @brief 计算SHA1签名(默认为大写形式)
 * @param data      待签名的数据
 * @param datalen   数据的长度
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_sha1(const void* data, uint32 datalen, bool uppercase = true);

/**
 * @brief 计算SHA1签名(默认为大写形式)
 * @param filename  待签名的文件
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_sha1_file(const char* filename, bool uppercase = true);

/**
 * @brief 计算SHA256签名(默认为大写形式)
 * @param data      待签名的数据
 * @param datalen   数据的长度
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_sha256(const void* data, uint32 datalen, bool uppercase = true);

/**
 * @brief 计算HmacSHA256签名(默认为大写形式)
 * @param data      待签名的数据
 * @param datalen   数据的长度
 * @param key       密钥
 * @param keylen    密钥的长度
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_hmacsha256(const void* data, uint32 datalen, const void* key, uint32 keylen, bool uppercase = true);

} // namespace evpp

#endif // __EV_SHA1_H__