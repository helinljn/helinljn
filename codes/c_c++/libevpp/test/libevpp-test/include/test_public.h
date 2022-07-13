//////////////////////////////////////////////////////////////////////////
// 公共测试结构体
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_PUBLIC_H__
#define __TEST_PUBLIC_H__

#include "base/ev_types.h"

#include <cstdio>
#include <string>

namespace test_public {

class person
{
public:
	person(const std::string& name, evpp::int32 age)
		: name_(name)
		, age_(age)
	{
		fprintf(stdout, "person(%s, %d)\n", get_name().c_str(), get_age());
	}

	virtual ~person(void)
	{
		fprintf(stdout, "~person(%s, %d)\n", get_name().c_str(), get_age());
	}

	const std::string& get_name(void) const { return name_; }

	evpp::int32 get_age(void) const { return age_; }

private:
	std::string name_;
	evpp::int32 age_;
};

class student : public person
{
public:
	student(const std::string& name, evpp::int32 age)
		: person(name, age)
	{
		fprintf(stdout, "student(%s, %d)\n", get_name().c_str(), get_age());
	}

	virtual ~student(void)
	{
		fprintf(stdout, "~student(%s, %d)\n", get_name().c_str(), get_age());
	}
};

} // namespace test_public

#endif // __TEST_PUBLIC_H__