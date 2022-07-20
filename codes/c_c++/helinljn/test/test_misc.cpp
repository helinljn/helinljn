#include "test_misc.h"
#include "util/common.h"
#include "util/time.h"

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