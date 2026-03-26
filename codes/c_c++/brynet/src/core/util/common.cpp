#include "common.h"
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>
#include <unordered_set>
#include <stdexcept>

#if defined(CORE_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <direct.h>
    #include <io.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(CORE_PLATFORM_LINUX)
    #include <unistd.h>
    #include <iconv.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/sysinfo.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)

namespace core    {
namespace details {

/**
 * @brief 16进制字符转换为数字
 * @param ch 16进制字符(0 ~ 9, A ~ F | a ~ f)
 * @return
 */
static inline uint8_t hex_string_to_num_internal(char ch)
{
    switch (ch)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': return 10;
    case 'B': return 11;
    case 'C': return 12;
    case 'D': return 13;
    case 'E': return 14;
    case 'F': return 15;
    case 'a': return 10;
    case 'b': return 11;
    case 'c': return 12;
    case 'd': return 13;
    case 'e': return 14;
    case 'f': return 15;
    default:  return 0xFF;
    }
}

/**
 * @brief 通用编码转换包装函数
 * @param str          待转换的字符串
 * @param convert_func 具体的转换函数
 * @return 转换后的字符串，失败时返回空字符串
 */
template <typename Func>
static inline std::string code_convert(const char* str, Func convert_func)
{
    if (!str)
        return std::string{};

    try
    {
        return convert_func(str);
    }
    catch (...)
    {
        return std::string{};
    }
}

#if defined(CORE_PLATFORM_WINDOWS)
/**
 * @brief Windows下字符编码转换
 * @param str       待转换编码的字符串
 * @param src_code  源字符编码代码页
 * @param dest_code 目标字符编码代码页
 * @return 成功返回对应转换的符串，失败返回空字符串
 */
static inline std::string code_convert_internal_windows(const char* str, uint32_t src_code, uint32_t dest_code)
{
    std::string ret_str;
    wchar_t*    temp_wstr = NULL;
    char*       temp_str  = NULL;
    int32_t     temp_len  = 0;

    if (!str)
        goto exit_handle;

    temp_len  = MultiByteToWideChar(src_code, 0, str, -1, NULL, 0);
    temp_wstr = static_cast<wchar_t*>(calloc(temp_len + 1, sizeof(wchar_t)));
    if (!temp_wstr)
        goto exit_handle;

    MultiByteToWideChar(src_code, 0, str, -1, temp_wstr, temp_len);

    temp_len = WideCharToMultiByte(dest_code, 0, temp_wstr, -1, NULL, 0, NULL, NULL);
    temp_str = static_cast<char*>(calloc(temp_len + 1, sizeof(char)));
    if (!temp_str)
        goto exit_handle;

    WideCharToMultiByte(dest_code, 0, temp_wstr, -1, temp_str, temp_len, NULL, NULL);

    ret_str = temp_str;

exit_handle:
    if (temp_wstr)
        free(temp_wstr);

    if (temp_str)
        free(temp_str);

    return ret_str;
}
#endif // defined(CORE_PLATFORM_WINDOWS)

} // namespace details

