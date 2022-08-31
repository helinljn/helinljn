#include <cstdio>

#include "asio.hpp"
#include "asio/ssl.hpp"

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_ASIO_VALUE_JUST_FOR_TEST);

    printf("Hello, I am asio project test.\n");

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}