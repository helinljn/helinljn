#include "test_misc.h"
#include "util/common.h"
#include "util/time.h"

#include <cstdint>
#include <limits>
#include <utility>

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