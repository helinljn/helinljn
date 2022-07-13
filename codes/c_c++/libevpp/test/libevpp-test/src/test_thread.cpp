#include "test_thread.h"

#include "base/ev_types.h"
#include "base/ev_thread.h"
#include "base/ev_assert.h"
#include "base/ev_common.h"
#include "base/ev_lock.h"
#include "base/ev_countdown_latch.h"
#include "base/ev_once.h"

#include <cstdio>

using namespace evpp;

namespace test_thread {
namespace details     {

static ev_thread_local int32 global_thread_local_val = 100;
void test_thread_local_thread(void* arg)
{
	fprintf(stdout, "I am %s(%u), global_thread_local_val = %d\n",
		this_thread::thread_name(), this_thread::thread_id(), details::global_thread_local_val);
	ev_assert(100 == details::global_thread_local_val);

	// 子线程修改global_thread_local_val
	details::global_thread_local_val += 100;
	fprintf(stdout, "I am %s(%u), I changed the global_thread_local_val = %d\n",
		this_thread::thread_name(), this_thread::thread_id(), details::global_thread_local_val);
	ev_assert(200 == details::global_thread_local_val);
}

void test_countdown_latch_thread(void* arg)
{
	ev_countdown_latch* latch = reinterpret_cast<ev_countdown_latch*>(arg);
	ev_assert(latch != NULL);

	fprintf(stdout, "I am %s(%u), I am started!\n", this_thread::thread_name(), this_thread::thread_id());

	// 模拟一些运算
	int64 result = 0;
	for (int64 idx = 0; idx <= 500000000; ++idx)
	{
		result = idx * 10;
	}
	ev_assert(5000000000 == result);
	
	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());

	// 通知主线程
	latch->countdown();
}

void test_lock(void* arg)
{
	ev_lock* lock = reinterpret_cast<ev_lock*>(arg);
	ev_assert(lock != NULL);
	ev_assert(!lock->is_locked_by_this_thread());

	ev_lock_guard lock_guard(*lock);
	ev_assert(lock->is_locked_by_this_thread());

	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());
}

int32 global_atomic_lock_var = 0;
void test_atomic_lock(void* arg)
{
	ev_atomic_lock* lock = reinterpret_cast<ev_atomic_lock*>(arg);
	ev_assert(lock != NULL);
	ev_assert(!lock->is_locked_by_this_thread());

	for (int32 idx = 0; idx != 10000000; ++idx)
	{
		ev_lock_guard lock_guard(*lock);
		ev_assert(lock->is_locked_by_this_thread());

		++global_atomic_lock_var;
	}

	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());
}

int32 global_once_called_times = 0;
void test_once_init(void* arg)
{
	++global_once_called_times;
}

void test_once(void* arg)
{
	ev_once* once = reinterpret_cast<ev_once*>(arg);
	ev_assert(once != NULL);
	once->once_init(&test_once_init, NULL);
}

} // namespace details

void test_thread_local(void)
{
	fprintf(stdout, "I am %s(%u), global_thread_local_val = %d\n",
		this_thread::thread_name(), this_thread::thread_id(), details::global_thread_local_val);

	ev_thread th;

	// 创建子线程，并等待子线程运行结束
	ev_assert(th.start_thread(&details::test_thread_local_thread, NULL, "latch thread"));
	ev_assert(th.join_thread());
	
	// 主线程获取global_thread_local_val
	fprintf(stdout, "I am %s(%u), global_thread_local_val = %d\n",
		this_thread::thread_name(), this_thread::thread_id(), details::global_thread_local_val);
	ev_assert(100 == details::global_thread_local_val);
}

void test_countdown_latch(void)
{
	fprintf(stdout, "I am %s(%u), I am started!\n", this_thread::thread_name(), this_thread::thread_id());

	ev_countdown_latch latch(1);
	ev_thread          th;

	// 创建并分离子线程
	ev_assert(th.start_thread(&details::test_countdown_latch_thread, &latch, "latch thread"));
	ev_assert(th.detach_thread());

	// 使用latch等待子线程运行结束
	latch.wait();

	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());
}

