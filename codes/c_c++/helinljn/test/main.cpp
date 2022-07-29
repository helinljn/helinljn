#include "util/common.h"
#include "test/test_all.h"

int main(void)
{
    fmt::print(fmt::fg(fmt::color::white), "Hello helinljn!\n");
    fmt::print(fmt::fg(fmt::color::green), "Hello helinljn!\n");
    fmt::print(fmt::fg(fmt::color::red),   "Hello helinljn!\n");

    test_all();

    fmt::print("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}