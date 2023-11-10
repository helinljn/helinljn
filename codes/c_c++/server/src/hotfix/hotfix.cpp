#include "hotfix.h"
#include "Poco/Array.h"
#include "Poco/MemoryStream.h"

std::string test_func(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "test_func(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_test_func(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_test_func(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string foo::func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "foo::func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string foo::func2(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "foo::func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_foo::hotfix_func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_foo::hotfix_func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_foo::hotfix_func2(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_foo::hotfix_func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}