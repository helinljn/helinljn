#include "common.h"
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <limits>
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
    #include <sys/syscall.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)

namespace core    {
namespace details {

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

static inline bool is_space_char(unsigned char ch)
{
    return std::isspace(ch) != 0;
}

static inline bool is_digit_char(unsigned char ch)
{
    return std::isdigit(ch) != 0;
}

static inline bool is_alpha_char(unsigned char ch)
{
    return std::isalpha(ch) != 0;
}

static inline bool is_alnum_char(unsigned char ch)
{
    return std::isalnum(ch) != 0;
}

static inline bool is_hexdigit_char(unsigned char ch)
{
    return std::isxdigit(ch) != 0;
}

static inline char to_upper_char(unsigned char ch)
{
    return static_cast<char>(std::toupper(ch));
}

static inline char to_lower_char(unsigned char ch)
{
    return static_cast<char>(std::tolower(ch));
}

static inline uint64_t get_monotonic_time_ms(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return GetTickCount64();
#elif defined(CORE_PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

template <typename Func>
static inline std::string code_convert(const std::string& str, Func convert_func)
{
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
static inline std::string code_convert_internal_windows(const std::string& str, uint32_t src_code, uint32_t dest_code)
{
    std::string          ret_str;
    std::vector<wchar_t> temp_wstr;
    std::vector<char>    temp_str;

    int32_t temp_len = MultiByteToWideChar(src_code, 0, str.data(), static_cast<int>(str.size()), NULL, 0);
    if (temp_len <= 0)
        return std::string{};

    temp_wstr.resize(static_cast<size_t>(temp_len));
    temp_len = MultiByteToWideChar(src_code, 0, str.data(), static_cast<int>(str.size()), temp_wstr.data(), temp_len);
    if (temp_len <= 0)
        return std::string{};

    int32_t out_len = WideCharToMultiByte(dest_code, 0, temp_wstr.data(), temp_len, NULL, 0, NULL, NULL);
    if (out_len <= 0)
        return std::string{};

    temp_str.resize(static_cast<size_t>(out_len));
    out_len = WideCharToMultiByte(dest_code, 0, temp_wstr.data(), temp_len, temp_str.data(), out_len, NULL, NULL);
    if (out_len <= 0)
        return std::string{};

    ret_str.assign(temp_str.data(), static_cast<size_t>(out_len));
    return ret_str;
}
#endif // defined(CORE_PLATFORM_WINDOWS)

static inline int32_t get_exepath_internal(char* buf, uint32_t* buflen)
{
    if (!buf || !buflen || *buflen == 0)
        return -1;

#if defined(CORE_PLATFORM_WINDOWS)
    const uint32_t utf16_buflen = (*buflen > 32768 ? 32768 : *buflen);
    if (utf16_buflen == 0)
        return -1;

    std::vector<WCHAR> utf16_buf(utf16_buflen);

    DWORD ret = GetModuleFileNameW(NULL, utf16_buf.data(), utf16_buflen);
    if (ret == 0 || ret >= utf16_buflen)
        return -1;

    // Convert to UTF-8
    ret = WideCharToMultiByte(CP_UTF8, 0, utf16_buf.data(), static_cast<int>(ret), buf, static_cast<int>(*buflen), NULL, NULL);
    if (ret <= 0 || static_cast<uint32_t>(ret) >= *buflen)
        return -1;

    buf[ret] = '\0';
    *buflen  = ret;

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

static const uint64_t s_program_start_time = get_monotonic_time_ms();

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
    // 返回程序运行时间（毫秒）
    return details::get_monotonic_time_ms() - details::s_program_start_time;
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
    return static_cast<uint32_t>(syscall(SYS_gettid));
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outlen, bool uppercase)
{
    static const char uppercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static const char lowercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    if (!mem || 0 == memlen || !outbuf || 0 == outlen)
        return false;

    if (memlen > ((std::numeric_limits<size_t>::max)() - 1) / 2)
        return false;

    const size_t required_size = memlen * 2 + 1;
    if (required_size > outlen)
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

    if (memlen > ((std::numeric_limits<size_t>::max)() - 1) / 2)
        return false;

    const size_t required_size = memlen * 2 + 1;
    std::string  temp_str(required_size, '\0');

    if (!memory_to_hex_string(mem, memlen, &temp_str[0], required_size, uppercase))
        return false;

    // 去除末尾的'\0'字符
    temp_str.resize(required_size - 1);
    outstr.swap(temp_str);

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
        temp_high = details::hex_string_to_num_internal(static_cast<unsigned char>(hexstr[idx]));
        temp_low  = details::hex_string_to_num_internal(static_cast<unsigned char>(hexstr[idx + 1]));
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
    return hex_string_to_memory(hexstr, outbuf, outbuf_len);
}

std::string trim(std::string_view str)
{
    size_t start = 0;
    while (start < str.size() && details::is_space_char(static_cast<unsigned char>(str[start])))
    {
        ++start;
    }

    size_t end = str.size();
    while (end > start && details::is_space_char(static_cast<unsigned char>(str[end - 1])))
    {
        --end;
    }

    return std::string(str.substr(start, end - start));
}

std::string ltrim(std::string_view str)
{
    size_t start = 0;
    while (start < str.size() && details::is_space_char(static_cast<unsigned char>(str[start])))
    {
        ++start;
    }

    return std::string(str.substr(start));
}

std::string rtrim(std::string_view str)
{
    size_t end = str.size();
    while (end > 0 && details::is_space_char(static_cast<unsigned char>(str[end - 1])))
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
        result.push_back(details::to_upper_char(static_cast<unsigned char>(c)));
    }

    return result;
}

std::string to_lower(std::string_view str)
{
    std::string result;
    result.reserve(str.size());

    for (char c : str)
    {
        result.push_back(details::to_lower_char(static_cast<unsigned char>(c)));
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
    out_result.clear();
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
    return std::all_of(str.begin(), str.end(), [](char c) { return details::is_space_char(static_cast<unsigned char>(c)); });
}

bool is_digit(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return details::is_digit_char(static_cast<unsigned char>(c)); });
}

bool is_alpha(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return details::is_alpha_char(static_cast<unsigned char>(c)); });
}

