#include "common.h"

#if defined(__COMMON_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WinSock2.h>
    #include <direct.h>
    #include <io.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(__COMMON_LINUX__)
    #include <unistd.h>
    #include <iconv.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/sysinfo.h>
#endif // defined(__COMMON_WINDOWS__)

#include <cstdlib>
#include <cstring>

namespace common  {
namespace details {

static inline int32_t get_exepath_internal(char* buf, uint32_t* buflen)
{
    if (NULL == buf || NULL == buflen || 0 == *buflen)
        return -1;

#if defined(__COMMON_WINDOWS__)
    uint32_t utf16_buflen = (*buflen > 32768 ? 32768 : *buflen);
    WCHAR*   utf16_buf    = (WCHAR*)malloc(sizeof(WCHAR) * utf16_buflen);
    if (NULL == utf16_buf)
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
    if (0 == ret)
    {
        free(utf16_buf);
        return -1;
    }

    free(utf16_buf);

    *buflen = ret - 1;
    return 0;
#elif defined(__COMMON_LINUX__)
    ssize_t n = *buflen - 1;
    if (n > 0)
        n = readlink("/proc/self/exe", buf, n);

    if (-1 == n)
        return -1;

    buf[n] = '\0';
    *buflen = n;

    return 0;
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

static inline uint8_t hex_string_to_num_internal(char ch)
{
    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return ch - 48;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return ch - 55;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return ch - 87;
    default:
        return 0xFF;
    }
}

#if defined(__COMMON_WINDOWS__)
static inline std::string code_convert_internal(const char* str, uint32_t src_code, uint32_t dest_code)
{
    std::string ret_str;
    wchar_t*    temp_wstr = NULL;
    char*       temp_str = NULL;
    int32_t     temp_len = 0;

    if (NULL == str)
        goto exit_handle;

    temp_len = MultiByteToWideChar(src_code, 0, str, -1, NULL, 0);
    temp_wstr = static_cast<wchar_t*>(calloc(temp_len + 1, sizeof(wchar_t)));
    if (NULL == temp_wstr)
        goto exit_handle;

    MultiByteToWideChar(src_code, 0, str, -1, temp_wstr, temp_len);

    temp_len = WideCharToMultiByte(dest_code, 0, temp_wstr, -1, NULL, 0, NULL, NULL);
    temp_str = static_cast<char*>(calloc(temp_len + 1, sizeof(char)));
    if (NULL == temp_str)
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
#elif defined(__COMMON_LINUX__)
static inline void code_convert_internal(const char* src_charset, const char* dest_charset, const char* src_str, size_t src_strlen, char* buf, size_t buflen)
{
    char**  pi = NULL;
    char**  po = NULL;
    iconv_t cd = (iconv_t)-1;

    if (NULL == src_charset || NULL == dest_charset || NULL == src_str || NULL == buf)
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
#endif // defined(__COMMON_WINDOWS__)

} // namespace details

bool get_exepath(char* buf, uint32_t* buflen)
{
    return (0 == details::get_exepath_internal(buf, buflen));
}

std::string get_exepath(void)
{
    char     buf[4096] = {0};
    uint32_t len       = sizeof(buf);
    return (get_exepath(buf, &len) ? std::string(buf, len) : std::string());
}

bool get_exedir(char* buf, uint32_t* buflen)
{
    if (-1 == details::get_exepath_internal(buf, buflen))
        return false;

#if defined(__COMMON_WINDOWS__)
    const char* slash = strrchr(buf, '\\');
#else
    const char* slash = strrchr(buf, '/');
#endif // defined(__COMMON_WINDOWS__)
    if (NULL == slash && slash - buf < 0)
    {
        buf[0] = '\0';
        *buflen = 0;
        return false;
    }

    // 考虑以下特殊情况：
    //   /a.out(非Windows平台)
    if (slash == buf)
    {
        buf[1] = '\0';
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
    char     buf[4096] = {0};
    uint32_t len       = sizeof(buf);
    return (get_exedir(buf, &len) ? std::string(buf, len) : std::string());
}

bool get_hostname(char* buf, uint32_t* buflen)
{
    if (NULL == buf || NULL == buflen || 0 == *buflen)
        return false;

    char tempbuf[1024] = {0};
    if (gethostname(tempbuf, sizeof(tempbuf)) != 0)
        return false;

    // 当buf的大小不足以存储hostname时，应该返回失败，而不是被截断的字符串
    uint32_t tempbuf_len = static_cast<uint32_t>(strlen(tempbuf));
    if (tempbuf_len > *buflen - 1)
        return false;

    memcpy(buf, tempbuf, tempbuf_len);
    buf[tempbuf_len] = '\0';
    *buflen          = tempbuf_len;

    return true;
}

std::string get_hostname(void)
{
    char     buf[1024] = { 0 };
    uint32_t len       = sizeof(buf);
    return (get_hostname(buf, &len) ? std::string(buf, len) : std::string());
}

uint32_t get_free_memory(void)
{
#if defined(__COMMON_WINDOWS__)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    if (!ret)
        return 0;

    return static_cast<uint32_t>(memory_status.ullAvailPhys / 1024);
#elif defined(__COMMON_LINUX__)
    struct sysinfo info;

    int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.freeram * info.mem_unit / 1024);
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

uint32_t get_total_memory(void)
{
#if defined(__COMMON_WINDOWS__)
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    if (!ret)
        return 0;

    return static_cast<uint32_t>(memory_status.ullTotalPhys / 1024);
#elif defined(__COMMON_LINUX__)
    struct sysinfo info;

    int32_t ret = sysinfo(&info);
    if (ret)
        return 0;

    return static_cast<uint32_t>(info.totalram * info.mem_unit / 1024);
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

uint32_t get_process_id(void)
{
#if defined(__COMMON_WINDOWS__)
    return static_cast<uint32_t>(GetCurrentProcessId());
#elif defined(__COMMON_LINUX__)
    return static_cast<uint32_t>(getpid());
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

std::string memory_to_hex_string(const void* mem, size_t memlen, bool uppercase)
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

bool hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len)
{
    if (NULL == hex_string || NULL == outbuf || 0 == outbuf_len)
        return false;

    size_t hex_string_len = strlen(hex_string);
    if (hex_string_len % 2 != 0 || hex_string_len / 2 > outbuf_len)
        return false;

    uint8_t* temp_outbuf = reinterpret_cast<uint8_t*>(outbuf);
    for (size_t idx = 0; idx != hex_string_len; idx += 2)
    {
        uint8_t temp_high = details::hex_string_to_num_internal(hex_string[idx]);
        uint8_t temp_low  = details::hex_string_to_num_internal(hex_string[idx + 1]);
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
    int32_t     start_pos   = 0;
    int32_t     cur_str_pos = 0;
    bool        flag        = false;

    if (NULL == src_str || NULL == separator)
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
    return is_gbk(str.c_str(), str.size());
}

bool is_gbk(const char* str, size_t len)
{
    if (NULL == str || 0 == len)
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
        if (idx + 1 < len
            && data[idx] >= 0x81
            && data[idx] <= 0xFE
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
    return is_utf8(str.c_str(), str.size());
}

bool is_utf8(const char* str, size_t len)
{
    if (NULL == str || 0 == len)
        return false;

    const uint8_t* data      = reinterpret_cast<const uint8_t*>(str);
    size_t         check_sub = 0;
    for (size_t idx = 0; idx < len; ++idx)
    {
        if (0 == check_sub)
        {
            if      (0x00 == (data[idx] & 0x80))  // 0XXX_XXXX
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
#if defined(__COMMON_WINDOWS__)
    return details::code_convert_internal(gbk_str, CP_ACP, CP_UTF8);
#elif defined(__COMMON_LINUX__)
    std::string ret_str;
    char*       temp_str    = NULL;
    size_t      temp_strlen = 0;
    size_t      gbk_strlen  = 0;

    if (NULL == gbk_str)
        goto exit_handle;

    gbk_strlen  = strlen(gbk_str);
    temp_strlen = gbk_strlen * 3;
    temp_str    = static_cast<char*>(calloc(temp_strlen + 1, sizeof(char)));
    if (NULL == temp_str)
        goto exit_handle;

    details::code_convert_internal("GBK", "UTF-8", gbk_str, gbk_strlen, temp_str, temp_strlen);

    ret_str = temp_str;

exit_handle:
    if (temp_str)
        free(temp_str);

    return ret_str;
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

std::string utf8_to_gbk(const char* utf8_str)
{
#if defined(__COMMON_WINDOWS__)
    return details::code_convert_internal(utf8_str, CP_UTF8, CP_ACP);
#elif defined(__COMMON_LINUX__)
    std::string ret_str;
    char*       temp_str    = NULL;
    size_t      temp_strlen = 0;
    size_t      utf8_strlen = 0;

    if (NULL == utf8_str)
        goto exit_handle;

    utf8_strlen = strlen(utf8_str);
    temp_strlen = utf8_strlen;
    temp_str    = static_cast<char*>(calloc(temp_strlen + 1, sizeof(char)));
    if (NULL == temp_str)
        goto exit_handle;

    details::code_convert_internal("UTF-8", "GBK", utf8_str, utf8_strlen, temp_str, temp_strlen);

    ret_str = temp_str;

exit_handle:
    if (temp_str)
        free(temp_str);

    return ret_str;
#else
    #error "Error! I don't know what to do..."
#endif // defined(__COMMON_WINDOWS__)
}

} // namespace common