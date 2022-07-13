#include "base/ev_async_logging.h"
#include "base/ev_common.h"

#include <cstdio>
#include <cstring>
#include <cassert>

namespace evpp    {
namespace details {

// 时间缓存
static ev_thread_local char   g_time_cache[64]    = {0};
// 线程Id字符串缓存
static ev_thread_local char   g_thread_id_str[32] = {0};
// 线程Id字符串长度缓存
static ev_thread_local uint32 g_thread_id_strlen  = 0;

/**
 * @brief 设置控制台彩色字体输出
 * @param level 日志记录级别，不同的级别输出不同的颜色
 *            LL_TRACE:绿色
 *            LL_INFO :白色
 *            LL_WARN :黄色
 *            LL_ERROR:红色
 *            其它未知:蓝色
 * @return 成功返回true，失败返回false
 */
static inline bool ev_set_output_color_internal(ev_async_logging::log_level level)
{
	switch (level)
	{
	case ev_async_logging::LL_TRACE:
		return ev_set_console_output_color(OC_GREEN);
	case ev_async_logging::LL_INFO:
		return ev_set_console_output_color(OC_WHITE);
	case ev_async_logging::LL_WARN:
		return ev_set_console_output_color(OC_YELLOW);
	case ev_async_logging::LL_ERROR:
		return ev_set_console_output_color(OC_RED);
	default:
		return ev_set_console_output_color(OC_BLUE);
	}
}

/**
 * @brief 程序名合法性检测
 * @param program_name 程序名(合法范围：['a', 'z'], ['A', 'Z'], ['0', '9'], '-'，'_')
 * @return 成功返回true，失败返回false
 */
bool ev_program_name_check_internal(const char* program_name)
{
	if (NULL == program_name || 0 == strlen(program_name))
	{
		return false;
	}

	while (*program_name)
	{
		if (!ev_isalnum(*program_name) && *program_name != '-' && *program_name != '_')
		{
			return false;
		}

		++program_name;
	}

	return true;
}

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_async_logging
//////////////////////////////////////////////////////////////////////////
bool ev_async_logging::start(const char* program_name, const std::string& thread_name)
{
	ev_time cur_time;

	if (running_)
	{
		return true;
	}

	assert(!cur_buffer_ && !next_buffer_ && !cur_log_file_ && buffers_.empty() && "Libevpp fatal error!");

	// 程序名、线程名合法性检测
	if (!details::ev_program_name_check_internal(program_name) || thread_name.empty())
	{
		goto error_handle;
	}

	program_name_ = program_name;

	// 获取当前执行文件所在的目录
	log_dir_ = ev_get_exedir();
	if (log_dir_.empty())
	{
		goto error_handle;
	}

	// 拼凑日志目录
#if defined(__EV_WINDOWS__)
	log_dir_ += "\\log_files\\";
#else
	log_dir_ += "/log_files/";
#endif // defined(__EV_WINDOWS__)

	// 初始化日志文件
	if (!init_log_file(cur_time))
	{
		goto error_handle;
	}

	// 创建两块日志缓冲区
	cur_buffer_  = ev_fixed_buffer::create(g_logconfig_max_fixed_buffer_size);
	next_buffer_ = ev_fixed_buffer::create(g_logconfig_max_fixed_buffer_size);
	if (!cur_buffer_ || !next_buffer_)
	{
		goto error_handle;
	}

	// 创建日志线程
	running_ = logging_thread_.start_thread(&ev_async_logging::logging_thread_entry, this, thread_name);
	if (!running_)
	{
		goto error_handle;
	}

	// 等待日志线程启动成功
	latch_.wait();

	return true;

error_handle:
	cur_log_file_.reset();
	cur_buffer_.reset();
	next_buffer_.reset();
	buffers_.clear();
	log_dir_.clear();
	program_name_.clear();

	return false;
}

void ev_async_logging::stop(void)
{
	if (!running_)
	{
		return;
	}
	else
	{
		running_ = false;
	}

	// 通知日志线程，要退出了
	not_empty_.cond_signal();

	// 等待日志线程退出
	logging_thread_.join_thread();

	// 忽略返回值，并确保latch_永远有效
	latch_.reset_count(1);

	assert(cur_buffer_ && 1 == cur_buffer_.use_count() && 0 == cur_buffer_->size() && "Libevpp fatal error!");
	assert(next_buffer_ && 1 == next_buffer_.use_count() && 0 == next_buffer_->size() && "Libevpp fatal error!");
	assert(cur_log_file_ && buffers_.empty() && "Libevpp fatal error!");

	cur_log_file_.reset();
	cur_buffer_.reset();
	next_buffer_.reset();
	buffers_.clear();
	log_dir_.clear();
	program_name_.clear();
}

void ev_async_logging::set_output_to_screen(bool flag)
{
	ev_lock_guard lock(lock_);
	output_to_screen_ = flag;
}

void ev_async_logging::writef(log_level level, const char* format_str, ...)
{
	char   temp_buffer[g_logconfig_log_length_limit + 16];
	uint32 write_size;

	// 格式化日志信息
	{
		int32   ret;
		va_list ap;
		va_start(ap, format_str);
			write_size = 0;
			ret        = ev_vsnprintf(temp_buffer, sizeof(temp_buffer), format_str, ap);
		va_end(ap);

		if (ret <= 0)
		{
			return;
		}

		write_size += static_cast<uint32>(ret);
		write_size  = (write_size >= g_logconfig_log_length_limit ? g_logconfig_log_length_limit : write_size);
	}

	return write(level, temp_buffer, write_size);
}

void ev_async_logging::write(log_level level, const char* msg)
{
	return write(level, msg, static_cast<uint32>(strlen(msg)));
}

void ev_async_logging::write(log_level level, const char* msg, uint32 msglen)
{
	assert(running_ && "Libevpp fatal error! ev_async_logging is not running!");
	assert(msg != NULL && msglen > 0 && msglen <= g_logconfig_log_length_limit && "Libevpp fatal error! Invalid parameters!");
	if (NULL == msg || 0 == msglen || msglen > g_logconfig_log_length_limit || !running_)
	{
		return;
	}

	// 将日志头写入到内部缓冲区
	char   header_buffer[128] = {0};
	uint32 header_size        = prepare_header_info(level, header_buffer, sizeof(header_buffer));

	// 写入内容到日志缓冲区并同步到标准输出
	{
		ev_lock_guard lock(lock_);
		uint32        write_size;

		write_size = write_to_buffer(header_buffer, header_size);
		assert(header_size == write_size && "Libevpp fatal error!");

		write_size = write_to_buffer(msg, msglen);
		assert(msglen == write_size && "Libevpp fatal error!");

		write_size = write_to_buffer("\n", 1);
		assert(1 == write_size && "Libevpp fatal error!");

		if (output_to_screen_)
		{
#if defined(__EV_WINDOWS__)
			details::ev_set_output_color_internal(level);
#endif // defined(__EV_WINDOWS__)
			fprintf(stdout, "%s%s\n", header_buffer, msg);
#if defined(__EV_WINDOWS__)
			details::ev_set_output_color_internal(LL_INFO);
#endif // defined(__EV_WINDOWS__)
		}

		EV_UNUSED_VARIABLE(write_size);
	}
}

ev_async_logging::ev_async_logging(void)
	: running_(false)
	, output_to_screen_(false)
	, notified_back_thread_(false)
	, log_dir_()
	, program_name_()
	, lock_()
	, cur_log_file_()
	, not_empty_(lock_)
	, latch_(1)
	, logging_thread_()
	, cur_buffer_()
	, next_buffer_()
	, buffers_()
{
}

ev_async_logging::~ev_async_logging(void)
{
	stop();
}

bool ev_async_logging::init_log_file(const ev_time& cur_time)
{
	// 如果日志目录不存在，则先创建
	if (!ev_access_exists(log_dir_.c_str()) && !ev_mkdir(log_dir_.c_str()))
	{
		return false;
	}

	// 格式化文件名，格式为：log_dir/program_name-pid-20080808-080808.log
	char log_filename[2048];
	{
		int32 ret = ev_snprintf(log_filename, sizeof(log_filename), "%s%s-%u-%04d%02d%02d-%02d%02d%02d.log",
			log_dir_.c_str(), program_name_.c_str(), ev_get_process_id(), cur_time.year(), cur_time.month(), cur_time.month_day(),
			cur_time.hour(), cur_time.minute(), cur_time.second()
		);

		if (ret <= 0)
		{
			return false;
		}
	}

	// 创建log文件
	file_ptr new_file = ev_file::fopen(log_filename, file_utils::FM_APPEND_ONLY);
	if (!new_file)
	{
		return false;
	}

	// 关闭旧文件
	if (cur_log_file_)
	{
		cur_log_file_.reset();
		assert(!cur_log_file_ && 0 == cur_log_file_.use_count() && "Libevpp fatal error!");
	}

	cur_log_file_ = new_file;
	new_file.reset();
	assert(cur_log_file_ && 1 == cur_log_file_.use_count() && 0 == new_file.use_count() && "Libevpp fatal error!");

	return true;
}

uint32 ev_async_logging::prepare_header_info(log_level level, char* buf, uint32 buflen)
{
	// 日志级别
	const char* level_str    = NULL;
	uint32      level_strlen = 0;
	uint32      write_size   = 0;
	switch (level)
	{
	case LL_TRACE:
		level_str    = "[TRACE]:";
		level_strlen = 8;
		break;
	case LL_INFO:
		level_str    = "[INFO]:";
		level_strlen = 7;
		break;
	case LL_WARN:
		level_str    = "[WARN]:";
		level_strlen = 7;
		break;
	case LL_ERROR:
		level_str    = "[ERROR]:";
		level_strlen = 8;
		break;
	default:
		level_str    = "[UNKNOWN]:";
		level_strlen = 10;
		break;
	}

	// PS:为性能优化设计，当每秒钟写入几十万乃至上百万条日志的时候，时间字符串是基本不变的
	ev_time::time_cache* tc = reinterpret_cast<ev_time::time_cache*>(&details::g_time_cache);
	if (!ev_time::update_time_cache(tc))
	{
		return 0;
	}

	// PS:为性能优化设计，线程Id的字符串放在线程变量中，自始至终只需要初始化一次
	if ('\0' == details::g_thread_id_str[0])
	{
		int32 ret = ev_snprintf(details::g_thread_id_str, sizeof(details::g_thread_id_str), "[TID:%u]", this_thread::thread_id());
		if (ret <= 0)
		{
			return 0;
		}

		details::g_thread_id_strlen = static_cast<uint32>(ret);
	}

	// 采用格式为：年-月-日 时:分:秒.微秒[TID:线程Id][日志级别]:
	uint32 format_time_len = 26;  /* 格式化时间字符串固定占用26字节 */
	uint32 log_head_size   = format_time_len + details::g_thread_id_strlen + level_strlen;
	if (log_head_size > buflen)
	{
		return 0;
	}

	memcpy(buf, tc->cache_format_time, format_time_len);
	buf        += format_time_len;
	write_size += format_time_len;

	memcpy(buf, details::g_thread_id_str, details::g_thread_id_strlen);
	buf        += details::g_thread_id_strlen;
	write_size += details::g_thread_id_strlen;

	memcpy(buf, level_str, level_strlen);
	buf        += level_strlen;
	write_size += level_strlen;

	return write_size;
}

uint32 ev_async_logging::write_to_buffer(const char* buf, uint32 write_size)
{
	// 可以一次性写入
	if (cur_buffer_ && cur_buffer_->writable_size() >= write_size)
	{
		return cur_buffer_->write(buf, write_size);
	}

	// 当前缓冲区不足以写入本次日志内容，将其放入buffers_中，等待后台线程写入日志文件
	if (cur_buffer_)
	{
		buffers_.push_back(cur_buffer_);
		cur_buffer_.reset();
		assert(0 == cur_buffer_.use_count() && 1 == buffers_.back().use_count() && "Libevpp fatal error!");
	}

	// 判断预备的下一块缓冲区是否可用，如果不可用，就新建一块缓冲区
	if (next_buffer_)
	{
		cur_buffer_ = next_buffer_;
		next_buffer_.reset();
		assert(1 == cur_buffer_.use_count() && 0 == next_buffer_.use_count() && "Libevpp fatal error!");
	}
	else
	{
		fixed_buffer_ptr newbuf = ev_fixed_buffer::create(g_logconfig_max_fixed_buffer_size);
		if (newbuf)
		{
			cur_buffer_ = newbuf;
			newbuf.reset();
			assert(1 == cur_buffer_.use_count() && 0 == newbuf.use_count() && "Libevpp fatal error!");
		}
		// TODO
		//else
		//{
		//	assert(false && "Libevpp fatal error! ev_fixed_buffer::create() failed, memory not enough!");
		//}
	}

	// 可以一次性写入
	uint32 ret = 0;
	if (cur_buffer_ && cur_buffer_->writable_size() >= write_size)
	{
		ret = cur_buffer_->write(buf, write_size);
	}

	// 当缓冲区数组中有待写入的缓冲区时，就通知后台线程
	if (!buffers_.empty() && !notified_back_thread_)
	{
		notified_back_thread_ = true;
		not_empty_.cond_signal();
	}

	return ret;
}

void ev_async_logging::logging_thread_entry(void* arg)
{
	ev_async_logging& al = *reinterpret_cast<ev_async_logging*>(arg);

	// 准备两块备胎日志缓冲区
	fixed_buffer_ptr prepare_cur_buffer  = ev_fixed_buffer::create(g_logconfig_max_fixed_buffer_size);
	fixed_buffer_ptr prepare_next_buffer = ev_fixed_buffer::create(g_logconfig_max_fixed_buffer_size);
	
	// 待写入文件的缓冲区列表
	buffer_list buffers_to_write;

	ev_time cur_time;
	int32 cur_date  = ev_time::convert_date_to_number(cur_time);
	int32 temp_date = 0;

	// 资源申请失败，直接返回以结束日志线程，此时主线程会阻塞在start()调用处，方便查问题
	if (!prepare_cur_buffer || !prepare_next_buffer || cur_date <= 0)
	{
		return;
	}

	// 通知主线程日志线程已经启动
	al.latch_.countdown();

	bool loop_one_more_time = true;
	while (true)
	{
		assert(prepare_cur_buffer && 1 == prepare_cur_buffer.use_count() && 0 == prepare_cur_buffer->size() && "Libevpp fatal error!");
		assert(prepare_next_buffer && 1 == prepare_next_buffer.use_count() && 0 == prepare_next_buffer->size() && "Libevpp fatal error!");
		assert(buffers_to_write.empty() && "Libevpp fatal error!");

		{
			ev_lock_guard lock(al.lock_);

			// 如果需要写入文件的缓冲区数组为空，就等待3秒
			if (al.buffers_.empty() && al.running_)
			{
				al.not_empty_.cond_timedwait(3000);
			}

			// 重置是否通知过后台线程的标记
			al.notified_back_thread_ = false;

			// 如果缓冲区数组为空，并且当前缓冲区内写入的有日志信息
			// 那么就表示当前日志写入量很少，压力很小，基于此要保证每3秒钟就将缓存写入文件一次
			if (al.buffers_.empty() && al.cur_buffer_ && al.cur_buffer_->size() > 0)
			{
				al.buffers_.push_back(al.cur_buffer_);
				al.cur_buffer_.reset();
				assert(0 == al.cur_buffer_.use_count() && 1 == al.buffers_.back().use_count() && "Libevpp fatal error!");
			}

			// 将需要写入文件的缓冲区列表取过来
			if (!al.buffers_.empty())
			{
				buffers_to_write.swap(al.buffers_);
				assert(!buffers_to_write.empty() && al.buffers_.empty() && "Libevpp fatal error!");
			}

			// 替换两块备胎缓冲区
			if (!al.cur_buffer_)
			{
				al.cur_buffer_ = prepare_cur_buffer;
				prepare_cur_buffer.reset();
				assert(0 == prepare_cur_buffer.use_count() && 1 == al.cur_buffer_.use_count() && "Libevpp fatal error!");
			}

			if (!al.next_buffer_)
			{
				al.next_buffer_ = prepare_next_buffer;
				prepare_next_buffer.reset();
				assert(0 == prepare_next_buffer.use_count() && 1 == al.next_buffer_.use_count() && "Libevpp fatal error!");
			}

			// 重置缓冲区
			al.cur_buffer_->reset();
			al.next_buffer_->reset();
		}

		// 遍历当前待写入文件的缓冲区数组，依次写入文件
		for (buffer_list::iterator iter = buffers_to_write.begin(); iter != buffers_to_write.end(); ++iter)
		{
			fixed_buffer_ptr& flushbuf = *iter;
			assert(1 == flushbuf.use_count() && "Libevpp fatal error!");

			// 每次写入都强制更新时间
			cur_time = ev_time();

			// 将日志缓冲区写入文件，如果日志文件达到最大写入字节，或者日期改变，则滚动日志
			temp_date = ev_time::convert_date_to_number(cur_time);
			if (al.cur_log_file_->fsize() >= g_logconfig_max_log_file_size || (temp_date > 0 && temp_date != cur_date))
			{
				if (al.init_log_file(cur_time))
				{
					cur_date = temp_date;
				}
				// TODO
				//else
				//{
				//	assert(false && "Libevpp fatal error! init_log_file() failed in logging thread!");
				//}
			}

			uint32 write_file_size = al.cur_log_file_->fwrite_unlocked(flushbuf->data(), flushbuf->size());
			assert(write_file_size == flushbuf->size() && "Libevpp fatal error! fwrite_unlocked() failed in logging thread!");
			EV_UNUSED_VARIABLE(write_file_size);
		}

		// flush日志文件
		al.cur_log_file_->fflush_unlocked();

		// 保证两块备胎日志缓冲区总是有效
		if (!prepare_cur_buffer)
		{
			assert(!buffers_to_write.empty() && "Libevpp fatal error!");
			prepare_cur_buffer = buffers_to_write.back();

			buffers_to_write.pop_back();
			assert(1 == prepare_cur_buffer.use_count() && "Libevpp fatal error!");
		}

		if (!prepare_next_buffer)
		{
			assert(!buffers_to_write.empty() && "Libevpp fatal error!");
			prepare_next_buffer = buffers_to_write.back();

			buffers_to_write.pop_back();
			assert(1 == prepare_next_buffer.use_count() && "Libevpp fatal error!");
		}

		// 重置缓冲区
		prepare_cur_buffer->reset();
		prepare_next_buffer->reset();

		// 丢弃多余的缓冲区，避免过多的占用内存
		buffers_to_write.clear();

		// 避免在停止日志系统时，有当前的缓冲没有被刷入日志文件，所以再多循环一次，用于处理此情况
		if (al.running_)
		{
			continue;
		}
		else if (false == al.running_ && true == loop_one_more_time)
		{
			loop_one_more_time = false;
			continue;
		}
		else
		{
			break;
		}
	}

	assert(prepare_cur_buffer && 1 == prepare_cur_buffer.use_count() && 0 == prepare_cur_buffer->size() && "Libevpp fatal error!");
	assert(prepare_next_buffer && 1 == prepare_next_buffer.use_count() && 0 == prepare_next_buffer->size() && "Libevpp fatal error!");
	assert(buffers_to_write.empty() && "Libevpp fatal error!");

	assert(al.cur_buffer_ && 1 == al.cur_buffer_.use_count() && 0 == al.cur_buffer_->size() && "Libevpp fatal error!");
	assert(al.next_buffer_ && 1 == al.next_buffer_.use_count() && 0 == al.next_buffer_->size() && "Libevpp fatal error!");
	assert(al.buffers_.empty() && "Libevpp fatal error!");

	assert(false == loop_one_more_time && false == al.running_ && "Libevpp fatal error!");
}

} // namespace evpp