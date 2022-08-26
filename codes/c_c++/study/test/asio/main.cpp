#include <cstdio>

#include "util/common.h"
#include "asio.hpp"
#include "asio/ssl.hpp"

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_ASIO_VALUE_JUST_FOR_TEST);

    time_t      cur_timestamp = common::system_clock_now<std::chrono::seconds>();
    std::string cur_timestr   = common::safe_ctime(&cur_timestamp);
    printf("Hello, I am asio, current time str=%s\n", cur_timestr.c_str());

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}