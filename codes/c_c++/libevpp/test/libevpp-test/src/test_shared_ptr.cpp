#include "test_shared_ptr.h"
#include "test_public.h"

#include "base/ev_shared_ptr.h"
#include "base/ev_assert.h"
#include "base/ev_thread.h"
#include "base/ev_countdown_latch.h"
#include "base/ev_time_counter.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace evpp;
using namespace test_public;

namespace test_shared_ptr {

struct test_x
{
public:
	static ev_shared_ptr<test_x> create(void)
	{
		return ev_shared_ptr<test_x>(new test_x);
	}

public:
	test_x(void)  {++instance_count;}
	~test_x(void) {--instance_count;}

public:
	static int32 instance_count;
};

int32 test_x::instance_count = 0;

struct test_y : public ev_enable_shared_from_this<test_y>
{
public:
	test_y(void) : ni(100)
	{
		fprintf(stdout, "test_y()\n");
	}

	~test_y(void)
	{
		fprintf(stdout, "~test_y()\n");
	}

public:
	int32 ni;
};

const static uint32 multi_thread_shared_ptr_test_count = 1000000;
void test_multi_thread_shared_ptr_thread(void* arg)
{
	fprintf(stdout, "I am %s(%u), I am started!\n", this_thread::thread_name(), this_thread::thread_id());

	ev_shared_ptr<person>* sp = reinterpret_cast<ev_shared_ptr<person>*>(arg);
	ev_assert(sp != NULL);

	std::vector<ev_shared_ptr<person> > vsp;
	vsp.reserve(multi_thread_shared_ptr_test_count);
	for (uint32 idx = 0; idx != multi_thread_shared_ptr_test_count; ++idx)
	{
		vsp.push_back(*sp);
	}
}

void test_ev_shared_ptr(void)
{
	{
		ev_shared_ptr<person> ps(new person("shmilyl", 25));
		ev_assert(ps && 1 == ps.use_count());
		fprintf(stdout, "I am %s. I am %d.\n", ps->get_name().c_str(), ps->get_age());
	}
	fprintf(stdout, "--------------------\n");
	
	{
		ev_shared_ptr<person> ps(new student("Milan", 25));
		ev_assert(ps && 1 == ps.use_count());
		fprintf(stdout, "I am %s. I am %d.\n", ps->get_name().c_str(), ps->get_age());
	}
	fprintf(stdout, "--------------------\n");

	{
		ev_shared_ptr<person> ps1(new person("shmilyl", 25));
		ev_assert(ps1 && 1 == ps1.use_count());

		ev_shared_ptr<person> ps2(new student("Milan", 25));
		ev_assert(ps2 && 1 == ps2.use_count());

		swap(ps1, ps2);
		ev_assert(ps1 && ps2 && 1 == ps1.use_count() && 1 == ps2.use_count());
	}
	fprintf(stdout, "--------------------\n");

	{
		ev_shared_ptr<person> ps(new person("shmilyl", 25));
		ev_assert(ps && 1 == ps.use_count());

		ps.reset(new student("Milan", 25));
		ev_assert(ps && 1 == ps.use_count());
	}

	ev_assert(0 == test_x::instance_count);

	{
		ev_shared_ptr<test_x> px(test_x::create());
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px.reset();
		ev_assert(0 == test_x::instance_count && 0 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());
	}

	ev_assert(0 == test_x::instance_count);

	{
		ev_shared_ptr<const test_x> px(test_x::create());
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px.reset();
		ev_assert(0 == test_x::instance_count && 0 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());
	}

	ev_assert(0 == test_x::instance_count);

	{
		ev_shared_ptr<void> px(test_x::create());
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px.reset();
		ev_assert(0 == test_x::instance_count && 0 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());
	}

	ev_assert(0 == test_x::instance_count);

	{
		ev_shared_ptr<const void> px(test_x::create());
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());

		px.reset();
		ev_assert(0 == test_x::instance_count && 0 == px.use_count());

		px = test_x::create();
		ev_assert(1 == test_x::instance_count && 1 == px.use_count());
	}

	ev_assert(0 == test_x::instance_count);
}

void test_ev_weak_ptr(void)
{
	{
		test_y*               raw_py = new test_y;
		ev_shared_ptr<test_y> py1(raw_py);
		ev_assert(raw_py == py1.get());
		ev_assert(1 == py1.use_count());
		
		ev_shared_ptr<test_y> py2 = raw_py->shared_from_this();
		ev_assert(raw_py == py2.get());
		ev_assert(2 == py2.use_count());

		py1.reset();
		ev_assert(0 == py1.use_count() && 1 ==py2.use_count());
	}
}

void test_multi_thread_shared_ptr(void)
{
	const uint32            thread_num = 8;
	std::vector<ev_thread*> threads;
	ev_time_counter         counter;
	ev_shared_ptr<person>   ps(new person("shmilyl", 25));

	// create threads
	for (uint32 idx = 0; idx != thread_num; ++idx)
	{
		threads.push_back(new ev_thread);
	}
	ev_assert(threads.size() == thread_num);

	counter.start();
	// start threads
	for (uint32 idx = 0; idx != thread_num; ++idx)
	{
		ev_assert(threads[idx]->start_thread(&test_multi_thread_shared_ptr_thread, &ps));
	}

	// join threads
	for (uint32 idx = 0; idx != thread_num; ++idx)
	{
		ev_assert(threads[idx]->join_thread());
	}
	counter.end();

	ev_assert(1 == ps.use_count());

	// delete threads
	for (uint32 idx = 0; idx != thread_num; ++idx)
	{
		delete threads[idx];
	}

	fprintf(stdout, "thread number:%u, and each repeat times:%u costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
		thread_num, multi_thread_shared_ptr_test_count, counter.seconds(), counter.milliseconds(), counter.microseconds());
}

} // namespace test_shared_ptr