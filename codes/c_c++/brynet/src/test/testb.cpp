#include "testb.h"
#include "fmt/format.h"
#include "core/common.h"

std::string testb_func(int32_t val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   testb_func({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb_base::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   testb_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   testb::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func2(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   testb::func2({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func3(const std::string& val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   testb::func3({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}