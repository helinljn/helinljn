#include "file.h"
#include <cstdio>
#include <cstring>
#include <vector>

#if defined(CORE_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <direct.h>
    #include <io.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(CORE_PLATFORM_LINUX)
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
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
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)
}

} // namespace details

bool mkdir(const char* dirname, uint32_t mode)
{
    if (!dirname)
        return false;

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
    if (!dirname)
        return false;

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
    if (!path)
        return false;

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
    if (!path)
        return false;

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
    if (!path)
        return false;

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
    if (!path)
        return false;

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
    if (!buf || !buflen)
        return false;

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
    if (!buf || !buflen)
        return false;

    int32_t ret = details::get_exepath_internal(buf, buflen);
    if (-1 == ret)
        return false;

#if defined(CORE_PLATFORM_WINDOWS)
    const char* slash = strrchr(buf, '\\');
#else
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
    char     buf[2048];
    uint32_t len = sizeof(buf);
    if (!get_exedir(buf, &len))
        return std::string{};

    return std::string(buf, len);
}

} // namespace core