#include "test_misc.h"
#include "util/time.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"

void test_safe_time(void)
{
    using namespace ljn;

    time_t cur_timestamp = system_clock_now<std::chrono::seconds>();
    fmt::print("Current timestamp={}\n", cur_timestamp);
    fmt::print("Call safe_ctime() is:{}\n", safe_ctime(&cur_timestamp));

    tm localTM{};
    ljn::safe_localtime(&cur_timestamp, &localTM);
    fmt::print("Call safe_localtime() and safe_asctime() is:{}\n", safe_asctime(&localTM));

    tm gmTM{};
    ljn::safe_gmtime(&cur_timestamp, &gmTM);
    fmt::print("Call safe_gmtime() and safe_asctime() is:{}\n", safe_asctime(&gmTM));

    fmt::print(fg(fmt::color::green), "{}() success!\n", __func__);
}