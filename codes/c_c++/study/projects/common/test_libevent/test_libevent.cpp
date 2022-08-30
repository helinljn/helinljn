#include <cstdio>

int main(void)
{
    static_assert(201703 == __cplusplus);
    static_assert(818    == EXEC_LIBEVENT_VALUE_JUST_FOR_TEST);

    printf("Hello, I am libevent project test.\n");

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}