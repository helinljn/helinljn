#pragma once

#ifndef AES_H
#define AES_H

#include "core_port.h"

namespace core {

/**
 * @brief AES-128加密
 * @param key         密钥
 * @param plain_text  待加密的明文
 * @param cipher_text 加密后的密文
 * @return
 */
CORE_API void aes128_encrypt(const uint8_t key[16], const uint8_t plain_text[16], uint8_t cipher_text[16]);

/**
 * @brief AES-128解密
 * @param key         密钥
 * @param cipher_text 待解密的密文
 * @param plain_text  解密后的明文
 * @return
 */
CORE_API void aes128_decrypt(const uint8_t key[16], const uint8_t cipher_text[16], uint8_t plain_text[16]);

/**
 * @brief AES-256加密
 * @param key         密钥
 * @param plain_text  待加密的明文
 * @param cipher_text 加密后的密文
 * @return
 */
CORE_API void aes256_encrypt(const uint8_t key[32], const uint8_t plain_text[16], uint8_t cipher_text[16]);

/**
 * @brief AES-256解密
 * @param key         密钥
 * @param cipher_text 待解密的密文
 * @param plain_text  解密后的明文
 * @return
 */
CORE_API void aes256_decrypt(const uint8_t key[32], const uint8_t cipher_text[16], uint8_t plain_text[16]);

} // namespace core

#endif // AES_H