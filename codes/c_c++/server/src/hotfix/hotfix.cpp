#include "hotfix.h"
#include "Poco/Array.h"
#include "Poco/MemoryStream.h"

std::string test_global_func_patch(int32_t val)
{
    Poco::Array<char, 1024>  buf;
    Poco::MemoryOutputStream mos(buf.data(), buf.size());
    poco_assert(mos.good());

    mos << "test_global_func_patch(" << val << ")" << std::ends;
    poco_assert(mos.good());

    return std::string(buf.data());
}