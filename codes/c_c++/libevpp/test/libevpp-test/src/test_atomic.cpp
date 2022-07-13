#include "test_atomic.h"

#include "base/ev_atomic.h"
#include "base/ev_assert.h"
#include "base/ev_thread.h"

#include <cstdio>

using namespace evpp;

namespace test_atomic {

static ev_atomic_int32 global_atomic_lock = 0;
static const int32     global_test_times  = 5;

void producer_thread(void* arg)
{
	fprintf(stdout, "I am %s(%u), I am started!\n", this_thread::thread_name(), this_thread::thread_id());

	int32 times = 0;
	while (true)
	{
		while (0 != ev_atomic_fetch_and_add(&global_atomic_lock, 0))
		{
			this_thread::sleep_for(0);
		}

		fprintf(stdout, "I am %s(%u), I produce a commodity!+\n", this_thread::thread_name(), this_thread::thread_id());
		ev_assert(0 == ev_atomic_fetch_and_increment(&global_atomic_lock));

		++times;
		if (global_test_times == times)
		{
			break;
		}
	}

	fprintf(stdout, "I am %s(%u), I am ended!\n", this_thread::thread_name(), this_thread::thread_id());
}

void consumer_thread(void* arg)
{
	fprintf(stdout, "I am %s(%u), I am started!\n", this_thread::thread_name(), this_thread::thread_id());

	int32 times = 0;
	while (true)
	{
		while (1 != ev_atomic_fetch_and_add(&global_atomic_lock, 0))
		{
			this_thread::sleep_for(0);
		}

		fprintf(stdout, "I am %s(%u), I consume a commodity!-\n", this_thread::thread_name(), this_thread::thread_id());
		ev_assert(1 == ev_atomic_fetch_and_decrement(&global_atomic_lock));

		++times;
		if (global_test_times == times)
		{
			break;
		}
	}

	fprintf(stdout, "I am %s(%u), I am ended!\n", this_thread::thread_name(), this_thread::thread_id());
}

void test_atomic_increment_and_fetch(void)
{
	ev_atomic_int32 i32 = 0, r32 = 0;
	ev_atomic_int64 i64 = 0, r64 = 0;

	// 测试++val
	r32 = ev_atomic_increment_and_fetch(&i32);
	ev_assert(1 == i32 && 1 == r32);
	if (1 == i32 && 1 == r32)
	{
		fprintf(stdout, "test ev_atomic_increment_and_fetch(int32) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_increment_and_fetch(int32) failed!\n");
	}

	r64 = ev_atomic_increment_and_fetch(&i64);
	ev_assert(1 == i64 && 1 == r64);
	if (1 == i64 && 1 == r64)
	{
		fprintf(stdout, "test ev_atomic_increment_and_fetch(int64) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_increment_and_fetch(int64) failed!\n");
	}
}

void test_atomic_decrement_and_fetch(void)
{
	ev_atomic_int32 i32 = 1, r32 = 1;
	ev_atomic_int64 i64 = 1, r64 = 1;

	// 测试--val
	r32 = ev_atomic_decrement_and_fetch(&i32);
	ev_assert(0 == i32 && 0 == r32);
	if (0 == i32 && 0 == r32)
	{
		fprintf(stdout, "test ev_atomic_decrement_and_fetch(int32) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_decrement_and_fetch(int32) failed!\n");
	}

	r64 = ev_atomic_decrement_and_fetch(&i64);
	ev_assert(0 == i64 && 0 == r64);
	if (0 == i64 && 0 == r64)
	{
		fprintf(stdout, "test ev_atomic_decrement_and_fetch(int64) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_decrement_and_fetch(int64) failed!\n");
	}
}

void test_atomic_fetch_and_increment(void)
{
	ev_atomic_int32 i32 = 0, r32 = 0;
	ev_atomic_int64 i64 = 0, r64 = 0;

	// 测试val++
	r32 = ev_atomic_fetch_and_increment(&i32);
	ev_assert(1 == i32 && 0 == r32);
	if (1 == i32 && 0 == r32)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_increment(int32) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_increment(int32) failed!\n");
	}

	r64 = ev_atomic_fetch_and_increment(&i64);
	ev_assert(1 == i64 && 0 == r64);
	if (1 == i64 && 0 == r64)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_increment(int64) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_increment(int64) failed!\n");
	}
}

void test_atomic_fetch_and_decrement(void)
{
	ev_atomic_int32 i32 = 1, r32 = 1;
	ev_atomic_int64 i64 = 1, r64 = 1;

	// 测试val--
	r32 = ev_atomic_fetch_and_decrement(&i32);
	ev_assert(0 == i32 && 1 == r32);
	if (0 == i32 && 1 == r32)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_decrement(int32) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_decrement(int32) failed!\n");
	}

	r64 = ev_atomic_fetch_and_decrement(&i64);
	ev_assert(0 == i64 && 1 == r64);
	if (0 == i64 && 1 == r64)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_decrement(int64) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_decrement(int64) failed!\n");
	}
}

void test_atomic_fetch_and_add(void)
{
	ev_atomic_int32 i32 = 0, r32 = 0;
	ev_atomic_int64 i64 = 0, r64 = 0;

	// 测试val + n
	r32 = ev_atomic_fetch_and_add(&i32, 100);
	ev_assert(100 == i32 && 0 == r32);
	if (100 == i32 && 0 == r32)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_add(int32) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_add(int32) failed!\n");
	}

	r64 = ev_atomic_fetch_and_add(&i64, 100);
	ev_assert(100 == i64 && 0 == r64);
	if (100 == i64 && 0 == r64)
	{
		fprintf(stdout, "test ev_atomic_fetch_and_add(int64) success!\n");
	}
	else
	{
		fprintf(stdout, "test ev_atomic_fetch_and_add(int64) failed!\n");
	}
}

void test_producer_and_consumer(void)
{
	// 创建两个线程，分别为生产者和消费者
	ev_thread producer;
	ev_thread consumer;

	ev_assert(producer.start_thread(&producer_thread, NULL, "producer thread"));
	ev_assert(consumer.start_thread(&consumer_thread, NULL, "consumer thread"));

	ev_assert(producer.join_thread());
	ev_assert(consumer.join_thread());

	// 测试ev_atomic_test_nonzero_and_increment
	ev_atomic_int32 test_val = 0;
	ev_assert(false == ev_atomic_test_nonzero_and_increment(&test_val));
	++test_val;
	ev_assert(true == ev_atomic_test_nonzero_and_increment(&test_val));
	ev_assert(2 == test_val);

	// 测试ev_atomic_test_zero_and_increment
	test_val = 1;
	ev_assert(false == ev_atomic_test_zero_and_increment(&test_val));
	--test_val;
	ev_assert(true == ev_atomic_test_zero_and_increment(&test_val));
	ev_assert(1 == test_val);

	// 测试ev_atomic_get_value
	ev_assert(1 == ev_atomic_get_value(&test_val));
}

} // namespace test_atomic