#include "test_all.h"

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_COMMON_VALUE_JUST_FOR_TEST);

    fprintf(stdout, "Hello, I am common project test.\n");

    test_cpp();
    test_libevent();
    test_protocol();

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}