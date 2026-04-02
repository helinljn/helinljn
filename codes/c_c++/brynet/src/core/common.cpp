#include "common.h"
#include <cstdlib>
#include <cstring>
#include <random>
#include <thread>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <cpptrace/cpptrace.hpp>

#if defined(CORE_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(CORE_PLATFORM_LINUX)
    #include <unistd.h>
    #include <time.h>
    #include <iconv.h>
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
static inline uint8_t hex_string_to_num_internal(int ch)
{
    static const uint8_t hex_table[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    if (ch < 0 || ch >= 128)
        return 0xFF;
    return hex_table[ch];
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

/**
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\test\test.exe
 *        如：/home/helin/test/a.out
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回0，失败返回-1
 */
static inline int32_t get_exepath_internal(char* buf, uint32_t* buflen)
{
    if (!buf || !buflen || *buflen == 0)
        return -1;

#if defined(CORE_PLATFORM_WINDOWS)
    const uint32_t     utf16_buflen = (*buflen > 32768 ? 32768 : *buflen);
    std::vector<WCHAR> utf16_buf(utf16_buflen);

    int32_t ret = GetModuleFileNameW(NULL, utf16_buf.data(), static_cast<int>(utf16_buflen));
    if (ret <= 0)
        return -1;

    utf16_buf[ret] = L'\0';

    // Convert to UTF-8
    ret = WideCharToMultiByte(CP_UTF8, 0, utf16_buf.data(), -1, buf, (int)*buflen, NULL, NULL);
    if (ret == 0)
        return -1;

    *buflen = ret - 1;
    return 0;
#elif defined(CORE_PLATFORM_LINUX)
    ssize_t n = *buflen - 1;
    if (n > 0)
        n = readlink("/proc/self/exe", buf, n);

    if (n == -1)
        return -1;

    buf[n]  = '\0';
    *buflen = static_cast<uint32_t>(n);

    return 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

} // namespace details

uint32_t get_free_memory(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    if (!GlobalMemoryStatusEx(&memory_status))
        return 0;

    return static_cast<uint32_t>(memory_status.ullAvailPhys / 1024);
#elif defined(CORE_PLATFORM_LINUX)
    struct sysinfo info;

    const int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.freeram * info.mem_unit / 1024);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_total_memory(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    if (!GlobalMemoryStatusEx(&memory_status))
        return 0;

    return static_cast<uint32_t>(memory_status.ullTotalPhys / 1024);
#elif defined(CORE_PLATFORM_LINUX)
    struct sysinfo info;

    const int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.totalram * info.mem_unit / 1024);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_cpu_logic_count(void)
{
    // 使用 C++11 标准库的 std::thread::hardware_concurrency() 函数
    // 该函数返回系统中可用的硬件线程数
    unsigned int count = std::thread::hardware_concurrency();
    // 如果获取失败（返回 0），则返回默认值 1
    return count > 0 ? static_cast<uint32_t>(count) : 1;
}

uint64_t get_program_running_time(void)
{
    // 静态变量，记录程序启动时间
    static uint64_t start_time = 0;

    // 首次调用时初始化启动时间
    if (start_time == 0)
    {
#if defined(CORE_PLATFORM_WINDOWS)
        // Windows 平台使用 GetTickCount64() 函数
        start_time = GetTickCount64();
#elif defined(CORE_PLATFORM_LINUX)
        // Linux 平台使用 clock_gettime() 函数
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        start_time = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
#endif // defined(CORE_PLATFORM_WINDOWS)
    }

    // 获取当前时间
    uint64_t current_time = 0;
#if defined(CORE_PLATFORM_WINDOWS)
    current_time = GetTickCount64();
#elif defined(CORE_PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    current_time = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
#endif // defined(CORE_PLATFORM_WINDOWS)

    // 返回程序运行时间（毫秒）
    return current_time - start_time;
}

uint32_t get_process_id(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return static_cast<uint32_t>(GetCurrentProcessId());
#elif defined(CORE_PLATFORM_LINUX)
    return static_cast<uint32_t>(getpid());
#endif // defined(CORE_PLATFORM_WINDOWS)
}

uint32_t get_thread_id(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return static_cast<uint32_t>(GetCurrentThreadId());
#elif defined(CORE_PLATFORM_LINUX)
    return static_cast<uint32_t>(gettid());
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outlen, bool uppercase)
{
    static const char uppercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static const char lowercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    if (!mem || 0 == memlen || !outbuf || 0 == outlen || memlen * 2 + 1 > outlen)
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

bool to_hex_string(const void* mem, size_t memlen, std::string& outstr, bool uppercase)
{
    if (!mem || 0 == memlen)
        return false;

    const size_t required_size = memlen * 2 + 1;
    outstr.resize(required_size);

    if (!memory_to_hex_string(mem, memlen, &outstr[0], required_size, uppercase))
        return false;

    // 去除末尾的'\0'字符
    outstr.resize(required_size - 1);
    return true;
}

bool hex_string_to_memory(std::string_view hexstr, void* outbuf, size_t outbuf_len)
{
    if (!outbuf || 0 == outbuf_len)
        return false;

    size_t hex_string_len = hexstr.size();
    if (hex_string_len % 2 != 0 || hex_string_len / 2 > outbuf_len)
        return false;

    uint8_t  temp_high;
    uint8_t  temp_low;
    uint8_t* temp_outbuf = reinterpret_cast<uint8_t*>(outbuf);
    for (size_t idx = 0; idx != hex_string_len; idx += 2)
    {
        temp_high = details::hex_string_to_num_internal(hexstr[idx]);
        temp_low  = details::hex_string_to_num_internal(hexstr[idx + 1]);
        if (0xFF == temp_high || 0xFF == temp_low)
            return false;

        temp_outbuf[idx / 2] = ((temp_high << 4) | temp_low);
    }

    return true;
}

bool from_hex_string(std::string_view hexstr, void* outbuf, size_t outbuf_len)
{
    if (hexstr.size() % 2 != 0)
        return false;
    return hex_string_to_memory(hexstr.data(), outbuf, outbuf_len);
}

std::string trim(std::string_view str)
{
    size_t start = 0;
    while (start < str.size() && std::isspace(str[start]))
    {
        ++start;
    }

    size_t end = str.size();
    while (end > start && std::isspace(str[end - 1]))
    {
        --end;
    }

    return std::string(str.substr(start, end - start));
}

std::string ltrim(std::string_view str)
{
    size_t start = 0;
    while (start < str.size() && std::isspace(str[start]))
    {
        ++start;
    }

    return std::string(str.substr(start));
}

std::string rtrim(std::string_view str)
{
    size_t end = str.size();
    while (end > 0 && std::isspace(str[end - 1]))
    {
        --end;
    }

    return std::string(str.substr(0, end));
}

std::string pad_left(std::string_view str, size_t total_width, char fill_char)
{
    std::string result;
    if (str.size() >= total_width)
        return std::string(str);

    result.reserve(total_width);
    result.append(total_width - str.size(), fill_char);
    result.append(str);

    return result;
}

std::string pad_right(std::string_view str, size_t total_width, char fill_char)
{
    std::string result;
    if (str.size() >= total_width)
        return std::string(str);

    result.reserve(total_width);
    result.append(str);
    result.append(total_width - str.size(), fill_char);

    return result;
}

std::string center(std::string_view str, size_t total_width, char fill_char)
{
    std::string result;
    if (str.size() >= total_width)
        return std::string(str);

    result.reserve(total_width);

    size_t pad_size  = total_width - str.size();
    size_t left_pad  = pad_size / 2;
    size_t right_pad = pad_size - left_pad;

    result.append(left_pad, fill_char);
    result.append(str);
    result.append(right_pad, fill_char);

    return result;
}

std::string to_upper(std::string_view str)
{
    std::string result;
    result.reserve(str.size());

    for (char c : str)
    {
        result.push_back(static_cast<char>(std::toupper(c)));
    }

    return result;
}

std::string to_lower(std::string_view str)
{
    std::string result;
    result.reserve(str.size());

    for (char c : str)
    {
        result.push_back(static_cast<char>(std::tolower(c)));
    }

    return result;
}

std::string replace(std::string_view str, std::string_view old_str, std::string_view new_str)
{
    if (old_str.empty())
        return std::string(str);

    std::string result;
    result.reserve(str.size());

    size_t pos     = 0;
    size_t old_len = old_str.size();
    while (pos < str.size())
    {
        size_t found = str.find(old_str, pos);
        if (found == std::string_view::npos)
        {
            result.append(str.substr(pos));
            break;
        }

        result.append(str.substr(pos, found - pos));
        result.append(new_str);
        pos = found + old_len;
    }

    return result;
}

std::string concat(std::string_view str1, std::string_view str2)
{
    std::string result;

    result.reserve(str1.size() + str2.size());
    result.append(str1);
    result.append(str2);

    return result;
}

std::string join(const std::vector<std::string>& parts, std::string_view delimiter)
{
    std::string result;
    if (parts.empty())
        return result;

    size_t total_size = 0;
    for (const auto& part : parts)
    {
        total_size += part.size();
    }
    total_size += delimiter.size() * (parts.size() - 1);

    result.reserve(total_size);
    result.append(parts[0]);

    for (size_t i = 1; i < parts.size(); ++i)
    {
        result.append(delimiter);
        result.append(parts[i]);
    }

    return result;
}

void split(std::string_view src_str, std::string_view separator, std::vector<std::string>& out_result)
{
    if (src_str.empty() || separator.empty())
        return;

    // 构建分隔符哈希集合
    std::unordered_set<char> sep_set;
    for (char c : separator)
    {
        sep_set.insert(c);
    }

    // 预分配空间，假设平均每个分隔符之间有10个字符
    out_result.reserve(src_str.size() / 10 + 1);

    size_t start = 0;
    size_t end   = 0;
    while (end < src_str.size())
    {
        if (sep_set.count(src_str[end]))
        {
            if (start < end)
                out_result.emplace_back(src_str.substr(start, end - start));

            // 跳过连续的分隔符
            while (end < src_str.size() && sep_set.count(src_str[end]))
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
        out_result.emplace_back(src_str.substr(start, end - start));
}

bool starts_with(std::string_view str, std::string_view prefix)
{
    if (prefix.size() > str.size())
        return false;

    return str.substr(0, prefix.size()) == prefix;
}

bool ends_with(std::string_view str, std::string_view suffix)
{
    if (suffix.size() > str.size())
        return false;

    return str.substr(str.size() - suffix.size()) == suffix;
}

bool contains(std::string_view str, std::string_view substr)
{
    return str.find(substr) != std::string_view::npos;
}

bool is_blank(std::string_view str)
{
    if (str.empty())
        return true;
    return std::all_of(str.begin(), str.end(), [](char c) {return std::isspace(c);});
}

bool is_digit(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) {return std::isdigit(c);});
}

bool is_alpha(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) {return std::isalpha(c);});
}

bool is_alnum(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) {return std::isalnum(c);});
}

