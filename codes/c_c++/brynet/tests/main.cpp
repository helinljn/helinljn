#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "util/brynet.h"
#include "util/stack_trace.h"
#include "fmt/format.h"

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    core::stack_trace::initialize();

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.run();

    core::stack_trace::uninitialize();

    return EXIT_SUCCESS;
}