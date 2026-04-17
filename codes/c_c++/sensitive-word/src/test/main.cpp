#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <cstdlib>

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.run();

    return EXIT_SUCCESS;
}