bool is_alnum(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return details::is_alnum_char(static_cast<unsigned char>(c)); });
}

bool is_hexdigit(std::string_view str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return details::is_hexdigit_char(static_cast<unsigned char>(c)); });
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

    bool has_decimal     = false;
    bool has_digit       = false;
    bool has_digit_after = false;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (str[i] == '.')
        {
            if (has_decimal)
                return false;

            has_decimal = true;
            continue;
        }

        if (!details::is_digit_char(static_cast<unsigned char>(str[i])))
            return false;

        has_digit = true;
        if (has_decimal)
            has_digit_after = true;
    }

    if (!has_digit)
        return false;

    if (has_decimal && str.back() == '.' && !has_digit_after)
        return false;

    return true;
}

int stringcmp(std::string_view str1, std::string_view str2)
{
    int result = str1.compare(str2);
    if (result < 0)
        return -1;
    if (result > 0)
        return 1;
    return 0;
}

int stringicmp(std::string_view str1, std::string_view str2)
{
    size_t min_len = (std::min)(str1.size(), str2.size());
    for (size_t i = 0; i < min_len; ++i)
    {
        int c1 = static_cast<unsigned char>(details::to_lower_char(static_cast<unsigned char>(str1[i])));
        int c2 = static_cast<unsigned char>(details::to_lower_char(static_cast<unsigned char>(str2[i])));
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
    return static_cast<size_t>(std::count(str.begin(), str.end(), ch));
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
    const size_t   len  = str.size();
    size_t         idx  = 0;
    while (idx < len)
    {
        const uint8_t byte = data[idx];
        if (!(byte & 0x80))
        {
            ++idx;
        }
        else if (byte >= 0xC2 && byte <= 0xDF)
        {
            if (idx + 1 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80)
                return false;
            idx += 2;
        }
        else if (byte == 0xE0)
        {
            if (idx + 2 >= len)
                return false;
            if (data[idx + 1] < 0xA0 || data[idx + 1] > 0xBF)
                return false;
            if ((data[idx + 2] & 0xC0) != 0x80)
                return false;
            idx += 3;
        }
        else if (byte >= 0xE1 && byte <= 0xEC)
        {
            if (idx + 2 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80 || (data[idx + 2] & 0xC0) != 0x80)
                return false;
            idx += 3;
        }
        else if (byte == 0xED)
        {
            if (idx + 2 >= len)
                return false;
            if (data[idx + 1] < 0x80 || data[idx + 1] > 0x9F)
                return false;
            if ((data[idx + 2] & 0xC0) != 0x80)
                return false;
            idx += 3;
        }
        else if (byte >= 0xEE && byte <= 0xEF)
        {
            if (idx + 2 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80 || (data[idx + 2] & 0xC0) != 0x80)
                return false;
            idx += 3;
        }
        else if (byte == 0xF0)
        {
            if (idx + 3 >= len)
                return false;
            if (data[idx + 1] < 0x90 || data[idx + 1] > 0xBF)
                return false;
            if ((data[idx + 2] & 0xC0) != 0x80 || (data[idx + 3] & 0xC0) != 0x80)
                return false;
            idx += 4;
        }
        else if (byte >= 0xF1 && byte <= 0xF3)
        {
            if (idx + 3 >= len)
                return false;
            if ((data[idx + 1] & 0xC0) != 0x80 || (data[idx + 2] & 0xC0) != 0x80 || (data[idx + 3] & 0xC0) != 0x80)
                return false;
            idx += 4;
        }
        else if (byte == 0xF4)
        {
            if (idx + 3 >= len)
                return false;
            if (data[idx + 1] < 0x80 || data[idx + 1] > 0x8F)
                return false;
            if ((data[idx + 2] & 0xC0) != 0x80 || (data[idx + 3] & 0xC0) != 0x80)
                return false;
            idx += 4;
        }
        else
        {
            return false;
        }
    }

    return true;
}

std::string gbk_to_utf8(const std::string& gbk_str)
{
    if (gbk_str.empty())
        return std::string{};

    return details::code_convert(gbk_str, [](const std::string& str) -> std::string {
        #if defined(CORE_PLATFORM_WINDOWS)
            // 使用 CP936 明确指定 GBK 编码
            return details::code_convert_internal_windows(str, 936, CP_UTF8);
        #elif defined(CORE_PLATFORM_LINUX)
            std::string ret_str;
            size_t      gbk_strlen = str.size();

            // 预分配足够的缓冲区
            size_t buf_size = gbk_strlen * 3 + 1;
            std::vector<char> buf(buf_size);

            // 尝试转换
            iconv_t cd = iconv_open("UTF-8", "GBK");
            if (cd == (iconv_t)-1)
                return std::string{};

            char*  inbuf  = const_cast<char*>(str.data());
            size_t inlen  = gbk_strlen;
            char*  outbuf = buf.data();
            size_t outlen = buf_size - 1;

            size_t result = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
            iconv_close(cd);

            if (result == (size_t)-1)
                return std::string{};

            ret_str.assign(buf.data(), (buf_size - 1) - outlen);
            return ret_str;
        #endif // defined(CORE_PLATFORM_WINDOWS)
    });
}

std::string utf8_to_gbk(const std::string& utf8_str)
{
    if (utf8_str.empty())
        return std::string{};

    return details::code_convert(utf8_str, [](const std::string& str) -> std::string {
        #if defined(CORE_PLATFORM_WINDOWS)
            // 使用 CP936 明确指定 GBK 编码
            return details::code_convert_internal_windows(str, CP_UTF8, 936);
        #elif defined(CORE_PLATFORM_LINUX)
            std::string ret_str;
            size_t      utf8_strlen = str.size();

            // 预分配足够的缓冲区
            size_t buf_size = utf8_strlen * 2 + 1;
            std::vector<char> buf(buf_size);

            // 尝试转换
            iconv_t cd = iconv_open("GBK", "UTF-8");
            if (cd == (iconv_t)-1)
                return std::string{};

            char*  inbuf  = const_cast<char*>(str.data());
            size_t inlen  = utf8_strlen;
            char*  outbuf = buf.data();
            size_t outlen = buf_size - 1;

            size_t result = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
            iconv_close(cd);

            if (result == (size_t)-1)
                return std::string{};

            ret_str.assign(buf.data(), (buf_size - 1) - outlen);
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

bool env_has(const std::string& name)
{
    if (name.empty())
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    DWORD size = GetEnvironmentVariableA(name.c_str(), nullptr, 0);
    return size != 0;
#elif defined(CORE_PLATFORM_LINUX)
    return getenv(name.c_str()) != nullptr;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

std::string env_get(const std::string& name)
{
    if (name.empty())
        return std::string{};

#if defined(CORE_PLATFORM_WINDOWS)
    // 先获取环境变量的大小
    DWORD size = GetEnvironmentVariableA(name.c_str(), nullptr, 0);
    if (size == 0)
        return std::string{};

    std::vector<char> buffer(size);
    DWORD ret = GetEnvironmentVariableA(name.c_str(), buffer.data(), size);
    if (ret == 0)
        return std::string{};

    return std::string(buffer.data(), ret);
#elif defined(CORE_PLATFORM_LINUX)
    const char* value = getenv(name.c_str());
    return value ? std::string(value) : std::string{};
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool env_set(const std::string& name, const std::string& value)
{
    if (name.empty())
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0;
#elif defined(CORE_PLATFORM_LINUX)
    return setenv(name.c_str(), value.c_str(), 1) == 0;
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
    std::vector<char> buf(8192);
    uint32_t          len = static_cast<uint32_t>(buf.size());
    if (!get_exepath(buf.data(), &len))
        return std::string{};

    return std::string(buf.data(), len);
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
    if (!slash)
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
    std::vector<char> buf(8192);
    uint32_t          len = static_cast<uint32_t>(buf.size());
    if (!get_exedir(buf.data(), &len))
        return std::string{};

    return std::string(buf.data(), len);
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