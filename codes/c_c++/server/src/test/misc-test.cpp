#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/poco.h"
#include "util/singleton.hpp"
#include "util/stack_trace.h"

#include <string>
#include <type_traits>

// Unnamed namespace for internal linkage
namespace {

class SingletonTest : public common::singleton<SingletonTest>
{
public:
    const std::string& GetFile(void) const {return _file;}
    void SetFile(std::string str) {_file = std::move(str);}

    int GetLine(void) const {return _line;}
    void SetLine(const int val) {_line = val;}

private:
    SingletonTest(void)
        : _file()
        , _line(0)
    {
    }

private:
    COMMON_SINGLETON_HELPER;

private:
    std::string _file;
    int         _line;
};

} // unnamed namespace

GTEST_TEST(MiscTest, Pair)
{
    auto& st = SingletonTest::instance();
    ASSERT_TRUE(st.GetFile().empty());
    ASSERT_TRUE(st.GetLine() == 0);

    st.SetFile("hello");
    st.SetLine(10);
    ASSERT_TRUE(st.GetFile() == "hello");
    ASSERT_TRUE(st.GetLine() == 10);
}

GTEST_TEST(MiscTest, StackTrace)
{
    const std::string callstack = common::stack_trace().to_string();
    ASSERT_TRUE(!callstack.empty());

    fmt::print("-- stack trace --\n{}-----------------\n", callstack);
}