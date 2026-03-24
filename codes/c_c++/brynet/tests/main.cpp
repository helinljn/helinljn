#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "util/brynet.h"
#include "util/stack_trace.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #include <Windows.h>
#endif // defined(CORE_PLATFORM_WINDOWS)

int main(int argc, char** argv)
{
#if defined(CORE_PLATFORM_WINDOWS)
    // Windows下设置控制台编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // defined(CORE_PLATFORM_WINDOWS)

    static_assert(__cplusplus == 201703);

    core::stack_trace::initialize();

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.run();

    core::stack_trace::uninitialize();

    return EXIT_SUCCESS;
}