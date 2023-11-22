#include "fooex.h"
#include "Poco/Array.h"
#include "Poco/MemoryStream.h"

std::string test_fooex(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "test_fooex(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string patch_test_fooex(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "patch_test_fooex(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string fooex_base::func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "fooex_base::func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string fooex::func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "fooex::func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string fooex::func2(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "fooex::func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string fooex::func3(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "fooex::func3(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string patch_fooex::patch_func1(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "patch_fooex::patch_func1(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string patch_fooex::patch_func2(const std::string& val) const
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "patch_fooex::patch_func2(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}

std::string patch_fooex::patch_func3(const std::string& val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "patch_fooex::patch_func3(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}