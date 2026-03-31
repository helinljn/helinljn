#include "testa/testa.h"
#include "test/testb.h"
#include "fmt/format.h"
#include "core/common.h"

std::string testa_func(int32_t val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testa_func({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testa_base::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testa_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testa::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testa::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testa::func2(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testa::func2({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testa::func3(const std::string& val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testa::func3({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb_func(int32_t val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testb_func({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb_base::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testb_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func1(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testb::func1({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func2(const std::string& val) const
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testb::func2({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}

std::string testb::func3(const std::string& val)
{
    const int         ramdom_val = core::random_range(1000);
    const std::string ret        = fmt::format("   patch_testb::func3({}), ramdom_val({})   ", val, ramdom_val);
    return core::trim(ret);
}