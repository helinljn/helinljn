//////////////////////////////////////////////////////////////////////////
// AES加密&解密
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_AES_H__
#define __EV_AES_H__

#include "base/ev_types.h"

namespace evpp {

/**
 * @brief AES-128加密
 * @param key         密钥
 * @param plain_text  待加密的明文
 * @param cipher_text 加密后的密文
 * @return 
 */
void ev_aes128_encrypt(const uint8 key[16], const uint8 plain_text[16], uint8 cipher_text[16]);

/**
 * @brief AES-128解密
 * @param key         密钥
 * @param cipher_text 待解密的密文
 * @param plain_text  解密后的明文
 * @return 
 */
void ev_aes128_decrypt(const uint8 key[16], const uint8 cipher_text[16], uint8 plain_text[16]);

/**
 * @brief AES-256加密
 * @param key         密钥
 * @param plain_text  待加密的明文
 * @param cipher_text 加密后的密文
 * @return 
 */
void ev_aes256_encrypt(const uint8 key[32], const uint8 plain_text[16], uint8 cipher_text[16]);

/**
 * @brief AES-256解密
 * @param key         密钥
 * @param cipher_text 待解密的密文
 * @param plain_text  解密后的明文
 * @return 
 */
void ev_aes256_decrypt(const uint8 key[32], const uint8 cipher_text[16], uint8 plain_text[16]);

} // namespace evpp

#endif // __EV_AES_H__