bool is_hexdigit(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) {return std::isxdigit(c);});
}

bool is_number(std::string_view str)
{
    if (str.empty())
        return false;

    size_t start = 0;
    if (str[0] == '+' || str[0] == '-')
        start = 1;

    if (start >= str.size())
        return false;

    bool has_decimal = false;
    bool has_digit   = false;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (str[i] == '.')
        {
            if (has_decimal)
                return false;

            has_decimal = true;
            continue;
        }

        if (!std::isdigit(str[i]))
            return false;

        has_digit = true;
    }

    return has_digit;
}

int stringcmp(std::string_view str1, std::string_view str2)
{
    if (str1 == str2)
        return 0;
    if (str1 < str2)
        return -1;
    return 1;
}

int stringicmp(std::string_view str1, std::string_view str2)
{
    size_t min_len = (std::min)(str1.size(), str2.size());
    for (size_t i = 0; i < min_len; ++i)
    {
        int c1 = std::tolower(str1[i]);
        int c2 = std::tolower(str2[i]);
        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

    if (str1.size() < str2.size())
        return -1;
    if (str1.size() > str2.size())
        return 1;
    return 0;
}

size_t count_str(std::string_view str, std::string_view substr)
{
    if (substr.empty())
        return 0;

    size_t count = 0;
    size_t pos   = 0;
    while ((pos = str.find(substr, pos)) != std::string_view::npos)
    {
        ++count;
        pos += substr.size();
    }
    return count;
}

size_t count_char(std::string_view str, char ch)
{
    auto count = std::count(str.begin(), str.end(), ch);
    return static_cast<size_t>(count >= 0 ? count : 0);
}

bool is_gbk(std::string_view str)
{
    if (str.empty())
        return false;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
    size_t         len  = str.size();
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

        const uint8_t first  = data[idx];
        const uint8_t second = data[idx + 1];

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

bool is_utf8(std::string_view str)
{
    if (str.empty())
        return false;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
    size_t         len  = str.size();
    size_t         idx  = 0;
    while (idx < len)
    {
        const uint8_t byte = data[idx];
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

std::string gbk_to_utf8(std::string_view gbk_str)
{
    return details::code_convert(gbk_str.data(), [](const char* str) -> std::string {
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
        #endif // defined(CORE_PLATFORM_WINDOWS)
    });
}

std::string utf8_to_gbk(std::string_view utf8_str)
{
    return details::code_convert(utf8_str.data(), [](const char* str) -> std::string {
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
        #endif // defined(CORE_PLATFORM_WINDOWS)
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

    // 使用 random_uint32 生成随机数，并通过乘法和除法实现均匀分布
    // 这种方法可以避免取模运算可能导致的分布不均匀问题
    const uint32_t random_value = random_uint32();
    const uint64_t product      = static_cast<uint64_t>(random_value) * static_cast<uint64_t>(upper_bound);
    const uint32_t result       = static_cast<uint32_t>(product / (UINT32_MAX + 1ULL));

    return static_cast<int32_t>(result);
}

bool env_has(std::string_view name)
{
    if (name.empty())
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    char buffer[1];
    DWORD result = GetEnvironmentVariableA(name.data(), buffer, sizeof(buffer));
    return result > 0;
#elif defined(CORE_PLATFORM_LINUX)
    return getenv(name.data()) != nullptr;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

std::string env_get(std::string_view name)
{
    if (name.empty())
        return std::string{};

#if defined(CORE_PLATFORM_WINDOWS)
    // 先获取环境变量的大小
    DWORD size = GetEnvironmentVariableA(name.data(), nullptr, 0);
    if (size == 0)
        return std::string{};

    // 分配足够的缓冲区
    std::string result(size - 1, '\0');
    GetEnvironmentVariableA(name.data(), &result[0], size);
    return result;
#elif defined(CORE_PLATFORM_LINUX)
    const char* value = getenv(name.data());
    return value ? std::string(value) : std::string{};
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool env_set(std::string_view name, std::string_view value)
{
    if (name.empty())
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    return SetEnvironmentVariableA(name.data(), value.data()) != 0;
#elif defined(CORE_PLATFORM_LINUX)
    return setenv(name.data(), value.data(), 1) == 0;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool get_exepath(char* buf, uint32_t* buflen)
{
    if (!buf || !buflen)
        return false;

    const int32_t ret = details::get_exepath_internal(buf, buflen);
    return ret == 0;
}

std::string get_exepath(void)
{
    char     buf[4096];
    uint32_t len = sizeof(buf);
    if (!get_exepath(buf, &len))
        return std::string{};

    return std::string(buf, len);
}

bool get_exedir(char* buf, uint32_t* buflen)
{
    if (!buf || !buflen)
        return false;

    const int32_t ret = details::get_exepath_internal(buf, buflen);
    if (ret == -1)
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    const char* slash = strrchr(buf, '\\');
#elif defined(CORE_PLATFORM_LINUX)
    const char* slash = strrchr(buf, '/');
#endif // defined(CORE_PLATFORM_WINDOWS)
    if (!slash || slash - buf < 0)
    {
        buf[0]  = '\0';
        *buflen = 0;
        return false;
    }

    // 考虑以下特殊情况：
    //   /a.out(非Windows平台)
    if (slash == buf)
    {
        buf[1]  = '\0';
        *buflen = 1;
        return true;
    }

    uint32_t pos = static_cast<uint32_t>(slash - buf);
    buf[pos]     = '\0';
    *buflen      = pos;

    return true;
}

std::string get_exedir(void)
{
    char     buf[4096];
    uint32_t len = sizeof(buf);
    if (!get_exedir(buf, &len))
        return std::string{};

    return std::string(buf, len);
}

std::string current_stacktrace(bool with_snippets, size_t skip, size_t max_depth)
{
    // 这个库目前只能这么用，其它用法要么空字符串，要么宕机
    cpptrace::stacktrace st = cpptrace::stacktrace::current(skip, max_depth);
    std::ostringstream   ostr;
    if (with_snippets)
        st.print_with_snippets(ostr);
    else
        st.print(ostr);

    return ostr.str();
}

} // namespace core