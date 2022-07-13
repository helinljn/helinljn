//////////////////////////////////////////////////////////////////////////
// MD5签名算法
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_MD5_H__
#define __EV_MD5_H__

#include "base/ev_types.h"

#include <string>

namespace evpp {

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param data      待签名的数据
 * @param datalen   数据的长度
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_md5(const void* data, uint32 datalen, bool uppercase = true);

/**
 * @brief 计算MD5签名(默认为大写形式)
 * @param filename  待签名的文件
 * @param uppercase 签名字符串是否大写
 * @return 成功返回一个签名字符串，失败返回空字符串
 */
std::string ev_md5_file(const char* filename, bool uppercase = true);

} // namespace evpp

#endif // __EV_MD5_H__