#include "base64.h"

namespace core   {
namespace base64 {

static constexpr char g_base64_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static constexpr unsigned char g_base64_decode_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

} // namespace base64

std::string base64_encode(std::string_view data)
{
    std::string ret;
    if (data.empty())
        return ret;

    // 预分配足够的空间，避免多次内存重新分配：每 3 字节原始数据编码为 4 个字符
    ret.reserve((data.size() + 2) / 3 * 4);

    const unsigned char* p   = reinterpret_cast<const unsigned char*>(data.data());
    const unsigned char* end = p + data.size();

    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (end - p >= 3)
    {
        char_array_3[0] = *p++;
        char_array_3[1] = *p++;
        char_array_3[2] = *p++;

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        ret += base64::g_base64_characters[char_array_4[0]];
        ret += base64::g_base64_characters[char_array_4[1]];
        ret += base64::g_base64_characters[char_array_4[2]];
        ret += base64::g_base64_characters[char_array_4[3]];
    }

    // 处理尾部不足 3 字节的余量（1 或 2 字节）
    int remaining = static_cast<int>(end - p);
    if (remaining > 0)
    {
        char_array_3[0] = p[0];
        char_array_3[1] = (remaining > 1) ? p[1] : static_cast<unsigned char>(0);
        char_array_3[2] = 0;

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        ret += base64::g_base64_characters[char_array_4[0]];
        ret += base64::g_base64_characters[char_array_4[1]];
        ret += (remaining > 1) ? base64::g_base64_characters[char_array_4[2]] : '=';
        ret += '=';
    }

    return ret;
}

std::string base64_decode(std::string_view data)
{
    std::string ret;
    if (data.empty())
        return ret;

    if (data.size() % 4 != 0)
        return ret;

    const unsigned char* p       = reinterpret_cast<const unsigned char*>(data.data());
    const size_t         datalen = data.size();

    // 预分配足够的空间，避免多次内存重新分配
    ret.reserve(datalen / 4 * 3);

    size_t        in_  = 0;
    int           i    = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_ < datalen && p[in_] != '=')
    {
        const unsigned char val = base64::g_base64_decode_table[p[in_]];
        if (val == 64)
            return std::string{};

        char_array_4[i++] = val;
        ++in_;

        if (i == 4)
        {
            char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
            char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0x0f) << 4) + ((char_array_4[2] & 0x3c) >> 2));
            char_array_3[2] = static_cast<unsigned char>(((char_array_4[2] & 0x03) << 6) + char_array_4[3]);

            ret += static_cast<char>(char_array_3[0]);
            ret += static_cast<char>(char_array_3[1]);
            ret += static_cast<char>(char_array_3[2]);
            i = 0;
        }
    }

    if (in_ < datalen)
    {
        // i == 0：在完整的 4-char 组边界处遇到 '='，属于非法填充（如 "ABCD===="）
        // i == 1：仅 1 个有效字符就遇到 '='，6-bit 无法表示任何字节，非法（如 "A==="）
        if (i == 0 || i == 1)
            return std::string{};

        // 验证剩余 '=' 的数量是否与 i 匹配（i==2 需要 2 个 '='，i==3 需要 1 个 '='）
        const size_t expected_padding = static_cast<size_t>(4 - i);
        if (in_ + expected_padding != datalen)
            return std::string{};

        // 验证填充区域中的所有字符都必须是 '='，拒绝如 "AB==XX" 这类混入非法字符的输入
        for (size_t k = in_; k < datalen; ++k)
        {
            if (p[k] != '=')
                return std::string{};
        }

        // 解码最后一个不完整的组
        // i==2 时：char_array_4 中有 2 个有效的 6-bit 值，输出 1 字节（char_array_3[0]）
        // i==3 时：char_array_4 中有 3 个有效的 6-bit 值，输出 2 字节（char_array_3[0] 和 [1]）
        for (int j = i; j < 4; ++j)
            char_array_4[j] = 0;

        char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
        if (i == 3)
            char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0x0f) << 4) + ((char_array_4[2] & 0x3c) >> 2));

        for (int j = 0; j < i - 1; ++j)
            ret += static_cast<char>(char_array_3[j]);
    }

    return ret;
}

std::string base64_url_encode(std::string_view data)
{
    std::string encoded = base64_encode(data);
    if (encoded.empty())
        return encoded;

    // 替换 URL 不安全字符
    for (auto& c : encoded)
    {
        if (c == '+')
            c = '-';
        else if (c == '/')
            c = '_';
    }

    // 移除末尾填充符 '='
    while (!encoded.empty() && encoded.back() == '=')
        encoded.pop_back();

    return encoded;
}

std::string base64_url_decode(std::string_view data)
{
    if (data.empty())
        return std::string{};

    // 还原 URL 安全字符为标准 Base64 字符
    std::string standard(data);
    for (auto& c : standard)
    {
        if (c == '-')
            c = '+';
        else if (c == '_')
            c = '/';
    }

    // 补齐填充符 '='，使长度为 4 的倍数
    switch (standard.size() % 4)
    {
    case 2:
        standard.append(2, '=');
        break;
    case 3:
        standard.append(1, '=');
        break;
    case 0:
        break;
    default:
        // 长度 % 4 == 1 是无效的 Base64 编码
        return std::string{};
    }

    return base64_decode(standard);
}

} // namespace core