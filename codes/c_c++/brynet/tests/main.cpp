#include "util/brynet.h"
#include "util/quill.h"
#include "util/stack_trace.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    core::stack_trace::initialize();

    testing::InitGoogleTest(&argc, argv);

    const auto ret = RUN_ALL_TESTS();

    core::stack_trace::uninitialize();

    return ret;
}