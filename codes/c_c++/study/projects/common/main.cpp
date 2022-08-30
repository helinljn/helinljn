#include <cstdio>

#include "common.h"
#include "test_all.h"

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_COMMON_VALUE_JUST_FOR_TEST);

    time_t      cur_timestamp = common::system_clock_now<std::chrono::seconds>();
    std::string cur_timestr   = common::safe_ctime(&cur_timestamp);
    printf("Hello, I am common, current time str=%s\n", cur_timestr.c_str());

    test_misc();

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}