#include "doctest.h"
#include "core/common.h"
#include "core/symbol_loader.h"
#include "core/hook.h"
#include "funchook.h"
#include "testa/testa.h"
#include "testb.h"

DOCTEST_TEST_SUITE("Hook")
{
    DOCTEST_TEST_CASE("SymbolLoader")
    {
        // 测试加载可执行文件中的main函数
        {
            core::symbol_loader main_loader;
            DOCTEST_CHECK(main_loader.load(""));

            const void* main_addr = main_loader.get_symbol("main");
            DOCTEST_CHECK(main_addr != nullptr);
        }

        // 测试加载动态链接库中的函数
        {
            core::symbol_loader lib_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            DOCTEST_CHECK(lib_loader.load("libcore.dll"));

            const void* to_upper_addr = lib_loader.get_symbol("?to_upper@core@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$basic_string_view@DU?$char_traits@D@std@@@3@@Z");
            DOCTEST_CHECK(to_upper_addr != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            DOCTEST_CHECK(lib_loader.load("libcore.so"));

            const void* to_upper_addr = lib_loader.get_symbol("_ZN4core8to_upperB5cxx11ESt17basic_string_viewIcSt11char_traitsIcEE");
            DOCTEST_CHECK(to_upper_addr != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            auto func = reinterpret_cast<std::string(*)(std::string_view)>(to_upper_addr);
            DOCTEST_CHECK(func != nullptr);
            DOCTEST_CHECK(func("hello") == "HELLO");
        }
    }

    DOCTEST_TEST_CASE("PatchSharedTestLightHook")
    {
        // Before patch
        {
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader testa_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.dll"));
            DOCTEST_CHECK(testa_loader.load("libtesta.dll"));

            // libtestpatch.dll
            void* patch_func = patch_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // libtesta.dll
            void* func = testa_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.so"));
            DOCTEST_CHECK(testa_loader.load("libtesta.so"));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testa_funcB5cxx11i");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // libtesta.so
            void* func = testa_loader.get_symbol("_Z10testa_funcB5cxx11i");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // detach shared
            patch_loader.detach();

            // Patch testa_func to patch_testa_func
            auto hook_func = CreateHook(func, patch_func);
            DOCTEST_CHECK(EnableHook(&hook_func) != 0);

            // Patch testa_base::func1 to patch_testa_base::func1
            auto hook_base_func1 = CreateHook(base_func1, patch_base_func1);
            DOCTEST_CHECK(EnableHook(&hook_base_func1) != 0);

            // Patch testa::func1 to patch_testa::func1
            auto hook_func1 = CreateHook(func1, patch_func1);
            DOCTEST_CHECK(EnableHook(&hook_func1) != 0);

            // Patch testa::func2 to patch_testa::func2
            auto hook_func2 = CreateHook(func2, patch_func2);
            DOCTEST_CHECK(EnableHook(&hook_func2) != 0);

            // Patch testa::func3 to patch_testa::func3
            auto hook_func3 = CreateHook(func3, patch_func3);
            DOCTEST_CHECK(EnableHook(&hook_func3) != 0);

            // Check output
            DOCTEST_CHECK(core::starts_with(testa_func(100), "patch_testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "patch_testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "patch_testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "patch_testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "patch_testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "patch_testa::func1"));

            // Disable hooks
            DOCTEST_CHECK(DisableHook(&hook_func) != 0);
            DOCTEST_CHECK(DisableHook(&hook_base_func1) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func1) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func2) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func3) != 0);

            // Check output
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }

        // After patch
        {
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }
    }

    DOCTEST_TEST_CASE("PatchExecutableTestLightHook")
    {
        // Before patch
        {
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader exe_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.dll"));
            DOCTEST_CHECK(exe_loader.load(""));

            // libtestpatch.dll
            void* patch_func = patch_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // test.exe
            void* func = exe_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.so"));
            DOCTEST_CHECK(exe_loader.load(""));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testb_funcB5cxx11i");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // test
            void* func = exe_loader.get_symbol("_Z10testb_funcB5cxx11i");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // Patch testb_func to patch_testb_func
            auto hook_func = CreateHook(func, patch_func);
            DOCTEST_CHECK(EnableHook(&hook_func) != 0);

            // Patch testb_base::func1 to patch_testb_base::func1
            auto hook_base_func1 = CreateHook(base_func1, patch_base_func1);
            DOCTEST_CHECK(EnableHook(&hook_base_func1) != 0);

            // Patch testb::func1 to patch_testb::func1
            auto hook_func1 = CreateHook(func1, patch_func1);
            DOCTEST_CHECK(EnableHook(&hook_func1) != 0);

            // Patch testb::func2 to patch_testb::func2
            auto hook_func2 = CreateHook(func2, patch_func2);
            DOCTEST_CHECK(EnableHook(&hook_func2) != 0);

            // Patch testb::func3 to patch_testb::func3
            auto hook_func3 = CreateHook(func3, patch_func3);
            DOCTEST_CHECK(EnableHook(&hook_func3) != 0);

            // Check output
            DOCTEST_CHECK(core::starts_with(testb_func(100), "patch_testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "patch_testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "patch_testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "patch_testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "patch_testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "patch_testb::func1"));

            // Disable hooks
            DOCTEST_CHECK(DisableHook(&hook_func) != 0);
            DOCTEST_CHECK(DisableHook(&hook_base_func1) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func1) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func2) != 0);
            DOCTEST_CHECK(DisableHook(&hook_func3) != 0);

            // Check output
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }

        // After patch
        {
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }
    }

    DOCTEST_TEST_CASE("PatchSharedTestFuncHook")
    {
        // Before patch
        {
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader testa_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.dll"));
            DOCTEST_CHECK(testa_loader.load("libtesta.dll"));

            // libtestpatch.dll
            void* patch_func = patch_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // libtesta.dll
            void* func = testa_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.so"));
            DOCTEST_CHECK(testa_loader.load("libtesta.so"));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testa_funcB5cxx11i");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // libtesta.so
            void* func = testa_loader.get_symbol("_Z10testa_funcB5cxx11i");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // Create hook handle
            auto hook_handle = funchook_create();
            DOCTEST_CHECK(hook_handle != nullptr);

            // Prepare hooks
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func, patch_func) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &base_func1, patch_base_func1) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func1, patch_func1) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func2, patch_func2) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func3, patch_func3) == FUNCHOOK_ERROR_SUCCESS);

            // Install hooks
            DOCTEST_CHECK(funchook_install(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            DOCTEST_CHECK(core::starts_with(testa_func(100), "patch_testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "patch_testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "patch_testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "patch_testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "patch_testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "patch_testa::func1"));

            // Disable hooks
            DOCTEST_CHECK(funchook_uninstall(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));

            // Destroy hook handle
            DOCTEST_CHECK(funchook_destroy(hook_handle) == FUNCHOOK_ERROR_SUCCESS);
            hook_handle = nullptr;
        }

        // After patch
        {
            DOCTEST_CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            DOCTEST_CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            DOCTEST_CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            DOCTEST_CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }
    }

    DOCTEST_TEST_CASE("PatchExecutableTestFuncHook")
    {
        // Before patch
        {
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader exe_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.dll"));
            DOCTEST_CHECK(exe_loader.load(""));

            // libtestpatch.dll
            void* patch_func = patch_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // test.exe
            void* func = exe_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            DOCTEST_CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            DOCTEST_CHECK(patch_loader.load("libtestpatch.so"));
            DOCTEST_CHECK(exe_loader.load(""));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testb_funcB5cxx11i");
            DOCTEST_CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(patch_func3 != nullptr);

            // test
            void* func = exe_loader.get_symbol("_Z10testb_funcB5cxx11i");
            DOCTEST_CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            DOCTEST_CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // Create hook handle
            auto hook_handle = funchook_create();
            DOCTEST_CHECK(hook_handle != nullptr);

            // Prepare hooks
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func, patch_func) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &base_func1, patch_base_func1) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func1, patch_func1) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func2, patch_func2) == FUNCHOOK_ERROR_SUCCESS);
            DOCTEST_CHECK(funchook_prepare(hook_handle, &func3, patch_func3) == FUNCHOOK_ERROR_SUCCESS);

            // Install hooks
            DOCTEST_CHECK(funchook_install(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            DOCTEST_CHECK(core::starts_with(testb_func(100), "patch_testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "patch_testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "patch_testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "patch_testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "patch_testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "patch_testb::func1"));

            // Disable hooks
            DOCTEST_CHECK(funchook_uninstall(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));

            // Destroy hook handle
            DOCTEST_CHECK(funchook_destroy(hook_handle) == FUNCHOOK_ERROR_SUCCESS);
            hook_handle = nullptr;
        }

        // After patch
        {
            DOCTEST_CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            DOCTEST_CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            DOCTEST_CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            DOCTEST_CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            DOCTEST_CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            DOCTEST_CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }
    }
}