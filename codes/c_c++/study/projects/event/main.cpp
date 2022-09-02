#include <cstdio>

#include "event.h"
#include "evhttp.h"
#include "evrpc.h"
#include "evdns.h"
#include "evutil.h"
#include "event2/bufferevent_ssl.h"

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_LIBEVENT_VALUE_JUST_FOR_TEST);

    fprintf(stdout, "Hello, I am event project test.\n");

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}