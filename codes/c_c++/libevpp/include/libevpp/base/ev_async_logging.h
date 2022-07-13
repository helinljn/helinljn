//////////////////////////////////////////////////////////////////////////
// 异步日志
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_ASYNC_LOGGING_H__
#define __EV_ASYNC_LOGGING_H__

#include "base/ev_types.h"
#include "base/ev_singleton.h"
#include "base/ev_thread.h"
#include "base/ev_lock.h"
#include "base/ev_time.h"
#include "base/ev_file.h"
#include "base/ev_condition.h"
#include "base/ev_countdown_latch.h"
#include "base/ev_fixed_buffer.h"

#include <list>
#include <string>
#include <cstdarg>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// 日志配置参数
//////////////////////////////////////////////////////////////////////////
// 单条日志长度上限
const uint32 g_logconfig_log_length_limit      = 16 * 1024 /* 16KB */;

// 日志缓冲区的大小
const uint32 g_logconfig_max_fixed_buffer_size = 16 * 1024 * 1024 /* 16MB */;

// 单个日志文件写入的最大字节，达到该写入大小会自动滚动，新建一个日志文件并继续写入
const uint32 g_logconfig_max_log_file_size     = 1024 * 1024 * 1024 /* 1GB */;

//////////////////////////////////////////////////////////////////////////
// ev_async_logging
//////////////////////////////////////////////////////////////////////////
class ev_async_logging : public ev_singleton<ev_async_logging>
{
public:
	// 日志级别
	enum log_level
	{
		LL_TRACE = 0,  // 追踪日志
		LL_INFO  = 1,  // 打印日志
		LL_WARN  = 2,  // 警告日志
		LL_ERROR = 3,  // 错误日志
	};

	// 缓冲区列表
	typedef std::list<fixed_buffer_ptr> buffer_list;

public:
	/**
	 * @brief 启动日志
	 *        1. 必须在输出第一条日志之前调用该接口，建议在程序一开始就调用该方法
	 *        2. 该接口会导致调用线程阻塞，直至日志线程启动成功
	 *        
	 *        该函数会在执行文件所在的目录下创建一个log_files目录
	 *        然后在该目录下创建固定格式的文件，格式如下：
	 *        program_name-pid-20080808-080808.log
	 * @param program_name 程序名(合法范围：['a', 'z'], ['A', 'Z'], ['0', '9'], '-'，'_')
	 * @param thread_name  日志线程名字，默认为：logging thread
	 * @return 成功返回true，失败返回false
	 */
	bool start(const char* program_name, const std::string& thread_name = "logging thread");

	/**
	 * @brief 停止日志
	 *        1. 程序结束前调用，也可以不调用，此时析构函数会自动调用
	 *        2. 该接口会导致调用线程阻塞，直至日志线程结束
	 * @param
	 * @return
	 */
	void stop(void);

	/**
	 * @brief 设置是否将日志内容输出至屏幕，默认为：不输出
	 *        (PS:Windows下输出会带颜色)
	 *            LL_TRACE:绿色
	 *            LL_INFO :白色
	 *            LL_WARN :黄色
	 *            LL_ERROR:红色
	 *            其它未知:蓝色
	 * @param flag true表示将日志内容输出至屏幕，false表示不输出
	 * @return
	 */
	void set_output_to_screen(bool flag);

	/**
	 * @brief 以格式化形式追加日志信息
	 *        自动在头部【添加日志头】，采用格式为：【年-月-日 时:分:秒.微秒[TID:线程Id][日志级别]:】
	 *        自动在尾部【追加'\n'】
	 * @param level      该条日志的级别，取值范围参照log_level
	 * @param format_str 格式化字符串
	 * @return
	 */
	void writef(log_level level, const char* format_str, ...);

