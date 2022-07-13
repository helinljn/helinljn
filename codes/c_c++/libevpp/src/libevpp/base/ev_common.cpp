#include "base/ev_common.h"
#include "event2/util.h"
#include "util-internal.h"
#include "mm-internal.h"

#if defined(__EV_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <direct.h>
    #include <io.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(__EV_LINUX__)
    #include <unistd.h>
    #include <iconv.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/sysinfo.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

#include <cstdio>
#include <cstring>

namespace evpp    {
namespace details {

/**
 * @brief 获取当前执行文件的绝对路径(【包含】执行文件名)
 *        如：C:\test\test.exe
 *        如：/home/helin/test/a.out
 * @param buf    存放路径的缓冲区
 * @param buflen 缓冲区的大小(成功时，会修改为实际占用大小)
 * @return 成功返回0，失败返回-1
 */
static inline int32 ev_get_exepath_internal(char* buf, uint32* buflen)
{
	if (NULL == buf || NULL == buflen || 0 == *buflen)
	{
		return -1;
	}

#if defined(__EV_WINDOWS__)
	uint32 utf16_buflen = (*buflen > 32768 ? 32768 : *buflen);
	WCHAR* utf16_buf    = (WCHAR*)mm_malloc(sizeof(WCHAR) * utf16_buflen);
	if (NULL == utf16_buf)
	{
		return -1;
	}

	int32 ret = GetModuleFileNameW(NULL, utf16_buf, utf16_buflen);
	if (ret <= 0)
	{
		mm_free(utf16_buf);
		return -1;
	}

	utf16_buf[ret] = L'\0';

	// Convert to UTF-8
	ret = WideCharToMultiByte(CP_UTF8, 0, utf16_buf, -1, buf, (int)*buflen, NULL, NULL);
	if (0 == ret)
	{
		mm_free(utf16_buf);
		return -1;
	}

	mm_free(utf16_buf);

	*buflen = ret - 1;
	return 0;
#elif defined(__EV_LINUX__)
	ssize_t n = *buflen - 1;
	if (n > 0)
	{
		n = readlink("/proc/self/exe", buf, n);
	}

	if (-1 == n)
	{
		return -1;
	}

	buf[n]  = '\0';
	*buflen = n;

	return 0;
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

/**
 * @brief 16进制字符转换为数字
 * @param ch 16进制字符(0 ~ 9, A ~ F | a ~ f)
 * @return
 */
static inline uint8 ev_hex_string_to_num_internal(char ch)
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

#if defined(__EV_WINDOWS__)
/**
 * @brief Windows下字符编码转换
 * @param str       待转换编码的字符串
 * @param src_code  源字符编码代码页
 * @param dest_code 目标字符编码代码页
 * @return 成功返回对应转换的符串，失败返回空字符串
 */
static inline std::string ev_code_convert_internal(const char* str, uint32 src_code, uint32 dest_code)
{
	std::string ret_str;
	wchar_t*    temp_wstr = NULL;
	char*       temp_str  = NULL;
	int32       temp_len  = 0;

	if (NULL == str)
	{
		goto exit_handle;
	}

	temp_len  = MultiByteToWideChar(src_code, 0, str, -1, NULL, 0);
	temp_wstr = static_cast<wchar_t*>(mm_calloc(temp_len + 1, sizeof(wchar_t)));
	if (NULL == temp_wstr)
	{
		goto exit_handle;
	}

	MultiByteToWideChar(src_code, 0, str, -1, temp_wstr, temp_len);

	temp_len = WideCharToMultiByte(dest_code, 0, temp_wstr, -1, NULL, 0, NULL, NULL);
	temp_str = static_cast<char*>(mm_calloc(temp_len + 1, sizeof(char)));
	if (NULL == temp_str)
	{
		goto exit_handle;
	}

	WideCharToMultiByte(dest_code, 0, temp_wstr, -1, temp_str, temp_len, NULL, NULL);

	ret_str = temp_str;

exit_handle:
	if (temp_wstr)
	{
		mm_free(temp_wstr);
	}

	if (temp_str)
	{
		mm_free(temp_str);
	}

	return ret_str;
}
#elif defined(__EV_LINUX__)
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
static inline void ev_code_convert_internal(const char* src_charset, const char* dest_charset,
											const char* src_str, size_t src_strlen, char* buf, size_t buflen)
{
	char**  pi = NULL;
	char**  po = NULL;
	iconv_t cd = (iconv_t)-1;

	if (NULL == src_charset || NULL == dest_charset || NULL == src_str || NULL == buf)
	{
		goto exit_handle;
	}

	pi = (char**)&src_str;
	po = &buf;
	cd = iconv_open(dest_charset, src_charset);
	if ((iconv_t)-1 == cd)
	{
		goto exit_handle;
	}

	if ((size_t)-1 == iconv(cd, pi, &src_strlen, po, &buflen))
	{
		goto exit_handle;
	}

exit_handle:
	if (cd != (iconv_t)-1)
	{
		iconv_close(cd);
	}
}
#else
#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

/**
 * @brief 随机整数
 * @param
 * @return
 */
template<typename T>
static inline T ev_random_internal(void)
{
	T num = 0;
	evutil_secure_rng_get_bytes(&num, sizeof(num));
	return num;
}

} // namespace details

bool ev_set_console_output_color(output_color color)
{
#if defined(__EV_WINDOWS__)
	HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (NULL == outputHandle)
	{
		return false;
	}

	BOOL result = FALSE;
	switch (color)
	{
	case OC_GREEN:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case OC_WHITE:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case OC_YELLOW:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case OC_RED:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case OC_BLUE:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	default:
		result = SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	}

	return (TRUE == result);
#else
	return true;
#endif // defined(__EV_WINDOWS__)
}

bool ev_mkdir(const char* dirname, uint32 mode)
{
#if defined(__EV_WINDOWS__)
	return (0 == _mkdir(dirname));
#elif defined(__EV_LINUX__)
	return (0 == mkdir(dirname, static_cast<mode_t>(mode)));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_rmdir(const char* dirname)
{
#if defined(__EV_WINDOWS__)
	return (0 == _rmdir(dirname));
#elif defined(__EV_LINUX__)
	return (0 == rmdir(dirname));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_chdir(const char* path)
{
#if defined(__EV_WINDOWS__)
	return (0 == _chdir(path));
#elif defined(__EV_LINUX__)
	return (0 == chdir(path));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_access_exists(const char* path)
{
#if defined(__EV_WINDOWS__)
	return (0 == _access(path, 0));
#elif defined(__EV_LINUX__)
	return (0 == access(path, F_OK));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_access_read(const char* path)
{
#if defined(__EV_WINDOWS__)
	return (0 == _access(path, 4));
#elif defined(__EV_LINUX__)
	return (0 == access(path, R_OK));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_access_write(const char* path)
{
#if defined(__EV_WINDOWS__)
	return (0 == _access(path, 2));
#elif defined(__EV_LINUX__)
	return (0 == access(path, W_OK));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_get_exepath(char* buf, uint32* buflen)
{
	int32 ret = details::ev_get_exepath_internal(buf, buflen);
	return (0 == ret);
}

std::string ev_get_exepath(void)
{
	char   buf[2048];
	uint32 len = sizeof(buf);
	if (!ev_get_exepath(buf, &len))
	{
		return std::string();
	}

	return std::string(buf, len);
}

bool ev_get_exedir(char* buf, uint32* buflen)
{
	int32 ret = details::ev_get_exepath_internal(buf, buflen);
	if (-1 == ret)
	{
		return false;
	}

#if defined(__EV_WINDOWS__)
	const char* slash = strrchr(buf, '\\');
#else
	const char* slash = strrchr(buf, '/');
#endif // defined(__EV_WINDOWS__)
	if (NULL == slash && slash - buf < 0)
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

	uint32 pos = static_cast<uint32>(slash - buf);
	buf[pos]   = '\0';
	*buflen    = pos;

	return true;
}

std::string ev_get_exedir(void)
{
	char   buf[2048];
	uint32 len = sizeof(buf);
	if (!ev_get_exedir(buf, &len))
	{
		return std::string();
	}

	return std::string(buf, len);
}

bool ev_get_hostname(char* buf, uint32* buflen)
{
	if (NULL == buf || NULL == buflen || 0 == *buflen)
	{
		return false;
	}

	char tempbuf[1024] = {0};
	if (gethostname(tempbuf, sizeof(tempbuf)) != 0)
	{
		return false;
	}

	// 当buf的大小不足以存储hostname时，应该返回失败，而不是被截断的字符串
	uint32 tempbuf_len = static_cast<uint32>(strlen(tempbuf));
	if (tempbuf_len > *buflen - 1)
	{
		return false;
	}

	memcpy(buf, tempbuf, tempbuf_len);
	buf[tempbuf_len] = '\0';
	*buflen          = tempbuf_len;

	return true;
}

std::string ev_get_hostname(void)
{
	char   buf[1024];
	uint32 len = sizeof(buf);
	if (!ev_get_hostname(buf, &len))
	{
		return std::string();
	}

	return std::string(buf, len);
}

uint32 ev_get_free_memory(void)
{
#if defined(__EV_WINDOWS__)
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	BOOL ret = GlobalMemoryStatusEx(&memory_status);
	if (!ret)
	{
		return 0;
	}

	return static_cast<uint32>(memory_status.ullAvailPhys / 1024);
#elif defined(__EV_LINUX__)
	struct sysinfo info;

	int32 ret = sysinfo(&info);
	if (ret)
	{
		return 0;
	}

	return static_cast<uint32>(info.freeram * info.mem_unit / 1024);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

uint32 ev_get_total_memory(void)
{
#if defined(__EV_WINDOWS__)
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	BOOL ret = GlobalMemoryStatusEx(&memory_status);
	if (!ret)
	{
		return 0;
	}

	return static_cast<uint32>(memory_status.ullTotalPhys / 1024);
#elif defined(__EV_LINUX__)
	struct sysinfo info;

	int32 ret = sysinfo(&info);
	if (ret)
	{
		return 0;
	}

	return static_cast<uint32>(info.totalram * info.mem_unit / 1024);
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

uint32 ev_get_process_id(void)
{
#if defined(__EV_WINDOWS__)
	return static_cast<uint32>(GetCurrentProcessId());
#elif defined(__EV_LINUX__)
	return static_cast<uint32>(getpid());
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

int32 ev_snprintf(char* buf, size_t buflen, const char* format_str, ...)
{
	va_list ap;
	va_start(ap, format_str);

	int32 ret = ev_vsnprintf(buf, buflen, format_str, ap);

	va_end(ap);

	return ret;
}

int32 ev_vsnprintf(char* buf, size_t buflen, const char* format_str, va_list ap)
{
	if (NULL == buf || 0 == buflen || NULL == format_str)
	{
		return 0;
	}

	int32 ret = evutil_vsnprintf(buf, buflen, format_str, ap);
	return (ret >= 0 ? ret : -1);
}

int32 ev_strcasecmp(const char* str1, const char* str2)
{
	if (NULL == str1 || NULL == str2)
	{
		if (str1 && !str2)
		{
			return 1;
		}
		else if (!str1 && str2)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	return evutil_ascii_strcasecmp(str1, str2);
}

int32 ev_strncasecmp(const char* str1, const char* str2, size_t n)
{
	if (NULL == str1 || NULL == str2)
	{
		if (str1 && !str2)
		{
			return 1;
		}
		else if (!str1 && str2)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	return evutil_ascii_strncasecmp(str1, str2, n);
}

bool ev_memory_to_hex_string(const void* mem, size_t memlen, char* outbuf, size_t outbuf_len, bool uppercase)
{
	static const char uppercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	static const char lowercase_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	if (NULL == mem || 0 == memlen || NULL == outbuf || 0 == outbuf_len || memlen * 2 + 1 > outbuf_len)
	{
		return false;
	}

	const char*  hex_table = (uppercase ? uppercase_hex_table : lowercase_hex_table);
	const uint8* temp_mem  = reinterpret_cast<const uint8*>(mem);
	size_t       len       = 0;

	for (size_t idx = 0; idx != memlen; ++idx)
	{
		outbuf[len++] = hex_table[(temp_mem[idx]  & 0xF0) >> 4];
		outbuf[len++] = hex_table[(temp_mem[idx]) & 0x0F];
	}

	outbuf[len] = '\0';

	return true;
}

bool ev_hex_string_to_memory(const char* hex_string, void* outbuf, size_t outbuf_len)
{
	if (NULL == hex_string || NULL == outbuf || 0 == outbuf_len)
	{
		return false;
	}

	size_t hex_string_len = strlen(hex_string);
	if (hex_string_len % 2 != 0 || hex_string_len / 2 > outbuf_len)
	{
		return false;
	}

	uint8  temp_high;
	uint8  temp_low;
	uint8* temp_outbuf = reinterpret_cast<uint8*>(outbuf);
	for (size_t idx = 0; idx != hex_string_len; idx += 2)
	{
		temp_high = details::ev_hex_string_to_num_internal(hex_string[idx]);
		temp_low  = details::ev_hex_string_to_num_internal(hex_string[idx + 1]);
		if (0xFF == temp_high || 0xFF == temp_low)
		{
			return false;
		}

		temp_outbuf[idx / 2] = ((temp_high << 4) | temp_low);
	}

	return true;
}

void ev_split_string(const char* src_str, const char* separator, std::vector<std::string>& out_result)
{
	const char* temp_str    = src_str;
	const char* temp_sep    = NULL;
	int32       start_pos   = 0;
	int32       cur_str_pos = 0;
	bool        flag        = false;

	if (NULL == src_str || NULL == separator)
	{
		return;
	}

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
			{
				out_result.push_back(std::string(src_str + start_pos, cur_str_pos - start_pos));
			}

			start_pos = cur_str_pos + 1;
		}

		++cur_str_pos;
		++temp_str;
	}

	// 处理最后一个分隔符后面的字符串
	if (start_pos < cur_str_pos)
	{
		out_result.push_back(std::string(src_str + start_pos, cur_str_pos - start_pos));
	}
}

bool ev_isalpha(char ch)
{
	return (1 == EVUTIL_ISALPHA_(ch));
}

bool ev_isdigit(char ch)
{
	return (1 == EVUTIL_ISDIGIT_(ch));
}

bool ev_isalnum(char ch)
{
	return (1 == EVUTIL_ISALNUM_(ch));
}

bool ev_isspace(char ch)
{
	return (1 == EVUTIL_ISSPACE_(ch));
}

bool ev_isprint(char ch)
{
	return (1 == EVUTIL_ISPRINT_(ch));
}

bool ev_islower(char ch)
{
	return (1 == EVUTIL_ISLOWER_(ch));
}

bool ev_isupper(char ch)
{
	return (1 == EVUTIL_ISUPPER_(ch));
}

char ev_tolower(char ch)
{
	return EVUTIL_TOLOWER_(ch);
}

char ev_toupper(char ch)
{
	return EVUTIL_TOUPPER_(ch);
}

int32 ev_strtoint32(const char* str)
{
	if (NULL == str)
	{
		return 0;
	}

	return static_cast<int32>(atoi(str));
}

int64 ev_strtoint64(const char* str)
{
	if (NULL == str)
	{
		return 0;
	}

	return static_cast<int64>(evutil_strtoll(str, NULL, 0));
}

float ev_strtof(const char* str)
{
	if (NULL == str)
	{
		return 0.f;
	}

	return static_cast<float>(atof(str));
}

double ev_strtod(const char* str)
{
	if (NULL == str)
	{
		return 0.0;
	}

	return atof(str);
}

bool ev_is_gbk(const std::string& str)
{
	return ev_is_gbk(str.c_str(), static_cast<size_t>(str.size()));
}

bool ev_is_gbk(const char* str, size_t len)
{
	if (NULL == str || 0 == len)
	{
		return false;
	}

	const uint8* data = reinterpret_cast<const uint8*>(str);
	size_t       idx  = 0;
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

bool ev_is_utf8(const std::string& str)
{
	return ev_is_utf8(str.c_str(), static_cast<size_t>(str.size()));
}

bool ev_is_utf8(const char* str, size_t len)
{
	if (NULL == str || 0 == len)
	{
		return false;
	}

	const uint8* data      = reinterpret_cast<const uint8*>(str);
	size_t       check_sub = 0;
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
			{
				return false;
			}

			--check_sub;
		}
	}

	// UTF-8字符串不完整，直接否认吧，毕竟字符串不完整出现问题是迟早的事情
	return (check_sub > 0 ? false : true);
}

std::string ev_gbk_to_utf8(const char* gbk_str)
{
#if defined(__EV_WINDOWS__)
	return details::ev_code_convert_internal(gbk_str, CP_ACP, CP_UTF8);
#elif defined(__EV_LINUX__)
	std::string ret_str;
	char*       temp_str    = NULL;
	size_t      temp_strlen = 0;
	size_t      gbk_strlen  = 0;

	if (NULL == gbk_str)
	{
		goto exit_handle;
	}

	gbk_strlen  = strlen(gbk_str);
	temp_strlen = gbk_strlen * 3;
	temp_str    = static_cast<char*>(mm_calloc(temp_strlen + 1, sizeof(char)));
	if (NULL == temp_str)
	{
		goto exit_handle;
	}

	details::ev_code_convert_internal("GBK", "UTF-8", gbk_str, gbk_strlen, temp_str, temp_strlen);

	ret_str = temp_str;

exit_handle:
	if (temp_str)
	{
		mm_free(temp_str);
	}

	return ret_str;
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

std::string ev_utf8_to_gbk(const char* utf8_str)
{
#if defined(__EV_WINDOWS__)
	return details::ev_code_convert_internal(utf8_str, CP_UTF8, CP_ACP);
#elif defined(__EV_LINUX__)
	std::string ret_str;
	char*       temp_str    = NULL;
	size_t      temp_strlen = 0;
	size_t      utf8_strlen = 0;

	if (NULL == utf8_str)
	{
		goto exit_handle;
	}

	utf8_strlen = strlen(utf8_str);
	temp_strlen = utf8_strlen;
	temp_str    = static_cast<char*>(mm_calloc(temp_strlen + 1, sizeof(char)));
	if (NULL == temp_str)
	{
		goto exit_handle;
	}

	details::ev_code_convert_internal("UTF-8", "GBK", utf8_str, utf8_strlen, temp_str, temp_strlen);

	ret_str = temp_str;

exit_handle:
	if (temp_str)
	{
		mm_free(temp_str);
	}

	return ret_str;
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

uint8 ev_random_uint8(void)
{
	return details::ev_random_internal<uint8>();
}

uint16 ev_random_uint16(void)
{
	return details::ev_random_internal<uint16>();
}

uint32 ev_random_uint32(void)
{
	return details::ev_random_internal<uint32>();
}

uint64 ev_random_uint64(void)
{
	return details::ev_random_internal<uint64>();
}

int32 ev_random_range(int32 upper_bound)
{
	// 确保 0 < upper_bound <= 0x7FFFFFFF
	upper_bound = (upper_bound <= 0 ? 1 : upper_bound);

	int32 random_val = static_cast<int32>(ev_random_float() * static_cast<float>(upper_bound));
	random_val       = (random_val == upper_bound ? random_val - 1 : random_val);
	return random_val;
}

float ev_random_float(void)
{
	float random_val = static_cast<float>(ev_random_uint16());
	float uint16_max = static_cast<float>(UINT16_MAX);
	return (random_val / uint16_max);
}

double ev_random_double(void)
{
	double random_val = static_cast<double>(ev_random_uint32());
	double uint32_max = static_cast<double>(UINT32_MAX);
	return (random_val / uint32_max);
/*
	union
	{
		double d;
		uint64 u;
	}x;

#if defined(__EV_WINDOWS__)
	uint64 fixed_num = 0x3FF0000000000001ULL;
#else
	#if defined(__EV_WORDSIZE32__)
		uint64 fixed_num = 0x3FF0000000000001ULL;
	#else
		uint64 fixed_num = 0x3FF0000000000001UL;
	#endif // defined(__EV_WORDSIZE32__)
#endif // defined(__EV_WINDOWS__)

	x.u = (ev_random_uint64() >> 12) | fixed_num;
	return (x.d - 1.0);
*/
}

} // namespace evpp