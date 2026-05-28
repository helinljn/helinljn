#include "testa.h"
#include "Poco/Random.h"
#include "Poco/String.h"
#include <format>

std::string testa_func(int32_t val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testa_func({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa_base::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testa_base::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func1(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testa::func1({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func2(const std::string& val) const
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testa::func2({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}

std::string testa::func3(const std::string& val)
{
    const auto        ramdom_val = Poco::Random().next(1000);
    const std::string ret        = std::format("   testa::func3({}), ramdom_val({})   ", val, ramdom_val);
    return Poco::trim(ret);
}
