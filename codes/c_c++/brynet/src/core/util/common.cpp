#include "common.h"
#include <cstdio>
#include <cstring>

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
    uint32_t utf16_buflen = (*buflen > 32768 ? 32768 : *buflen);
    WCHAR*   utf16_buf    = (WCHAR*)malloc(sizeof(WCHAR) * utf16_buflen);
    if (!utf16_buf)
        return -1;

    int32_t ret = GetModuleFileNameW(NULL, utf16_buf, utf16_buflen);
    if (ret <= 0)
    {
        free(utf16_buf);
        return -1;
    }

    utf16_buf[ret] = L'\0';

    // Convert to UTF-8
    ret = WideCharToMultiByte(CP_UTF8, 0, utf16_buf, -1, buf, (int)*buflen, NULL, NULL);
    if (ret == 0)
    {
        free(utf16_buf);
        return -1;
    }

    free(utf16_buf);

    *buflen = ret - 1;
    return 0;
#elif defined(CORE_PLATFORM_LINUX)
    ssize_t n = *buflen - 1;
    if (n > 0)
        n = readlink("/proc/self/exe", buf, n);

    if (n == -1)
        return -1;

    buf[n]  = '\0';
    *buflen = n;

    return 0;
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

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

#if defined(CORE_PLATFORM_WINDOWS)
/**
 * @brief Windows下字符编码转换
 * @param str       待转换编码的字符串
 * @param src_code  源字符编码代码页
 * @param dest_code 目标字符编码代码页
 * @return 成功返回对应转换的符串，失败返回空字符串
 */
static inline std::string code_convert_internal(const char* str, uint32_t src_code, uint32_t dest_code)
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
#elif defined(CORE_PLATFORM_LINUX)
/**
 * @brief Linux下字符编码转换
 * @param src_charset  源字符编码
 * @param dest_charset 目标字符编码
 * @param src_str      待转换编码的源字符串
 * @param src_strlen   源字符串长度
 * @param buf          输出缓冲区
 * @param buflen       输出缓冲区长度
 * @return
 */
static inline void code_convert_internal(const char* src_charset, const char* dest_charset,
                                            const char* src_str, size_t src_strlen, char* buf, size_t buflen)
{
    char**  pi = NULL;
    char**  po = NULL;
    iconv_t cd = (iconv_t)-1;

    if (!src_charset || !dest_charset || !src_str || !buf)
        goto exit_handle;

    pi = (char**)&src_str;
    po = &buf;
    cd = iconv_open(dest_charset, src_charset);
    if ((iconv_t)-1 == cd)
        goto exit_handle;

    if ((size_t)-1 == iconv(cd, pi, &src_strlen, po, &buflen))
        goto exit_handle;

exit_handle:
    if (cd != (iconv_t)-1)
        iconv_close(cd);
}
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)

} // namespace details

bool mkdir(const char* dirname, uint32_t mode)
{
#if defined(CORE_PLATFORM_WINDOWS)
    std::ignore = mode;
    return (0 == _mkdir(dirname));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == mkdir(dirname, static_cast<mode_t>(mode)));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool rmdir(const char* dirname)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return (0 == _rmdir(dirname));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == rmdir(dirname));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool chdir(const char* path)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return (0 == _chdir(path));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == chdir(path));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool access_exists(const char* path)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return (0 == _access(path, 0));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == access(path, F_OK));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool access_read(const char* path)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return (0 == _access(path, 4));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == access(path, R_OK));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool access_write(const char* path)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return (0 == _access(path, 2));
#elif defined(CORE_PLATFORM_LINUX)
    return (0 == access(path, W_OK));
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

bool get_exepath(char* buf, uint32_t* buflen)
{
    int32_t ret = details::get_exepath_internal(buf, buflen);
    return (0 == ret);
}

std::string get_exepath(void)
{
    char     buf[2048];
    uint32_t len = sizeof(buf);
    if (!get_exepath(buf, &len))
        return std::string{};

    return std::string(buf, len);
}

bool get_exedir(char* buf, uint32_t* buflen)
{
    int32_t ret = details::get_exepath_internal(buf, buflen);
    if (-1 == ret)
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    const char* slash = strrchr(buf, '\\');
#else
    const char* slash = strrchr(buf, '/');
#endif // defined(CORE_PLATFORM_WINDOWS)
    if (!slash && slash - buf < 0)
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
    char     buf[2048];
    uint32_t len = sizeof(buf);
    if (!get_exedir(buf, &len))
        return std::string{};

    return std::string(buf, len);
}

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
    const char* temp_str    = src_str;
    const char* temp_sep    = NULL;
    uint32_t    start_pos   = 0;
    uint32_t    cur_str_pos = 0;
    bool        flag        = false;

    if (!src_str || !separator)
        return;

    while (*temp_str)
    {
        // 每一次都重置分隔符字符串和查找标记
        temp_sep = separator;
        flag     = false;

        // 比较当前字符是否为分割字符
        while (*temp_sep)
        {
            if (*temp_str == *temp_sep)
            {
                flag = true;
                break;
            }

            ++temp_sep;
        }

        // 如果当前字符是分割字符
        if (flag)
        {
            if (start_pos < cur_str_pos)
                out_result.push_back(std::string(src_str + start_pos, cur_str_pos - start_pos));

            start_pos = cur_str_pos + 1;
        }

        ++cur_str_pos;
        ++temp_str;
    }

    // 处理最后一个分隔符后面的字符串
    if (start_pos < cur_str_pos)
        out_result.push_back(std::string(src_str + start_pos, cur_str_pos - start_pos));
}