uint32_t get_free_memory(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    if (!ret)
        return 0;

    return static_cast<uint32_t>(memory_status.ullAvailPhys / 1024);
#elif defined(CORE_PLATFORM_LINUX)
    struct sysinfo info;

    int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.freeram * info.mem_unit / 1024);
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_total_memory(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    if (!ret)
        return 0;

    return static_cast<uint32_t>(memory_status.ullTotalPhys / 1024);
#elif defined(CORE_PLATFORM_LINUX)
    struct sysinfo info;

    int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.totalram * info.mem_unit / 1024);
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_cpu_logic_count(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return static_cast<uint32_t>(sys_info.dwNumberOfProcessors);
#elif defined(CORE_PLATFORM_LINUX)
    long count = sysconf(_SC_NPROCESSORS_ONLN);
    return static_cast<uint32_t>(count > 0 ? count : 1);
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_process_id(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return static_cast<uint32_t>(GetCurrentProcessId());
#elif defined(CORE_PLATFORM_LINUX)
    return static_cast<uint32_t>(getpid());
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outbuf_len, bool uppercase)
{
    static const char uppercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static const char lowercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    if (!mem || 0 == memlen || !outbuf || 0 == outbuf_len || memlen * 2 + 1 > outbuf_len)
        return false;

    const char*    hex_table = (uppercase ? uppercase_hex_table : lowercase_hex_table);
    const uint8_t* temp_mem  = reinterpret_cast<const uint8_t*>(mem);
    size_t         len       = 0;
    for (size_t idx = 0; idx != memlen; ++idx)
    {
        outbuf[len++] = hex_table[(temp_mem[idx]  & 0xF0) >> 4];
        outbuf[len++] = hex_table[(temp_mem[idx]) & 0x0F];
    }

    outbuf[len] = '\0';

    return true;
}

bool hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len)
{
    if (!hex_string || !outbuf || 0 == outbuf_len)
        return false;

    size_t hex_string_len = strlen(hex_string);
    if (hex_string_len % 2 != 0 || hex_string_len / 2 > outbuf_len)
        return false;

    uint8_t  temp_high;
    uint8_t  temp_low;
    uint8_t* temp_outbuf = reinterpret_cast<uint8_t*>(outbuf);
    for (size_t idx = 0; idx != hex_string_len; idx += 2)
    {
        temp_high = details::hex_string_to_num_internal(hex_string[idx]);
        temp_low  = details::hex_string_to_num_internal(hex_string[idx + 1]);
        if (0xFF == temp_high || 0xFF == temp_low)
            return false;

        temp_outbuf[idx / 2] = ((temp_high << 4) | temp_low);
    }

    return true;
}

void split_string(const char* src_str, const char* separator, std::vector<std::string>& out_result)
{
    if (!src_str || !separator)
        return;

    // 构建分隔符哈希集合
    std::unordered_set<char> sep_set;
    for (const char* p = separator; *p; ++p)
    {
        sep_set.insert(*p);
    }

    // 预分配空间，假设平均每个分隔符之间有10个字符
    size_t src_len = strlen(src_str);
    out_result.reserve(src_len / 10 + 1);

    const char* start = src_str;
    const char* end   = src_str;
    while (*end)
    {
        if (sep_set.count(*end))
        {
            if (start < end)
                out_result.emplace_back(start, end - start);

            // 跳过连续的分隔符
            while (*end && sep_set.count(*end))
                ++end;

            start = end;
        }
        else
        {
            ++end;
        }
    }

    // 处理最后一个分隔符后面的字符串
    if (start < end)
        out_result.emplace_back(start, end - start);
}

bool is_gbk(const std::string& str)
{
    return is_gbk(str.c_str(), str.size());
}

bool is_gbk(const char* str, size_t len)
{
    if (!str || len == 0)
        return false;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);
    size_t         idx  = 0;
    while (idx < len)
    {
        // ASCII编码
        if (!(data[idx] & 0x80))
        {
            ++idx;
            continue;
        }

        // 双字节编码检查
        if (idx + 1 >= len)
            return false;

        uint8_t first  = data[idx];
        uint8_t second = data[idx + 1];

        // 首字节范围：0x81 ~ 0xFE
        if (first < 0x81 || first > 0xFE)
            return false;

        // 尾字节范围：0x40 ~ 0xFE，排除0x7F
        if (second < 0x40 || second > 0xFE || second == 0x7F)
            return false;

        idx += 2;
    }

    return true;
}

bool is_utf8(const std::string& str)
{
    return is_utf8(str.c_str(), str.size());
}

bool is_utf8(const char* str, size_t len)
{
    if (!str || len == 0)
        return false;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);
    size_t         idx  = 0;
    while (idx < len)
    {
        uint8_t byte = data[idx];
        if (!(byte & 0x80))
        {
            // 单字节字符 (0x00-0x7F)
            ++idx;
        }
        else if ((byte & 0xE0) == 0xC0)
        {
            // 双字节字符
            if (idx + 1 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80)
                return false;
            // 检查过度编码：0xC0 0x80 是无效的（应该用单字节 0x00）
            if (byte == 0xC0 && data[idx + 1] == 0x80)
                return false;
            idx += 2;
        }
        else if ((byte & 0xF0) == 0xE0)
        {
            // 三字节字符
            if (idx + 2 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80 || (data[idx + 2] & 0xC0) != 0x80)
                return false;
            // 检查过度编码：0xE0 0x80 0x80 是无效的（应该用单字节 0x00）
            if (byte == 0xE0 && data[idx + 1] == 0x80 && data[idx + 2] == 0x80)
                return false;
            idx += 3;
        }
        else if ((byte & 0xF8) == 0xF0)
        {
            // 四字节字符 (Unicode标准最大长度)
            if (idx + 3 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80 || (data[idx + 2] & 0xC0) != 0x80 || (data[idx + 3] & 0xC0) != 0x80)
                return false;
            idx += 4;
        }
        else
        {
            // 无效的UTF-8首字节
            return false;
        }
    }

    return true;
}

