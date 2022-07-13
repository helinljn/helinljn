#include "test_scoped_ptr.h"
#include "test_public.h"

#include "base/ev_scoped_ptr.h"
#include "base/ev_assert.h"

#include <cstdio>
#include <string>

using namespace evpp;
using namespace test_public;

namespace test_scoped_ptr {

void test_ev_scoped_ptr(void)
{
	{
		ev_scoped_ptr<person> ps(new person("shmilyl", 25));
		ev_assert(ps);
		fprintf(stdout, "I am %s. I am %d.\n", ps->get_name().c_str(), ps->get_age());
	}
	fprintf(stdout, "--------------------\n");
	
	{
		ev_scoped_ptr<person> ps(new student("Milan", 25));
		ev_assert(ps);
		fprintf(stdout, "I am %s. I am %d.\n", ps->get_name().c_str(), ps->get_age());
	}
	fprintf(stdout, "--------------------\n");

	{
		ev_scoped_ptr<person> ps1(new person("shmilyl", 25));
		ev_assert(ps1);

		ev_scoped_ptr<person> ps2(new student("Milan", 25));
		ev_assert(ps2);

		swap(ps1, ps2);
	}
	fprintf(stdout, "--------------------\n");

	{
		ev_scoped_ptr<person> ps(new person("shmilyl", 25));
		ev_assert(ps);

		ps.reset(new student("Milan", 25));
		ev_assert(ps);
	}
}

} // namespace test_scoped_ptr