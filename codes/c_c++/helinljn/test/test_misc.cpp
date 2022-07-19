#include "test_misc.h"
#include "util/common.h"
#include "util/time.h"

void test_safe_time(void)
{
    using namespace ljn;

    time_t cur_timestamp = system_clock_now<std::chrono::seconds>();
    fmt::print("Current timestamp={}\n", cur_timestamp);
    fmt::print("Call safe_ctime() is:{}\n", safe_ctime(&cur_timestamp));

    tm local_tm = ljn::safe_localtime(&cur_timestamp);
    fmt::print("Call safe_localtime() and safe_asctime() is:{}\n", safe_asctime(&local_tm));

    tm gm_tm = ljn::safe_gmtime(&cur_timestamp);
    fmt::print("Call safe_gmtime() and safe_asctime() is:{}\n", safe_asctime(&gm_tm));

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}