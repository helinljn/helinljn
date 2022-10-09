#include "test_all.h"

int main(void)
{
    static_assert(201703 == __cplusplus);

    fprintf(stdout, "Hello, I am test project test.\n");

    test_cpp();
    test_libevent();
    test_protocol();

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}