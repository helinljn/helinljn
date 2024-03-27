#include "brynet.h"
#include "quill.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}