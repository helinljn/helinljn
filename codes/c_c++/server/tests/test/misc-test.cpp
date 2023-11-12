#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/poco.h"
#include "util/singleton.hpp"
#include "util/stack_trace.h"
#include "hook/pelf_hook.h"
#include "hook/inject_hook.h"
#include "Poco/SharedLibrary.h"
#include "bar.h"
#include "foo.h"
#include "fooex.h"

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
    ASSERT_TRUE(test_bar(1000) == "test_bar(1000)");
    ASSERT_TRUE(hotfix_test_bar(1000) == "hotfix_test_bar(1000)");

    common::pelf_hook hook;
    ASSERT_TRUE(hook.load(nullptr));

    void* newfaddr = reinterpret_cast<void*>(&hotfix_test_bar);
#if POCO_OS == POCO_OS_WINDOWS_NT
    ASSERT_TRUE(hook.replace("?test_bar@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z", newfaddr));
#else
    ASSERT_TRUE(hook.replace("_Z8test_barB5cxx11i", newfaddr));
#endif

    ASSERT_TRUE(test_bar(1000) == "hotfix_test_bar(1000)");
    ASSERT_TRUE(hotfix_test_bar(1000) == "hotfix_test_bar(1000)");
}

GTEST_TEST(MiscTest, PELFHookMemberFunction)
{
    bar tb;
    ASSERT_TRUE(tb.func1("test") == "bar::func1(test)");
    ASSERT_TRUE(tb.func2("test") == "bar::func2(test)");

    common::pelf_hook hook;
    ASSERT_TRUE(hook.load(nullptr));

#if POCO_OS == POCO_OS_WINDOWS_NT
    decltype(&hotfix_bar::hotfix_func1) memfaddr1 = &hotfix_bar::hotfix_func1;
    void* newfaddr = *reinterpret_cast<void**>(&memfaddr1);
    ASSERT_TRUE(hook.replace("?func1@bar@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", newfaddr));

    ASSERT_TRUE(hook.reload(nullptr));

    decltype(&hotfix_bar::hotfix_func2) memfaddr2 = &hotfix_bar::hotfix_func2;
    newfaddr = *reinterpret_cast<void**>(&memfaddr2);
    ASSERT_TRUE(hook.replace("?func2@bar@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", newfaddr));
#else
    decltype(&hotfix_bar::hotfix_func1) memfaddr1 = &hotfix_bar::hotfix_func1;
    void* newfaddr = *reinterpret_cast<void**>(&memfaddr1);
    ASSERT_TRUE(hook.replace("_ZNK3bar5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", newfaddr));

    ASSERT_TRUE(hook.reload(nullptr));

    decltype(&hotfix_bar::hotfix_func2) memfaddr2 = &hotfix_bar::hotfix_func2;
    newfaddr = *reinterpret_cast<void**>(&memfaddr2);
    ASSERT_TRUE(hook.replace("_ZN3bar5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", newfaddr));
#endif

    ASSERT_TRUE(tb.func1("test") == "hotfix_bar::hotfix_func1(test)");
    ASSERT_TRUE(tb.func2("test") == "hotfix_bar::hotfix_func2(test)");
}

