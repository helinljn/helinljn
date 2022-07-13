#include "base/ev_assert.h"
#include "base/ev_thread.h"
#include "base/ev_common.h"
#include "base/ev_lock.h"
#include "base/ev_file.h"
#include "base/ev_time.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace evpp    {
namespace details {

extern bool ev_program_name_check_internal(const char* program_name);

static const uint32         g_internal_buffer_size     = 2048;
static ev_thread_local char g_assertion_time_cache[64] = {0};
static bool                 g_redirect_stderr_called   = false;
static file_ptr             g_assertion_file;
static ev_lock              g_assert_lock;

int ev_assert_internal(const char* msg, const char* file, const char* func, int line)
{
	// 格式化断言信息
	char   temp_buffer[g_internal_buffer_size + 16];
	uint32 write_size;
	{
		// 去除文件名中的路径信息
#if defined(__EV_WINDOWS__)
		const char* slash = strrchr(file, '\\');
#else
		const char* slash = strrchr(file, '/');
#endif // defined(__EV_WINDOWS__)
		if (slash)
		{
			file = slash + 1;
		}

		// 获取年-月-日 时:分:秒.微秒
		ev_time::time_cache* tc = reinterpret_cast<ev_time::time_cache*>(&g_assertion_time_cache);
		if (!ev_time::update_time_cache(tc))
		{
			return 1;
		}

		int32 ret = ev_snprintf(temp_buffer, sizeof(temp_buffer),
			"----------- [ASSERTION FAILED BEGIN] -----------\n"
			"File        : %s\n"
			"Func        : %s\n"
			"Line        : %d\n"
			"Time        : %s\n"
			"Reason      : %s\n"
			"Thread id   : %u\n"
			"Thread name : %s\n"
			"----------- [ ASSERTION FAILED END ] -----------\n\n",
			file, func, line, tc->cache_format_time, msg, this_thread::thread_id(), this_thread::thread_name()
		);

		if (ret <= 0)
		{
			return 1;
		}

		write_size = static_cast<uint32>(ret);
		write_size = (write_size >= g_internal_buffer_size ? g_internal_buffer_size : write_size);
	}
	
	// 写入断言信息
	{
		ev_lock_guard lock(g_assert_lock);

		if (g_redirect_stderr_called && g_assertion_file)
		{
			g_assertion_file->fwrite_unlocked(temp_buffer, write_size);

#if defined(__EV_WINDOWS__)
			ev_set_console_output_color(OC_RED);

			fprintf(stdout, "%s", temp_buffer);

			ev_set_console_output_color(OC_WHITE);
#endif // defined(__EV_WINDOWS__)
		}
		else
		{
#if defined(__EV_WINDOWS__)
			ev_set_console_output_color(OC_RED);
#endif // defined(__EV_WINDOWS__)

			fprintf(stdout, "%s", temp_buffer);

#if defined(__EV_WINDOWS__)
			ev_set_console_output_color(OC_WHITE);
#endif // defined(__EV_WINDOWS__)
		}
	}

#if defined(__EV_WINDOWS__) && !defined(NDEBUG)
	::MessageBoxA(NULL, temp_buffer, "!!! Assertion Failed !!!", MB_ABORTRETRYIGNORE);
#endif // defined(__EV_WINDOWS__) && !defined(NDEBUG)

#if !defined(NDEBUG)
	abort();
#endif // !defined(NDEBUG)

	return 1;
}

} // namespace details

bool ev_redirect_assertion(const char* program_name)
{
	if (details::g_redirect_stderr_called || details::g_assertion_file)
	{
		return true;
	}

	// 程序名合法性检测
	if (!details::ev_program_name_check_internal(program_name))
	{
		return false;
	}

	// 获取当前执行文件所在的目录
	std::string exedir = ev_get_exedir();
	if (exedir.empty())
	{
		return false;
	}

	// 拼凑日志目录
#if defined(__EV_WINDOWS__)
	exedir += "\\log_files\\";
#else
	exedir += "/log_files/";
#endif // defined(__EV_WINDOWS__)

	// 如果日志目录不存在，则先创建
	if (!ev_access_exists(exedir.c_str()) && !ev_mkdir(exedir.c_str()))
	{
		return false;
	}

	// 格式化文件名，格式为：log_dir/program_name-pid-20080808-080808-assertion-failed.log
	char log_filename[2048];
	{
		ev_time cur_time;
		int32   ret = ev_snprintf(log_filename, sizeof(log_filename), "%s%s-%u-%04d%02d%02d-%02d%02d%02d-assertion-failed.log",
			exedir.c_str(), program_name, ev_get_process_id(), cur_time.year(), cur_time.month(), cur_time.month_day(),
			cur_time.hour(), cur_time.minute(), cur_time.second()
		);

		if (ret <= 0)
		{
			return false;
		}
	}

	details::g_assertion_file = ev_file::fopen(log_filename, file_utils::FM_APPEND_ONLY, 0);
	if (!details::g_assertion_file)
	{
		return false;
	}

	details::g_redirect_stderr_called = true;

	return true;
}

} // namespace evpp