#include "doctest/doctest.h"
#include "util/file.h"
#include "fmt/format.h"

DOCTEST_TEST_SUITE("File")
{
    DOCTEST_TEST_CASE("FileOperations")
    {
        // 测试目录操作（注意：这些测试可能会在不同环境下有不同结果）
        // 测试 access_exists 函数
        DOCTEST_CHECK(core::access_exists(".")); // 当前目录应该存在

        // 测试 access_read 函数
        DOCTEST_CHECK(core::access_read(".")); // 当前目录应该可读

        // 测试 access_write 函数
        DOCTEST_CHECK(core::access_write(".")); // 当前目录应该可写
    }

    DOCTEST_TEST_CASE("PathOperations")
    {
        // 测试 get_exepath 函数
        char exepath_buf[2048];
        uint32_t exepath_len = sizeof(exepath_buf);
        DOCTEST_CHECK(core::get_exepath(exepath_buf, &exepath_len));
        DOCTEST_CHECK(exepath_len > 0);

        std::string exepath_str = core::get_exepath();
        DOCTEST_CHECK(!exepath_str.empty());

        // 测试 get_exedir 函数
        char exedir_buf[2048];
        uint32_t exedir_len = sizeof(exedir_buf);
        DOCTEST_CHECK(core::get_exedir(exedir_buf, &exedir_len));
        DOCTEST_CHECK(exedir_len > 0);

        std::string exedir_str = core::get_exedir();
        DOCTEST_CHECK(!exedir_str.empty());

        fmt::print("exepath: {}, exedir: {}\n", exepath_str, exedir_str);
    }
}