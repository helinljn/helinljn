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
    Poco::SharedLibrary barlib;
    barlib.load(std::string{"libbar"} + Poco::SharedLibrary::suffix());
    ASSERT_TRUE(barlib.isLoaded());

    ASSERT_TRUE(test_bar(1000) == "test_bar(1000)");
    ASSERT_TRUE(patch_test_bar(1000) == "patch_test_bar(1000)");

    common::pelf_hook hook;
    ASSERT_TRUE(hook.load(nullptr));

#if POCO_OS == POCO_OS_WINDOWS_NT
    void* newfaddr = barlib.getSymbol("?patch_test_bar@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
    ASSERT_TRUE(hook.replace("?test_bar@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z", newfaddr));
#else
    void* newfaddr = barlib.getSymbol("_Z14patch_test_barB5cxx11i");
    ASSERT_TRUE(hook.replace("_Z8test_barB5cxx11i", newfaddr));
#endif

    ASSERT_TRUE(test_bar(1000) == "patch_test_bar(1000)");
    ASSERT_TRUE(patch_test_bar(1000) == "patch_test_bar(1000)");

    barlib.unload();
    ASSERT_TRUE(!barlib.isLoaded());
}

GTEST_TEST(MiscTest, PELFHookMemberFunction)
{
    Poco::SharedLibrary barlib;
    barlib.load(std::string{"libbar"} + Poco::SharedLibrary::suffix());
    ASSERT_TRUE(barlib.isLoaded());

    bar tb;
    ASSERT_TRUE(tb.func1("test") == "bar::func1(test)");
    ASSERT_TRUE(tb.func2("test") == "bar::func2(test)");

    common::pelf_hook hook;
    ASSERT_TRUE(hook.load(nullptr));

#if POCO_OS == POCO_OS_WINDOWS_NT
    void* symbol1 = barlib.getSymbol("?patch_func1@patch_bar@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol2 = barlib.getSymbol("?patch_func2@patch_bar@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");

    ASSERT_TRUE(hook.replace("?func1@bar@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", symbol1));
    ASSERT_TRUE(hook.replace("?func2@bar@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z", symbol2));
#else
    void* symbol1 = barlib.getSymbol("_ZNK9patch_bar11patch_func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol2 = barlib.getSymbol("_ZN9patch_bar11patch_func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");

    ASSERT_TRUE(hook.replace("_ZNK3bar5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", symbol1));
    ASSERT_TRUE(hook.replace("_ZN3bar5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", symbol2));
#endif

    ASSERT_TRUE(tb.func1("test") == "patch_bar::patch_func1(test)");
    ASSERT_TRUE(tb.func2("test") == "patch_bar::patch_func2(test)");

    barlib.unload();
    ASSERT_TRUE(!barlib.isLoaded());
}

GTEST_TEST(MiscTest, InjectHookFunction)
{
    Poco::SharedLibrary foolib;
    foolib.load(std::string{"libfoo"} + Poco::SharedLibrary::suffix());
    ASSERT_TRUE(foolib.isLoaded());

    ASSERT_TRUE(test_foo(1000) == "test_foo(1000)");
    ASSERT_TRUE(patch_test_foo(1000) == "patch_test_foo(1000)");

    common::inject_hook hook;
    ASSERT_TRUE(hook.load());

#if POCO_OS == POCO_OS_WINDOWS_NT
    void* symbol1 = foolib.getSymbol("?test_foo@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
    void* symbol2 = foolib.getSymbol("?patch_test_foo@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
#else
    void* symbol1 = foolib.getSymbol("_Z8test_fooB5cxx11i");
    void* symbol2 = foolib.getSymbol("_Z14patch_test_fooB5cxx11i");
#endif

    ASSERT_TRUE(hook.replace(symbol1, symbol2));

    ASSERT_TRUE(hook.install());
    ASSERT_TRUE(test_foo(1000) == "patch_test_foo(1000)");
    ASSERT_TRUE(patch_test_foo(1000) == "patch_test_foo(1000)");

    ASSERT_TRUE(hook.uninstall());
    ASSERT_TRUE(test_foo(1000) == "test_foo(1000)");
    ASSERT_TRUE(patch_test_foo(1000) == "patch_test_foo(1000)");

    foolib.unload();
    ASSERT_TRUE(!foolib.isLoaded());
}

GTEST_TEST(MiscTest, InjectHookMemberFunction)
{
    Poco::SharedLibrary foolib;
    foolib.load(std::string{"libfoo"} + Poco::SharedLibrary::suffix());
    ASSERT_TRUE(foolib.isLoaded());

    foo       tf;
    foo_base& rf = tf;
    ASSERT_TRUE(rf.func1("1000") == "foo::func1(1000)");
    ASSERT_TRUE(tf.func2("1000") == "foo::func2(1000)");
    ASSERT_TRUE(tf.func3("1000") == "foo::func3(1000)");

    common::inject_hook hook;
    ASSERT_TRUE(hook.load());

#if POCO_OS == POCO_OS_WINDOWS_NT
    void* symbol1 = foolib.getSymbol("?func1@foo@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol2 = foolib.getSymbol("?func2@foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol3 = foolib.getSymbol("?func3@foo@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol4 = foolib.getSymbol("?patch_func1@patch_foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol5 = foolib.getSymbol("?patch_func2@patch_foo@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
    void* symbol6 = foolib.getSymbol("?patch_func3@patch_foo@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
#else
    void* symbol1 = foolib.getSymbol("_ZNK3foo5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol2 = foolib.getSymbol("_ZNK3foo5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol3 = foolib.getSymbol("_ZN3foo5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol4 = foolib.getSymbol("_ZNK9patch_foo11patch_func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol5 = foolib.getSymbol("_ZNK9patch_foo11patch_func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
    void* symbol6 = foolib.getSymbol("_ZN9patch_foo11patch_func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
#endif

    ASSERT_TRUE(hook.replace(symbol1, symbol4));
    ASSERT_TRUE(hook.replace(symbol2, symbol5));
    ASSERT_TRUE(hook.replace(symbol3, symbol6));

    ASSERT_TRUE(hook.install());
    ASSERT_TRUE(rf.func1("1000") == "patch_foo::patch_func1(1000)");
    ASSERT_TRUE(tf.func2("1000") == "patch_foo::patch_func2(1000)");
    ASSERT_TRUE(tf.func3("1000") == "patch_foo::patch_func3(1000)");

    ASSERT_TRUE(hook.uninstall());
    ASSERT_TRUE(rf.func1("1000") == "foo::func1(1000)");
    ASSERT_TRUE(tf.func2("1000") == "foo::func2(1000)");
    ASSERT_TRUE(tf.func3("1000") == "foo::func3(1000)");

    foolib.unload();
    ASSERT_TRUE(!foolib.isLoaded());
}