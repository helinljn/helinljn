#include "test_misc.h"

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/color.h"

#include "protocol/TestMsgDefine.pb.h"
#include "protocol/TestMsgStruct.pb.h"

#include <limits>
#include <utility>

class CommonInfo
{
public:
    bool        test_bool;
    float       test_float;
    double      test_double;
    std::string test_string;
    int32_t     test_int32;
    int32_t     test_sint32;
    uint32_t    test_uint32;
    int64_t     test_int64;
    int64_t     test_sint64;
    uint64_t    test_uint64;
};

bool operator ==(const CommonInfo& cinfo, const TestMsg::CommonInfo& pinfo)
{
    return cinfo.test_bool   == pinfo.test_bool()
        && cinfo.test_float  == pinfo.test_float()
        && cinfo.test_double == pinfo.test_double()
        && cinfo.test_string == pinfo.test_string()
        && cinfo.test_int32  == pinfo.test_int32()
        && cinfo.test_sint32 == pinfo.test_sint32()
        && cinfo.test_uint32 == pinfo.test_uint32()
        && cinfo.test_int64 == pinfo.test_int64()
        && cinfo.test_sint64 == pinfo.test_sint64()
        && cinfo.test_uint64 == pinfo.test_uint64();
}

bool operator ==(const TestMsg::CommonInfo& pinfo, const CommonInfo& cinfo)
{
    return cinfo == pinfo;
}

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

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
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

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}

void test_safe_time(void)
{
    using namespace common;

    time_t cur_timestamp = system_clock_now<std::chrono::seconds>();
    fmt::print("Current timestamp={}\n", cur_timestamp);
    fmt::print("Call safe_ctime() is:{}\n", safe_ctime(&cur_timestamp));

    tm local_tm = safe_localtime(&cur_timestamp);
    fmt::print("Call safe_localtime() and safe_asctime() is:{}\n", safe_asctime(&local_tm));

    tm gm_tm = safe_gmtime(&cur_timestamp);
    fmt::print("Call safe_gmtime() and safe_asctime() is:{}\n", safe_asctime(&gm_tm));

    time_t start_time{};
    time_t end_time{};

    start_time = steady_clock_now();
    local_tm   = safe_localtime(&cur_timestamp);
    end_time   = steady_clock_now();
    fmt::print("safe_localtime() cost {} microseconds\n", end_time - start_time);

    start_time = steady_clock_now();
    gm_tm      = safe_gmtime(&cur_timestamp);
    end_time   = steady_clock_now();
    fmt::print("safe_gmtime() cost {} microseconds\n", end_time - start_time);

    start_time = steady_clock_now();
    safe_ctime(&cur_timestamp);
    end_time   = steady_clock_now();
    fmt::print("safe_ctime() cost {} microseconds\n", end_time - start_time);

    start_time = steady_clock_now();
    safe_asctime(&local_tm);
    end_time   = steady_clock_now();
    fmt::print("safe_asctime(local_tm) cost {} microseconds\n", end_time - start_time);

    start_time = steady_clock_now();
    safe_asctime(&gm_tm);
    end_time   = steady_clock_now();
    fmt::print("safe_asctime(gm_tm) cost {} microseconds\n", end_time - start_time);

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}

void test_memory_and_hex_convert(void)
{
    using namespace common;

    time_t cur_timestamp = system_clock_now<std::chrono::seconds>();
    fmt::print("Current timestamp={}\n", cur_timestamp);

    tm          local_tm     = safe_localtime(&cur_timestamp);
    std::string local_tm_str = safe_asctime(&local_tm);
    fmt::print("local_tm={}", local_tm_str);

    std::string hex_string_upper;
    abort_assert(convert_memory_to_hex_string(&local_tm, sizeof(local_tm), hex_string_upper, true));

    std::string hex_string_lower;
    abort_assert(convert_memory_to_hex_string(&local_tm, sizeof(local_tm), hex_string_lower, false));

    fmt::print("hex_string_upper={}\nhex_string_lower={}\n", hex_string_upper, hex_string_lower);

    std::string local_tm_upper_str;
    {
        tm local_tm_upper{};
        abort_assert(convert_hex_string_to_memory(hex_string_upper, &local_tm_upper, sizeof(local_tm_upper)));

        local_tm_upper_str = safe_asctime(&local_tm_upper);
        abort_assert(local_tm_str == local_tm_upper_str);
        fmt::print("local_tm_upper={}", local_tm_upper_str);
    }

    std::string local_tm_lower_str;
    {
        tm local_tm_lower{};
        abort_assert(convert_hex_string_to_memory(hex_string_lower, &local_tm_lower, sizeof(local_tm_lower)));

        local_tm_lower_str = safe_asctime(&local_tm_lower);
        abort_assert(local_tm_str == local_tm_lower_str);
        fmt::print("local_tm_upper={}", local_tm_lower_str);
    }
    
    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}

void test_protobuf(void)
{
    CommonInfo info{true, 1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890};

    TestMsg::CommonInfo testInfo;
    testInfo.set_test_bool(info.test_bool);
    testInfo.set_test_float(info.test_float);
    testInfo.set_test_double(info.test_double);
    testInfo.set_test_string(info.test_string);
    testInfo.set_test_int32(info.test_int32);
    testInfo.set_test_sint32(info.test_sint32);
    testInfo.set_test_uint32(info.test_uint32);
    testInfo.set_test_int64(info.test_int64);
    testInfo.set_test_sint64(info.test_sint64);
    testInfo.set_test_uint64(info.test_uint64);
}

void test_pair_tuple_tie(void)
{
    // pair
    {
        std::pair<std::string, int> p1{"p1", 1};
        fmt::print("[{}, {}]\n", p1.first, p1.second);

        std::pair<std::string, int> p2("p2", 2);
        fmt::print("[{}, {}]\n", p2.first, p2.second);

        auto p3 = std::make_pair<std::string, int>("p3", 3);
        fmt::print("[{}, {}]\n", p3.first, p3.second);
    }

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}