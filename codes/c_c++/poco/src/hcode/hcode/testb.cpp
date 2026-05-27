#include "testb.h"
#include "Poco/Random.h"
#include "Poco/String.h"
#include <format>

std::string testb_func(int32_t val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testb_func({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb_base::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testb_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testb::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func2(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testb::func2({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func3(const std::string& val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testb::func3({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}
