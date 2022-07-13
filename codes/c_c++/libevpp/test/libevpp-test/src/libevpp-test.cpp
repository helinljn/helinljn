#include <cstdio>
#include <cstdlib>

#include "base/ev_assert.h"
#include "base/ev_thread.h"
#include "base/ev_time_counter.h"
#include "base/ev_async_logging.h"
#include "base/ev_countdown_latch.h"

#include "libevpp.h"
#include "test_misc.h"
#include "test_atomic.h"
#include "test_thread.h"
#include "test_scoped_ptr.h"
#include "test_shared_ptr.h"
#include "test_libevent.h"
#include "test_socket.h"
#include "test_tcp_performance.h"

#define evpp_test_begin(xx)  fprintf(stdout, "---------- test %s begin ----------\n",   xx);{
#define evpp_test_end(xx)    }fprintf(stdout, "---------- test %s end   ----------\n\n", xx);

//#define TEST_MISC
//#define TEST_ASYNC_LOGGING
//#define TEST_MULTI_ASYNC_RAW_LOGGING
//#define TEST_MULTI_ASYNC_FMT_LOGGING
//#define TEST_LIBEVENT
//#define TEST_SOCKET
//#define TEST_TCP_PERFORMANCE

#ifdef TEST_MULTI_ASYNC_RAW_LOGGING
static void test_async_logging_raw_thread(void* arg)
{
	evpp::ev_countdown_latch* latch = reinterpret_cast<evpp::ev_countdown_latch*>(arg);
	ev_assert(latch != NULL && "latch is a null pointer, please check your code!");
	if (NULL == latch)
	{
		return;
	}

	latch->wait();

	fprintf(stdout, "I am %s(%u), I am started!\n", evpp::this_thread::thread_name(), evpp::this_thread::thread_id());

	// 每次写入100字节，写入200万次(约260MB)
	for (evpp::int32 idx = 0; idx != 2000000; ++idx)
	{
		ev_log_write(ev_trace, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	}

	fprintf(stdout, "I am %s(%u), I am finished!\n", evpp::this_thread::thread_name(), evpp::this_thread::thread_id());
}
#endif // TEST_MULTI_ASYNC_RAW_LOGGING

#ifdef TEST_MULTI_ASYNC_FMT_LOGGING
static void test_async_logging_fmt_thread(void* arg)
{
	evpp::ev_countdown_latch* latch = reinterpret_cast<evpp::ev_countdown_latch*>(arg);
	ev_assert(latch != NULL && "latch is a null pointer, please check your code!");
	if (NULL == latch)
	{
		return;
	}

	latch->wait();

	fprintf(stdout, "I am %s(%u), I am started!\n", evpp::this_thread::thread_name(), evpp::this_thread::thread_id());

	// 每次写入100字节，写入200万次(约260MB)
	for (evpp::int32 idx = 0; idx != 2000000; ++idx)
	{
		ev_log_writef(ev_trace, "%s", "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	}

	fprintf(stdout, "I am %s(%u), I am finished!\n", evpp::this_thread::thread_name(), evpp::this_thread::thread_id());
}
#endif // TEST_MULTI_ASYNC_FMT_LOGGING

int main(void)
{
#if !defined(TEST_TCP_PERFORMANCE)
	// 初始化libevpp库
	if (!libevpp::libevpp_init("libevpp-test"))
	{
		return EXIT_FAILURE;
	}

#if defined(__EV_WINDOWS__) && !defined(TEST_ASYNC_LOGGING) && !defined(TEST_MULTI_ASYNC_RAW_LOGGING) && !defined(TEST_MULTI_ASYNC_FMT_LOGGING)
	// 设置日志内容输出至屏幕
	ev_log_instance().set_output_to_screen(true);
#endif // defined(__EV_WINDOWS__) && !defined(TEST_ASYNC_LOGGING)

#ifdef TEST_MISC
	ev_assert(ev_log_instance().start("libevpp-test-log-restart"));
	ev_assert(ev_log_instance().start("libevpp-test-log-restart"));
	ev_log_instance().stop();
	ev_log_instance().stop();
	ev_assert(ev_log_instance().start("libevpp-test-log-restart"));
	ev_assert(ev_log_instance().start("libevpp-test-log-restart"));

	ev_log_write(ev_trace, "this is a trace message!");
	ev_log_write(ev_info,  "this is an info message!");
	ev_log_write(ev_warn,  "this is a warning message!");
	ev_log_write(ev_error, "this is an error message!");

	ev_log_writef(ev_trace, "%s (%s:%d)", "this is a formatted trace message!",   __EV_FILE__, __EV_LINE__);
	ev_log_writef(ev_info,  "%s (%s:%d)", "this is a formatted info message!",    __EV_FILE__, __EV_LINE__);
	ev_log_writef(ev_warn,  "%s (%s:%d)", "this is a formatted warning message!", __EV_FILE__, __EV_LINE__);
	ev_log_writef(ev_error, "%s (%s:%d)", "this is a formatted error message!",   __EV_FILE__, __EV_LINE__);

	ev_log_trace("this is a formatted trace message! (%s:%d)",  __EV_FILE__, __EV_LINE__);
	ev_log_info("this is a formatted info message! (%s:%d)",    __EV_FILE__, __EV_LINE__);
	ev_log_warn("this is a formatted warning message! (%s:%d)", __EV_FILE__, __EV_LINE__);
	ev_log_error("this is a formatted error message! (%s:%d)",  __EV_FILE__, __EV_LINE__);

	evpp_test_begin("test_misc::test_ev_common");
		test_misc::test_ev_common();
	evpp_test_end("test_misc::test_ev_common");

	evpp_test_begin("test_misc::test_md5");
		test_misc::test_md5();
	evpp_test_end("test_misc::test_md5");

	evpp_test_begin("test_misc::test_sha");
		test_misc::test_sha();
	evpp_test_end("test_misc::test_sha");

	evpp_test_begin("test_misc::test_aes");
		test_misc::test_aes();
	evpp_test_end("test_misc::test_aes");

	evpp_test_begin("test_misc::test_base64");
		test_misc::test_base64();
	evpp_test_end("test_misc::test_base64");

	evpp_test_begin("test_misc::test_time");
		test_misc::test_time(100000);
	evpp_test_end("test_misc::test_time");

	evpp_test_begin("test_misc::test_time_counter");
		test_misc::test_time_counter(100000);
	evpp_test_end("test_misc::test_time_counter");

	evpp_test_begin("test_misc::test_random");
		test_misc::test_random(10000000);
	evpp_test_end("test_misc::test_random");

	evpp_test_begin("test_misc::test_fixed_buffer");
		test_misc::test_fixed_buffer();
	evpp_test_end("test_misc::test_fixed_buffer");

	evpp_test_begin("test_misc::test_stream_buffer");
		test_misc::test_stream_buffer();
	evpp_test_end("test_misc::test_stream_buffer");

	evpp_test_begin("test_misc::test_file");
		test_misc::test_file();
	evpp_test_end("test_misc::test_file");

	evpp_test_begin("test_misc::test_socketpair");
		test_misc::test_socketpair();
	evpp_test_end("test_misc::test_socketpair");

	evpp_test_begin("test_atomic::test_atomic_increment_and_fetch");
		test_atomic::test_atomic_increment_and_fetch();
	evpp_test_end("test_atomic::test_atomic_increment_and_fetch");

	evpp_test_begin("test_atomic::test_atomic_decrement_and_fetch");
		test_atomic::test_atomic_decrement_and_fetch();
	evpp_test_end("test_atomic::test_atomic_decrement_and_fetch");

	evpp_test_begin("test_atomic::test_atomic_fetch_and_increment");
		test_atomic::test_atomic_fetch_and_increment();
	evpp_test_end("test_atomic::test_atomic_fetch_and_increment");

	evpp_test_begin("test_atomic::test_atomic_fetch_and_decrement");
		test_atomic::test_atomic_fetch_and_decrement();
	evpp_test_end("test_atomic::test_atomic_fetch_and_decrement");

	evpp_test_begin("test_atomic::test_atomic_fetch_and_add");
		test_atomic::test_atomic_fetch_and_add();
	evpp_test_end("test_atomic::test_atomic_fetch_and_add");

	evpp_test_begin("test_atomic::test_producer_and_consumer");
		test_atomic::test_producer_and_consumer();
	evpp_test_end("test_atomic::test_producer_and_consumer");

	evpp_test_begin("test_thread::test_thread_local");
		test_thread::test_thread_local();
	evpp_test_end("test_thread::test_thread_local");

	evpp_test_begin("test_thread::test_countdown_latch");
		test_thread::test_countdown_latch();
	evpp_test_end("test_thread::test_countdown_latch");

	evpp_test_begin("test_thread::test_lock");
		test_thread::test_lock();
	evpp_test_end("test_thread::test_lock");

	evpp_test_begin("test_thread::test_atomic_lock");
		test_thread::test_atomic_lock();
	evpp_test_end("test_thread::test_atomic_lock");

	evpp_test_begin("test_thread::test_once");
		test_thread::test_once();
	evpp_test_end("test_thread::test_once");

	evpp_test_begin("test_scoped_ptr::test_ev_scoped_ptr");
		test_scoped_ptr::test_ev_scoped_ptr();
	evpp_test_end("test_scoped_ptr::test_ev_scoped_ptr");

	evpp_test_begin("test_shared_ptr::test_ev_shared_ptr");
		test_shared_ptr::test_ev_shared_ptr();
	evpp_test_end("test_shared_ptr::test_ev_shared_ptr");

	evpp_test_begin("test_shared_ptr::test_ev_weak_ptr");
		test_shared_ptr::test_ev_weak_ptr();
	evpp_test_end("test_shared_ptr::test_ev_weak_ptr");

	evpp_test_begin("test_shared_ptr::test_multi_thread_shared_ptr");
		test_shared_ptr::test_multi_thread_shared_ptr();
	evpp_test_end("test_shared_ptr::test_multi_thread_shared_ptr");
#endif // TEST_MISC

#ifdef TEST_ASYNC_LOGGING
	evpp_test_begin("evpp::ev_async_log");
		// 每次写入100字节，写入500万次(约650MB)需要多久
		evpp::uint32          repeat_times = 5000000;
		evpp::ev_time_counter counter;

		counter.start();
		for (evpp::uint32 idx = 0; idx < repeat_times; ++idx)
		{
			ev_log_write(ev_trace, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
		}
		counter.end();

		fprintf(stdout, "ev_log_write()! repeat %u times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
			repeat_times, counter.seconds(), counter.milliseconds(), counter.microseconds());

		counter.reset();
		counter.start();
		for (evpp::uint32 idx = 0; idx < repeat_times; ++idx)
		{
			ev_log_writef(ev_trace, "%s", "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
		}
		counter.end();

		fprintf(stdout, "ev_log_writef()! repeat %u times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
			repeat_times, counter.seconds(), counter.milliseconds(), counter.microseconds());
	evpp_test_end("evpp::st_async_log");
#endif // TEST_ASYNC_LOGGING

#ifdef TEST_MULTI_ASYNC_RAW_LOGGING
	evpp_test_begin("evpp::ev_async_log raw-multi-thread");
		evpp::ev_time_counter    counter;
		evpp::ev_countdown_latch latch(1);
		evpp::ev_thread          th1;
		evpp::ev_thread          th2;
		evpp::ev_thread          th3;
		evpp::ev_thread          th4;
		evpp::ev_thread          th5;
		evpp::ev_thread          th6;
		evpp::ev_thread          th7;
		evpp::ev_thread          th8;

		ev_assert(th1.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 1"));
		ev_assert(th2.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 2"));
		ev_assert(th3.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 3"));
		ev_assert(th4.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 4"));
		ev_assert(th5.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 5"));
		ev_assert(th6.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 6"));
		ev_assert(th7.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 7"));
		ev_assert(th8.start_thread(&test_async_logging_raw_thread, &latch, "ev_async_log raw-multi-thread 8"));

		// 等待500毫秒，保证线程全部成功创建
		evpp::this_thread::sleep_for(500);

		// 通知线程继续运行
		latch.countdown();

		counter.start();
			// 等待线程结束
			ev_assert(th1.join_thread());
			ev_assert(th2.join_thread());
			ev_assert(th3.join_thread());
			ev_assert(th4.join_thread());
			ev_assert(th5.join_thread());
			ev_assert(th6.join_thread());
			ev_assert(th7.join_thread());
			ev_assert(th8.join_thread());
		counter.end();

		fprintf(stdout, "ev_log_write()! repeat %d times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
			2000000 * 8, counter.seconds(), counter.milliseconds(), counter.microseconds());
	evpp_test_end("evpp::st_async_log raw-multi-thread");
#endif // TEST_MULTI_ASYNC_RAW_LOGGING

#ifdef TEST_MULTI_ASYNC_FMT_LOGGING
	evpp_test_begin("evpp::ev_async_log fmt-multi-thread");
		evpp::ev_time_counter    counter;
		evpp::ev_countdown_latch latch(1);
		evpp::ev_thread          th1;
		evpp::ev_thread          th2;
		evpp::ev_thread          th3;
		evpp::ev_thread          th4;
		evpp::ev_thread          th5;
		evpp::ev_thread          th6;
		evpp::ev_thread          th7;
		evpp::ev_thread          th8;

		ev_assert(th1.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 1"));
		ev_assert(th2.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 2"));
		ev_assert(th3.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 3"));
		ev_assert(th4.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 4"));
		ev_assert(th5.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 5"));
		ev_assert(th6.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 6"));
		ev_assert(th7.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 7"));
		ev_assert(th8.start_thread(&test_async_logging_fmt_thread, &latch, "ev_async_log fmt-multi-thread 8"));

		// 等待500毫秒，保证线程全部成功创建
		evpp::this_thread::sleep_for(500);

		// 通知线程继续运行
		latch.countdown();

		counter.start();
			// 等待线程结束
			ev_assert(th1.join_thread());
			ev_assert(th2.join_thread());
			ev_assert(th3.join_thread());
			ev_assert(th4.join_thread());
			ev_assert(th5.join_thread());
			ev_assert(th6.join_thread());
			ev_assert(th7.join_thread());
			ev_assert(th8.join_thread());
		counter.end();

		fprintf(stdout, "ev_log_writef()! repeat %d times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
			2000000 * 8, counter.seconds(), counter.milliseconds(), counter.microseconds());
	evpp_test_end("evpp::st_async_log fmt-multi-thread");
#endif // TEST_MULTI_ASYNC_FMT_LOGGING

#ifdef TEST_LIBEVENT
	evpp_test_begin("test_libevent::test_libevent_interface");
		test_libevent::test_libevent_interface();
	evpp_test_end("test_libevent::test_libevent_interface");
#endif // TEST_LIBEVENT

#ifdef TEST_SOCKET
	evpp_test_begin("test_socket::test_tcp_echo_server");
		test_socket::test_tcp_echo_server();
	evpp_test_end("test_socket::test_tcp_echo_server");
#endif // TEST_SOCKET

#endif // !defined(TEST_TCP_PERFORMANCE)

#ifdef TEST_TCP_PERFORMANCE
	// 初始化libevpp库
	if (!libevpp::libevpp_init("TCPPerformanceTest"))
	{
		return EXIT_FAILURE;
	}

	// 设置日志内容输出至屏幕
	ev_log_instance().set_output_to_screen(true);

	test_tcp_performance::test_performance_main();
#endif // TEST_TCP_PERFORMANCE

	// 销毁libevpp库
	libevpp::libevpp_destroy();

	return EXIT_SUCCESS;
}