bool is_gbk(const std::string& str)
{
    return is_gbk(str.c_str(), static_cast<size_t>(str.size()));
}

bool is_gbk(const char* str, size_t len)
{
    if (!str || 0 == len)
        return false;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);
    size_t         idx  = 0;
    while (idx < len)
    {
        // ASCII编码
        if (0x00 == (data[idx] & 0x80))
        {
            ++idx;
            continue;
        }

        // 双字节编码，首字节编码范围：0x81 ~ 0xFE，尾字节编码范围：0x40 ~ 0xFE，同时剔除0xXX7F一条线
        if (   idx + 1       <  len
            && data[idx]     >= 0x81
            && data[idx]     <= 0xFE
            && data[idx + 1] >= 0x40
            && data[idx + 1] <= 0xFE
            && data[idx + 1] != 0x7F)
        {
            idx += 2;
            continue;
        }

        // 不符合GBK编码
        return false;
    }

    return true;
}

bool is_utf8(const std::string& str)
{
    return is_utf8(str.c_str(), static_cast<size_t>(str.size()));
}

bool is_utf8(const char* str, size_t len)
{
    if (!str || 0 == len)
        return false;

    const uint8_t* data      = reinterpret_cast<const uint8_t*>(str);
    size_t         check_sub = 0;
    for (size_t idx = 0; idx < len; ++idx)
    {
        if (0 == check_sub)
        {
            if (     0x00 == (data[idx] & 0x80))  // 0XXX_XXXX
                check_sub = 0;
            else if (0xC0 == (data[idx] & 0xE0))  // 110X_XXXX 10XX_XXXX
                check_sub = 1;
            else if (0xE0 == (data[idx] & 0xF0))  // 1110_XXXX 10XX_XXXX 10XX_XXXX
                check_sub = 2;
            else if (0xF0 == (data[idx] & 0xF8))  // 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                check_sub = 3;
            else if (0xF8 == (data[idx] & 0xFC))  // 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                check_sub = 4;
            else if (0xFC == (data[idx] & 0xFE))  // 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
                check_sub = 5;
            else
                return false;
        }
        else
        {
            if ((data[idx] & 0xC0) != 0x80)
                return false;

            --check_sub;
        }
    }

    // UTF-8字符串不完整，直接否认吧，毕竟字符串不完整出现问题是迟早的事情
    return (check_sub > 0 ? false : true);
}

std::string gbk_to_utf8(const char* gbk_str)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return details::code_convert_internal(gbk_str, CP_ACP, CP_UTF8);
#elif defined(CORE_PLATFORM_LINUX)
    std::string ret_str;
    char*       temp_str    = NULL;
    size_t      temp_strlen = 0;
    size_t      gbk_strlen  = 0;

    if (!gbk_str)
        goto exit_handle;

    gbk_strlen  = strlen(gbk_str);
    temp_strlen = gbk_strlen * 3;
    temp_str    = static_cast<char*>(calloc(temp_strlen + 1, sizeof(char)));
    if (!temp_str)
        goto exit_handle;

    details::code_convert_internal("GBK", "UTF-8", gbk_str, gbk_strlen, temp_str, temp_strlen);

    ret_str = temp_str;

exit_handle:
    if (temp_str)
        free(temp_str);

    return ret_str;
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

std::string utf8_to_gbk(const char* utf8_str)
{
#if defined(CORE_PLATFORM_WINDOWS)
    return details::code_convert_internal(utf8_str, CP_UTF8, CP_ACP);
#elif defined(CORE_PLATFORM_LINUX)
    std::string ret_str;
    char*       temp_str    = NULL;
    size_t      temp_strlen = 0;
    size_t      utf8_strlen = 0;

    if (!utf8_str)
        goto exit_handle;

    utf8_strlen = strlen(utf8_str);
    temp_strlen = utf8_strlen;
    temp_str    = static_cast<char*>(calloc(temp_strlen + 1, sizeof(char)));
    if (!temp_str)
        goto exit_handle;

    details::code_convert_internal("UTF-8", "GBK", utf8_str, utf8_strlen, temp_str, temp_strlen);

    ret_str = temp_str;

exit_handle:
    if (temp_str)
        free(temp_str);

    return ret_str;
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

} // namespace core