	/**
	 * @brief 以字符串形式追加日志信息
	 *        自动在头部【添加日志头】，采用格式为：【年-月-日 时:分:秒.微秒[TID:线程Id][日志级别]:】
	 *        自动在尾部【追加'\n'】
	 * @param level  该条日志的级别，取值范围参照log_level
	 * @param msg    以'\0'结尾的字符串
	 * @param msglen 字符串长度
	 * @return
	 */
	void write(log_level level, const char* msg);
	void write(log_level level, const char* msg, uint32 msglen);

private:
	ev_async_logging(void);
	~ev_async_logging(void);

	/**
	 * @brief 初始化日志文件
	 * @param cur_time 当前的时间
	 * @return 初始化成功返回true，失败返回false
	 */
	bool init_log_file(const ev_time& cur_time);

	/**
	 * @brief 将日志头写入到内部缓冲区
	 * @param level  该条日志的级别，取值范围参照log_level
	 * @param buf    缓冲区
	 * @param buflen 缓冲区长度
	 * @return 返回写入的字节数
	 */
	uint32 prepare_header_info(log_level level, char* buf, uint32 buflen);

	/**
	 * @brief 写入内容到日志缓冲区
	 * @param buf        写入的缓冲区
	 * @param write_size 写入的字节数
	 * @return 返回写入的字节数
	 */
	uint32 write_to_buffer(const char* buf, uint32 write_size);

	/**
	 * @brief 日志线程入口函数
	 * @param arg 回传参数
	 * @return
	 */
	static void logging_thread_entry(void* arg);

private:
	EV_DECLARE_SINGLETON_CLASS(ev_async_logging);

private:
	bool               running_;
	bool               output_to_screen_;
	bool               notified_back_thread_;
	std::string        log_dir_;
	std::string        program_name_;
	ev_lock            lock_;
	file_ptr           cur_log_file_;
	ev_condition       not_empty_;
	ev_countdown_latch latch_;
	ev_thread          logging_thread_;
	fixed_buffer_ptr   cur_buffer_;
	fixed_buffer_ptr   next_buffer_;
	buffer_list        buffers_;
};

} // namespace evpp

#define ev_trace evpp::ev_async_logging::LL_TRACE
#define ev_info  evpp::ev_async_logging::LL_INFO
#define ev_warn  evpp::ev_async_logging::LL_WARN
#define ev_error evpp::ev_async_logging::LL_ERROR

#define ev_log_instance evpp::ev_async_logging::get_instance
#define ev_log_write    ev_log_instance().write
#define ev_log_writef   ev_log_instance().writef

#define ev_log_trace(format_str, ...) ev_log_writef(ev_trace, format_str, ##__VA_ARGS__)
#define ev_log_info(format_str,  ...) ev_log_writef(ev_info,  format_str, ##__VA_ARGS__)
#define ev_log_warn(format_str,  ...) ev_log_writef(ev_warn,  format_str, ##__VA_ARGS__)
#define ev_log_error(format_str, ...) ev_log_writef(ev_error, format_str, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
// std::string str = "Hello, this is an info message!";
// ev_log_write(ev_info, str.c_str(), str.size());
//
// ev_log_write(ev_info, "Hello, this is an info message!");
//
// ev_log_writef(ev_info, "%s", "Hello, this is an info message!");
//
// ev_log_trace("hello, this is a trace message!");
// ev_log_info("hello, this is an info message!");
// ev_log_warn("hello, this is a warning message!");
// ev_log_error("hello, this is an error message!");
//
// ev_log_trace("hello, this is a trace message! file = %s, line = %d",  __EV_FILE__, __EV_LINE__);
// ev_log_info("hello, this is an info message! file = %s, line = %d",   __EV_FILE__, __EV_LINE__);
// ev_log_warn("hello, this is a warning message! file = %s, line = %d", __EV_FILE__, __EV_LINE__);
// ev_log_error("hello, this is an error message! file = %s, line = %d", __EV_FILE__, __EV_LINE__);
//////////////////////////////////////////////////////////////////////////

#endif // __EV_ASYNC_LOGGING_H__