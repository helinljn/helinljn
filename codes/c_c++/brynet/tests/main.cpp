#include "util/brynet.h"
#include "util/stack_trace.h"
#include "fmt/format.h"

int main(int argc, char** argv)
{
    std::ignore = argc;
    std::ignore = argv;

    static_assert(__cplusplus == 201703);

    core::stack_trace::initialize();

    const std::string callstack = core::stack_trace().to_string();
    fmt::print("-- stack trace --\n{}-----------------\n", callstack);

    core::stack_trace::uninitialize();

    return 0;
}