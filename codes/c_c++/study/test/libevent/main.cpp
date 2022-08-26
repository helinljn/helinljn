#include <cstdio>

#include "common.h"
#include "libevent-config.h"

int main(void)
{
    libevent::libevent_init();

    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_LIBEVENT_VALUE_JUST_FOR_TEST);

    time_t      cur_timestamp = common::system_clock_now<std::chrono::seconds>();
    std::string cur_timestr   = common::safe_ctime(&cur_timestamp);
    printf("Hello, I am libevent, current time str=%s\n", cur_timestr.c_str());

    libevent::libevent_destroy();

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}