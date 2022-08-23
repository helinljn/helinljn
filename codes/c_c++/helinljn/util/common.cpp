#include "common.h"

#include <cstdio>
#include <cstring>

namespace common  {
namespace details {

int assert_internal(const char* msg, const char* file, const char* func, int line, bool is_abort)
{
    // 去除文件名中的路径信息
#if defined(__COMMON_WINDOWS__)
    const char* slash = strrchr(file, '\\');
#else
    const char* slash = strrchr(file, '/');
#endif // defined(__COMMON_WINDOWS__)
    if (slash)
        file = slash + 1;

    // 格式化输出断言信息
    fprintf(stdout,
        "\n----------- [ASSERTION FAILED BEGIN] -----------\n"
        "File   : %s\n"
        "Func   : %s\n"
        "Line   : %d\n"
        "Reason : %s\n"
        "----------- [ ASSERTION FAILED END ] -----------\n\n",
        file, func, line, msg
    );

    // 终止进程
    if (is_abort)
        abort();

    return 1;
}

} // namespace details

std::string convert_memory_to_hex_string(const void* mem, size_t memlen, bool uppercase)
{
    static const char uppercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static const char lowercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string ret;
    if (NULL == mem || 0 == memlen)
        return ret;

    ret.reserve(memlen * 2);

    const char*    hex_table = (uppercase ? uppercase_hex_table : lowercase_hex_table);
    const uint8_t* temp_mem  = reinterpret_cast<const uint8_t*>(mem);
    for (size_t idx = 0; idx != memlen; ++idx)
    {
        ret.push_back(hex_table[(temp_mem[idx]  & 0xF0) >> 4]);
        ret.push_back(hex_table[(temp_mem[idx]) & 0x0F]);
    }

    return ret;
}

bool convert_hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len)
{
    static const uint8_t ascii_to_uint8_table[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    0,    1,
           2,    3,    4,    5,    6,    7,    8,    9, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   10,   11,   12,   13,   14,
          15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   10,   11,   12,
          13,   14,   15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    };

    if (NULL == hex_string || NULL == outbuf || 0 == outbuf_len)
        return false;

    size_t hex_string_len = strlen(hex_string);
    if (hex_string_len % 2 != 0 || hex_string_len / 2 > outbuf_len)
        return false;

    uint8_t* temp_outbuf = reinterpret_cast<uint8_t*>(outbuf);
    for (size_t idx = 0; idx != hex_string_len; idx += 2)
    {
        uint8_t temp_high = ascii_to_uint8_table[static_cast<uint8_t>(hex_string[idx])];
        uint8_t temp_low  = ascii_to_uint8_table[static_cast<uint8_t>(hex_string[idx + 1])];
        if (0xFF == temp_high || 0xFF == temp_low)
            return false;

        temp_outbuf[idx / 2] = ((temp_high << 4) | temp_low);
    }

    return true;
}

} // namespace common