void test_lock(void)
{
	{
		ev_recursive_lock lock;
		ev_assert(!lock.is_locked_by_this_thread());

		lock.lock();
		ev_assert(lock.is_locked_by_this_thread());

		lock.lock();
		ev_assert(lock.is_locked_by_this_thread());

		lock.unlock();
		ev_assert(lock.is_locked_by_this_thread());

		lock.unlock();
		ev_assert(!lock.is_locked_by_this_thread());
	}

	{
		ev_lock lock;
		ev_assert(!lock.is_locked_by_this_thread());

		lock.lock();
		ev_assert(lock.is_locked_by_this_thread());

		// 死锁
		//lock.lock();

		lock.unlock();
		ev_assert(!lock.is_locked_by_this_thread());
	}

	{
		ev_atomic_lock lock;
		ev_assert(!lock.is_locked_by_this_thread());

		lock.lock();
		ev_assert(lock.is_locked_by_this_thread());

		// 死锁
		//lock.lock();

		lock.unlock();
		ev_assert(!lock.is_locked_by_this_thread());
	}

	ev_lock lock;
	ev_assert(!lock.is_locked_by_this_thread());

	lock.lock();
	ev_assert(lock.is_locked_by_this_thread());

	// 死锁
	//lock.lock();

	lock.unlock();
	ev_assert(!lock.is_locked_by_this_thread());

	ev_thread th;
	ev_assert(th.start_thread(&details::test_lock, &lock, "lock thread"));

	// 模拟一些运算
	{
		ev_lock_guard lock_guard(lock);
		int64 result = 0;
		for (int64 idx = 0; idx <= 200000000; ++idx)
		{
			result = idx * 10;
		}

		ev_assert(2000000000 == result);
		ev_assert(lock.is_locked_by_this_thread());
	}

	ev_assert(th.join_thread());

	ev_assert(!lock.is_locked_by_this_thread());
	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());
}

void test_atomic_lock(void)
{
	ev_atomic_lock lock;
	ev_thread      th1;
	ev_thread      th2;
	ev_thread      th3;
	ev_thread      th4;

	ev_assert(th1.start_thread(&details::test_atomic_lock, &lock, "test atomic thread 1"));
	ev_assert(th2.start_thread(&details::test_atomic_lock, &lock, "test atomic thread 2"));
	ev_assert(th3.start_thread(&details::test_atomic_lock, &lock, "test atomic thread 3"));
	ev_assert(th4.start_thread(&details::test_atomic_lock, &lock, "test atomic thread 4"));

	ev_assert(th1.join_thread());
	ev_assert(th2.join_thread());
	ev_assert(th3.join_thread());
	ev_assert(th4.join_thread());

	ev_assert(40000000 == details::global_atomic_lock_var);
	ev_assert(!lock.is_locked_by_this_thread());
	fprintf(stdout, "I am %s(%u), I am finished!\n", this_thread::thread_name(), this_thread::thread_id());
}

void test_once(void)
{
	for (uint32 idx = 0; idx != 256; ++idx)
	{
		ev_thread th1;
		ev_thread th2;
		ev_thread th3;
		ev_thread th4;
		ev_once   once;

		ev_assert(th1.start_thread(&details::test_once, &once, "test once thread(1)"));
		ev_assert(th2.start_thread(&details::test_once, &once, "test once thread(2)"));
		ev_assert(th3.start_thread(&details::test_once, &once, "test once thread(3)"));
		ev_assert(th4.start_thread(&details::test_once, &once, "test once thread(4)"));

		ev_assert(th1.join_thread());
		ev_assert(th2.join_thread());
		ev_assert(th3.join_thread());
		ev_assert(th4.join_thread());

		ev_assert(1 == details::global_once_called_times);
		details::global_once_called_times = 0;
	}

	fprintf(stdout, "test ev_once success!\n");
}

} // namespace test_thread