GTEST_TEST(MiscTest, InjectHookFunctionInShared)
{
    Poco::SharedLibrary foolib;
#if POCO_OS == POCO_OS_WINDOWS_NT
    foolib.load("libfoo.dll");
    ASSERT_TRUE(foolib.isLoaded());
#else
    foolib.load("libfoo.so");
    ASSERT_TRUE(foolib.isLoaded());
#endif

    // hook function
    {
        using foofunc = std::string (*)(int32_t);
        void* symbol1 = nullptr;
        void* symbol2 = nullptr;
#if POCO_OS == POCO_OS_WINDOWS_NT
        symbol1 = foolib.getSymbol("?test_foo@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
        symbol2 = foolib.getSymbol("?hotfix_test_foo@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
#else
        symbol1 = foolib.getSymbol("_Z8test_fooB5cxx11i");
        symbol2 = foolib.getSymbol("_Z15hotfix_test_fooB5cxx11i");
#endif

        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol1)(1000) == "test_foo(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol2)(1000) == "hotfix_test_foo(1000)");

        common::inject_hook hook;
        ASSERT_TRUE(hook.load());

        ASSERT_TRUE(hook.replace(symbol1, symbol2));

        ASSERT_TRUE(hook.install());
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol1)(1000) == "hotfix_test_foo(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol2)(1000) == "hotfix_test_foo(1000)");

        ASSERT_TRUE(hook.uninstall());
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol1)(1000) == "test_foo(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol2)(1000) == "hotfix_test_foo(1000)");
    }

    // hook member function
    {
        foo tf;
        ASSERT_TRUE(tf.func1("1000") == "foo::func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "foo::func2(1000)");

        void* symbol1 = nullptr;
        void* symbol2 = nullptr;
#if POCO_OS == POCO_OS_WINDOWS_NT
        symbol1 = foolib.getSymbol("?hotfix_func1@hotfix_foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
        symbol2 = foolib.getSymbol("?hotfix_func2@hotfix_foo@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
#else
        symbol1 = foolib.getSymbol("_ZNK10hotfix_foo12hotfix_func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
        symbol2 = foolib.getSymbol("_ZN10hotfix_foo12hotfix_func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
#endif

        common::inject_hook hook;
        ASSERT_TRUE(hook.load());

        decltype(&foo::func1) memfaddr1 = &foo::func1;
        void* oldfaddr1 = *reinterpret_cast<void**>(&memfaddr1);
        ASSERT_TRUE(hook.replace(oldfaddr1, symbol1));

        decltype(&foo::func2) memfaddr2 = &foo::func2;
        void* oldfaddr2 = *reinterpret_cast<void**>(&memfaddr2);
        ASSERT_TRUE(hook.replace(oldfaddr2, symbol2));

        ASSERT_TRUE(hook.install());
        ASSERT_TRUE(tf.func1("1000") == "hotfix_foo::hotfix_func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "hotfix_foo::hotfix_func2(1000)");

        ASSERT_TRUE(hook.uninstall());
        ASSERT_TRUE(tf.func1("1000") == "foo::func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "foo::func2(1000)");
    }
}

GTEST_TEST(MiscTest, InjectHookFunctionInExecutable)
{
    Poco::SharedLibrary foolib;
#if POCO_OS == POCO_OS_WINDOWS_NT
    foolib.load("libfoo.dll");
    ASSERT_TRUE(foolib.isLoaded());
#else
    foolib.load("libfoo.so");
    ASSERT_TRUE(foolib.isLoaded());
#endif

    // hook function
    {
        using foofunc = std::string (*)(int32_t);
#if POCO_OS == POCO_OS_WINDOWS_NT
        void* symbol = foolib.getSymbol("?hotfix_test_foo@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
#else
        void* symbol = foolib.getSymbol("_Z15hotfix_test_fooB5cxx11i");
#endif

        ASSERT_TRUE(test_fooex(1000) == "test_fooex(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol)(1000) == "hotfix_test_foo(1000)");

        common::inject_hook hook;
        ASSERT_TRUE(hook.load());

        ASSERT_TRUE(hook.replace(reinterpret_cast<void*>(&test_fooex), symbol));

        ASSERT_TRUE(hook.install());
        ASSERT_TRUE(test_fooex(1000) == "hotfix_test_foo(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol)(1000) == "hotfix_test_foo(1000)");

        ASSERT_TRUE(hook.uninstall());
        ASSERT_TRUE(test_fooex(1000) == "test_fooex(1000)");
        ASSERT_TRUE(reinterpret_cast<foofunc>(symbol)(1000) == "hotfix_test_foo(1000)");
    }

    // hook member function
    {
        fooex tf;
        ASSERT_TRUE(tf.func1("1000") == "fooex::func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "fooex::func2(1000)");

        void* symbol1 = nullptr;
        void* symbol2 = nullptr;
#if POCO_OS == POCO_OS_WINDOWS_NT
        symbol1 = foolib.getSymbol("?hotfix_func1@hotfix_foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
        symbol2 = foolib.getSymbol("?hotfix_func2@hotfix_foo@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
#else
        symbol1 = foolib.getSymbol("_ZNK10hotfix_foo12hotfix_func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
        symbol2 = foolib.getSymbol("_ZN10hotfix_foo12hotfix_func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
#endif

        common::inject_hook hook;
        ASSERT_TRUE(hook.load());

        decltype(&fooex::func1) memfaddr1 = &fooex::func1;
        void* oldfaddr1 = *reinterpret_cast<void**>(&memfaddr1);
        ASSERT_TRUE(hook.replace(oldfaddr1, symbol1));

        decltype(&fooex::func2) memfaddr2 = &fooex::func2;
        void* oldfaddr2 = *reinterpret_cast<void**>(&memfaddr2);
        ASSERT_TRUE(hook.replace(oldfaddr2, symbol2));

        ASSERT_TRUE(hook.install());
        ASSERT_TRUE(tf.func1("1000") == "hotfix_foo::hotfix_func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "hotfix_foo::hotfix_func2(1000)");

        ASSERT_TRUE(hook.uninstall());
        ASSERT_TRUE(tf.func1("1000") == "fooex::func1(1000)");
        ASSERT_TRUE(tf.func2("1000") == "fooex::func2(1000)");
    }
}