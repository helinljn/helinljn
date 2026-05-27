#include "testa/testa.h"
#include "hcode/testb.h"
#include "Poco/Random.h"
#include "Poco/String.h"
#include <format>

std::string testa_func(int32_t val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testa_func({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa_base::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testa_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testa::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func2(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testa::func2({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func3(const std::string& val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testa::func3({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb_func(int32_t val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testb_func({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb_base::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testb_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testb::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func2(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testb::func2({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testb::func3(const std::string& val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   patch_testb::func3({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}
