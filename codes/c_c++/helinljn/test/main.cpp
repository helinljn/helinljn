#include "test/test_all.h"

#include <cstdio>

int main(void)
{
    test_misc();

    printf("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}