std::string gbk_to_utf8(const char* gbk_str)
{
    return details::code_convert(gbk_str, [](const char* str) -> std::string {
        #if defined(CORE_PLATFORM_WINDOWS)
            // 使用 CP936 明确指定 GBK 编码
            return details::code_convert_internal_windows(str, 936, CP_UTF8);
        #elif defined(CORE_PLATFORM_LINUX)
            std::string ret_str;
            size_t gbk_strlen = strlen(str);
            // 预分配足够的缓冲区
            size_t buf_size = gbk_strlen * 3 + 1;
            std::vector<char> buf(buf_size);

            // 尝试转换
            iconv_t cd = iconv_open("UTF-8", "GBK");
            if (cd == (iconv_t)-1)
                return std::string{};

            char*  inbuf  = const_cast<char*>(str);
            size_t inlen  = gbk_strlen;
            char*  outbuf = buf.data();
            size_t outlen = buf_size - 1;

            size_t result = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
            iconv_close(cd);

            if (result == (size_t)-1)
                return std::string{};

            // 正确设置字符串长度
            buf[buf_size - outlen - 1] = '\0';
            ret_str = buf.data();
            return ret_str;
        #else
            #error "Error! I don't know what to do..."
        #endif
    });
}

std::string utf8_to_gbk(const char* utf8_str)
{
    return details::code_convert(utf8_str, [](const char* str) -> std::string {
        #if defined(CORE_PLATFORM_WINDOWS)
            // 使用 CP936 明确指定 GBK 编码
            return details::code_convert_internal_windows(str, CP_UTF8, 936);
        #elif defined(CORE_PLATFORM_LINUX)
            std::string ret_str;
            size_t utf8_strlen = strlen(str);
            // 预分配足够的缓冲区
            size_t buf_size = utf8_strlen + 1;
            std::vector<char> buf(buf_size);

            // 尝试转换
            iconv_t cd = iconv_open("GBK", "UTF-8");
            if (cd == (iconv_t)-1)
                return std::string{};

            char*  inbuf  = const_cast<char*>(str);
            size_t inlen  = utf8_strlen;
            char*  outbuf = buf.data();
            size_t outlen = buf_size - 1;

            size_t result = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
            iconv_close(cd);

            if (result == (size_t)-1)
                return std::string{};

            // 正确设置字符串长度
            buf[buf_size - outlen - 1] = '\0';
            ret_str = buf.data();
            return ret_str;
        #else
            #error "Error! I don't know what to do..."
        #endif
    });
}

uint32_t random_uint32(void)
{
    // 使用线程本地存储的随机数生成器，确保线程安全
    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_int_distribution<uint32_t> distribution;

    return distribution(generator);
}

uint64_t random_uint64(void)
{
    // 使用线程本地存储的随机数生成器，确保线程安全
    thread_local std::mt19937_64 generator(std::random_device{}());
    thread_local std::uniform_int_distribution<uint64_t> distribution;

    return distribution(generator);
}

float random_float(void)
{
    // 使用线程本地存储的随机数生成器，确保线程安全
    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<float> distribution(0.f, 1.f);

    return distribution(generator);
}

double random_double(void)
{
    // 使用线程本地存储的随机数生成器，确保线程安全
    thread_local std::mt19937_64 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);
}

int32_t random_range(int32_t upper_bound)
{
    if (upper_bound <= 0)
        return 0;

    // 使用线程本地存储的随机数生成器，确保线程安全
    thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int32_t> distribution(0, upper_bound - 1);

    return distribution(generator);
}

} // namespace core