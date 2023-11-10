#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/poco.h"
#include "util/singleton.hpp"
#include "util/stack_trace.h"
#include "util/pelfhook.h"
#include "hotfix.h"

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

GTEST_TEST(MiscTest, PELFHookFunction)
{
    ASSERT_TRUE(test_func(1000) == "test_func(1000)");
    ASSERT_TRUE(hotfix_test_func(1000) == "hotfix_test_func(1000)");

    common::pelfhook hook;
    ASSERT_TRUE(hook.load(nullptr));

    void* newfaddr = reinterpret_cast<void*>(&hotfix_test_func);
#if POCO_OS == POCO_OS_WINDOWS_NT
    ASSERT_TRUE(hook.replace("?test_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z", newfaddr));
#else
    ASSERT_TRUE(hook.replace("_Z9test_funcB5cxx11i", newfaddr));
#endif

    ASSERT_TRUE(test_func(1000) == "hotfix_test_func(1000)");
    ASSERT_TRUE(hotfix_test_func(1000) == "hotfix_test_func(1000)");
}

GTEST_TEST(MiscTest, PELFHookMemberFunction)
{
    foo tf;
    ASSERT_TRUE(tf.func1("test") == "foo::func1(test)");
    ASSERT_TRUE(tf.func2("test") == "foo::func2(test)");

    common::pelfhook hook;
    ASSERT_TRUE(hook.load(nullptr));

#if POCO_OS == POCO_OS_WINDOWS_NT
    decltype(&hotfix_foo::hotfix_func1) memfaddr1 = &hotfix_foo::hotfix_func1;
    void* newfaddr = *reinterpret_cast<void**>(&memfaddr1);
    ASSERT_TRUE(hook.replace("?func1@foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", newfaddr));

    ASSERT_TRUE(hook.reload(nullptr));

    decltype(&hotfix_foo::hotfix_func2) memfaddr2 = &hotfix_foo::hotfix_func2;
    newfaddr = *reinterpret_cast<void**>(&memfaddr2);
    ASSERT_TRUE(hook.replace("?func2@foo@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", newfaddr));
#else
    decltype(&hotfix_foo::hotfix_func1) memfaddr1 = &hotfix_foo::hotfix_func1;
    void* newfaddr = *reinterpret_cast<void**>(&memfaddr1);
    ASSERT_TRUE(hook.replace("_ZNK3foo5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", newfaddr));

    ASSERT_TRUE(hook.reload(nullptr));

    decltype(&hotfix_foo::hotfix_func2) memfaddr2 = &hotfix_foo::hotfix_func2;
    newfaddr = *reinterpret_cast<void**>(&memfaddr2);
    ASSERT_TRUE(hook.replace("_ZN3foo5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", newfaddr));
#endif

    ASSERT_TRUE(tf.func1("test") == "hotfix_foo::hotfix_func1(test)");
    ASSERT_TRUE(tf.func2("test") == "hotfix_foo::hotfix_func2(test)");
}