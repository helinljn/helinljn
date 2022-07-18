#include "test_size.h"
#include "spdlog/fmt/fmt.h"

#include <ctime>
#include <cstdint>
#include <limits>

void test_sizeof(void)
{
    static_assert(1 == sizeof(int8_t));
    static_assert(1 == sizeof(uint8_t));

    static_assert(2 == sizeof(int16_t));
    static_assert(2 == sizeof(uint16_t));

    static_assert(4 == sizeof(int32_t));
    static_assert(4 == sizeof(uint32_t));

    static_assert(8 == sizeof(int64_t));
    static_assert(8 == sizeof(uint64_t));

    static_assert(4 == sizeof(float));
    static_assert(8 == sizeof(double));

    static_assert(8 == sizeof(time_t));
    static_assert(8 == sizeof(size_t));
    static_assert(8 == sizeof(ptrdiff_t));
    static_assert(8 == sizeof(intptr_t));

    static_assert(201703 == __cplusplus);
    static_assert(818    == XXX_VALUE_JUST_FOR_TEST);

    fmt::print("{}() success!\n", __func__);
}

void test_numeric_limits(void)
{
    static_assert(SCHAR_MIN == std::numeric_limits<int8_t>::min());
    static_assert(SCHAR_MAX == std::numeric_limits<int8_t>::max());
    static_assert(UCHAR_MAX == std::numeric_limits<uint8_t>::max());

    static_assert(SHRT_MIN  == std::numeric_limits<int16_t>::min());
    static_assert(SHRT_MAX  == std::numeric_limits<int16_t>::max());
    static_assert(USHRT_MAX == std::numeric_limits<uint16_t>::max());

    static_assert(INT_MIN  == std::numeric_limits<int32_t>::min());
    static_assert(INT_MAX  == std::numeric_limits<int32_t>::max());
    static_assert(UINT_MAX == std::numeric_limits<uint32_t>::max());

    static_assert(LLONG_MIN  == std::numeric_limits<int64_t>::min());
    static_assert(LLONG_MAX  == std::numeric_limits<int64_t>::max());
    static_assert(ULLONG_MAX == std::numeric_limits<uint64_t>::max());

    fmt::print("{}() success!\n", __func__);
}