#ifndef __HEX_BINARY_H__
#define __HEX_BINARY_H__

#include <string>
#include <string_view>
#include <type_traits>

namespace common {

/**
 * @brief 将内存数据转换为十六进制字符串
 *        该函数是按字节转换，每个字节都会转换成两个对应的十六进制字符(0xF8转换为："F8")
 * @param mem       待转换的内存地址
 * @param memlen    内存长度
 * @param outstr    输出的十六进制字符串
 * @param uppercase 十六进制字符是否大写(默认为大写形式)
 * @return 成功返回true，失败返回false
 */
bool to_hex_string(const void* mem, const size_t memlen, std::string& outstr, const bool uppercase = true);

/**
 * @brief 将十六进制字符串转换为内存数据
 *        该函数是按双字符转换，每两个十六进制字符都会转换成对应的一个字节("F8"转换为：0xF8)
 * @param hexstr    待转换的十六进制字符串
 * @param outbuf    输出缓冲区
 * @param outbuflen 输出缓冲区的长度
 * @return 成功返回true，失败返回false
 */
bool from_hex_string(const std::string_view hexstr, void* outbuf, size_t outbuflen);

// binary to hex string
template <typename T>
bool to_hex_string(const T& t, std::string& outstr, const bool uppercase = true)
{
    static_assert(std::is_arithmetic_v<T> || std::is_aggregate_v<T>, "Invalid type!");
    return to_hex_string(static_cast<const void*>(&t), sizeof(t), outstr, uppercase);
}

// hex string to binary
template <typename T>
bool from_hex_string(const std::string_view hexstr, T& t)
{
    static_assert(!std::is_const_v<T>, "Invalid type!");
    static_assert(std::is_arithmetic_v<T> || std::is_aggregate_v<T>, "Invalid type!");
    return from_hex_string(hexstr, static_cast<void*>(&t), sizeof(t));
}

} // namespace common

#endif // __HEX_BINARY_H__