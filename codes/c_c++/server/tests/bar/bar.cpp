#include "bar.h"
#include "Poco/Array.h"
#include "Poco/MemoryStream.h"

std::string test_bar(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "test_bar(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_test_bar(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_test_bar(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string bar::func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "bar::func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string bar::func2(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "bar::func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_bar::hotfix_func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_bar::hotfix_func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string hotfix_bar::hotfix_func2(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "hotfix_bar::hotfix_func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}