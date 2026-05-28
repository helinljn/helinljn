#include "doctest.h"
#include "Poco/String.h"
#include "core/stack_trace.h"
#include "core/symbol_loader.h"
#include "funchook.h"
#include "testa/testa.h"
#include "testb.h"

TEST_SUITE("Hook")
{
    TEST_CASE("stack_trace")
    {
        const auto str = core::stack_trace().to_string();
        CHECK(!str.empty());
        std::printf("%s\n", str.c_str());
    }

    TEST_CASE("SymbolLoader")
    {
        // 测试加载可执行文件中的main函数
        {
            core::symbol_loader main_loader;
            CHECK(main_loader.load(""));

            const void* main_addr = main_loader.get_symbol("main");
            CHECK(main_addr != nullptr);
        }
    }

    TEST_CASE("PatchSharedTestFuncHook")
    {
        // Before patch
        {
            CHECK(Poco::startsWith<std::string>(testa_func(100), "testa_func"));

            testa_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testa_base::func1"));

            testa a;
            CHECK(Poco::startsWith<std::string>(a.func1("hello"), "testa::func1"));
            CHECK(Poco::startsWith<std::string>(a.func2("hello"), "testa::func2"));
            CHECK(Poco::startsWith<std::string>(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testa::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader testa_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            CHECK(patch_loader.load("testpatch.dll"));
            CHECK(testa_loader.load("testa.dll"));

            // testpatch.dll
            void* patch_func = patch_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func3 != nullptr);

            // testa.dll
            void* func = testa_loader.get_symbol("?testa_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("?func1@testa_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("?func1@testa@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("?func2@testa@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("?func3@testa@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            CHECK(patch_loader.load("libtestpatch.so"));
            CHECK(testa_loader.load("libtesta.so"));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testa_funcB5cxx11i");
            CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func3 != nullptr);

            // libtesta.so
            void* func = testa_loader.get_symbol("_Z10testa_funcB5cxx11i");
            CHECK(func != nullptr);

            void* base_func1 = testa_loader.get_symbol("_ZNK10testa_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(base_func1 != nullptr);

            void* func1 = testa_loader.get_symbol("_ZNK5testa5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func1 != nullptr);

            void* func2 = testa_loader.get_symbol("_ZNK5testa5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func2 != nullptr);

            void* func3 = testa_loader.get_symbol("_ZN5testa5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // Create hook handle
            auto hook_handle = funchook_create();
            CHECK(hook_handle != nullptr);

            // Prepare hooks
            CHECK(funchook_prepare(hook_handle, &func, patch_func) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &base_func1, patch_base_func1) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func1, patch_func1) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func2, patch_func2) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func3, patch_func3) == FUNCHOOK_ERROR_SUCCESS);

            // Install hooks
            CHECK(funchook_install(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            CHECK(Poco::startsWith<std::string>(testa_func(100), "patch_testa_func"));

            testa_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "patch_testa_base::func1"));

            testa a;
            CHECK(Poco::startsWith<std::string>(a.func1("hello"), "patch_testa::func1"));
            CHECK(Poco::startsWith<std::string>(a.func2("hello"), "patch_testa::func2"));
            CHECK(Poco::startsWith<std::string>(a.func3("hello"), "patch_testa::func3"));

            testa_base& base_ref = a;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "patch_testa::func1"));

            // Disable hooks
            CHECK(funchook_uninstall(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            CHECK(Poco::startsWith<std::string>(testa_func(100), "testa_func"));
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testa_base::func1"));
            CHECK(Poco::startsWith<std::string>(a.func1("hello"), "testa::func1"));
            CHECK(Poco::startsWith<std::string>(a.func2("hello"), "testa::func2"));
            CHECK(Poco::startsWith<std::string>(a.func3("hello"), "testa::func3"));
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testa::func1"));

            // Destroy hook handle
            CHECK(funchook_destroy(hook_handle) == FUNCHOOK_ERROR_SUCCESS);
            hook_handle = nullptr;
        }

        // After patch
        {
            CHECK(Poco::startsWith<std::string>(testa_func(100), "testa_func"));

            testa_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testa_base::func1"));

            testa a;
            CHECK(Poco::startsWith<std::string>(a.func1("hello"), "testa::func1"));
            CHECK(Poco::startsWith<std::string>(a.func2("hello"), "testa::func2"));
            CHECK(Poco::startsWith<std::string>(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testa::func1"));
        }
    }

    TEST_CASE("PatchExecutableTestFuncHook")
    {
        // Before patch
        {
            CHECK(Poco::startsWith<std::string>(testb_func(100), "testb_func"));

            testb_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testb_base::func1"));

            testb b;
            CHECK(Poco::startsWith<std::string>(b.func1("hello"), "testb::func1"));
            CHECK(Poco::startsWith<std::string>(b.func2("hello"), "testb::func2"));
            CHECK(Poco::startsWith<std::string>(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testb::func1"));
        }

        // Patch
        {
            core::symbol_loader patch_loader;
            core::symbol_loader exe_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            CHECK(patch_loader.load("testpatch.dll"));
            CHECK(exe_loader.load(""));

            // testpatch.dll
            void* patch_func = patch_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(patch_func3 != nullptr);

            // test.exe
            void* func = exe_loader.get_symbol("?testb_func@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z");
            CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("?func1@testb_base@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("?func1@testb@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("?func2@testb@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("?func3@testb@@SA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z");
            CHECK(func3 != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            CHECK(patch_loader.load("libtestpatch.so"));
            CHECK(exe_loader.load(""));

            // libtestpatch.so
            void* patch_func = patch_loader.get_symbol("_Z10testb_funcB5cxx11i");
            CHECK(patch_func != nullptr);

            void* patch_base_func1 = patch_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_base_func1 != nullptr);

            void* patch_func1 = patch_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func1 != nullptr);

            void* patch_func2 = patch_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func2 != nullptr);

            void* patch_func3 = patch_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(patch_func3 != nullptr);

            // test
            void* func = exe_loader.get_symbol("_Z10testb_funcB5cxx11i");
            CHECK(func != nullptr);

            void* base_func1 = exe_loader.get_symbol("_ZNK10testb_base5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(base_func1 != nullptr);

            void* func1 = exe_loader.get_symbol("_ZNK5testb5func1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func1 != nullptr);

            void* func2 = exe_loader.get_symbol("_ZNK5testb5func2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func2 != nullptr);

            void* func3 = exe_loader.get_symbol("_ZN5testb5func3ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
            CHECK(func3 != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            // Create hook handle
            auto hook_handle = funchook_create();
            CHECK(hook_handle != nullptr);

            // Prepare hooks
            CHECK(funchook_prepare(hook_handle, &func, patch_func) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &base_func1, patch_base_func1) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func1, patch_func1) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func2, patch_func2) == FUNCHOOK_ERROR_SUCCESS);
            CHECK(funchook_prepare(hook_handle, &func3, patch_func3) == FUNCHOOK_ERROR_SUCCESS);

            // Install hooks
            CHECK(funchook_install(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            CHECK(Poco::startsWith<std::string>(testb_func(100), "patch_testb_func"));

            testb_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "patch_testb_base::func1"));

            testb b;
            CHECK(Poco::startsWith<std::string>(b.func1("hello"), "patch_testb::func1"));
            CHECK(Poco::startsWith<std::string>(b.func2("hello"), "patch_testb::func2"));
            CHECK(Poco::startsWith<std::string>(b.func3("hello"), "patch_testb::func3"));

            testb_base& base_ref = b;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "patch_testb::func1"));

            // Disable hooks
            CHECK(funchook_uninstall(hook_handle, 0) == FUNCHOOK_ERROR_SUCCESS);

            // Check output
            CHECK(Poco::startsWith<std::string>(testb_func(100), "testb_func"));
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testb_base::func1"));
            CHECK(Poco::startsWith<std::string>(b.func1("hello"), "testb::func1"));
            CHECK(Poco::startsWith<std::string>(b.func2("hello"), "testb::func2"));
            CHECK(Poco::startsWith<std::string>(b.func3("hello"), "testb::func3"));
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testb::func1"));

            // Destroy hook handle
            CHECK(funchook_destroy(hook_handle) == FUNCHOOK_ERROR_SUCCESS);
            hook_handle = nullptr;
        }

        // After patch
        {
            CHECK(Poco::startsWith<std::string>(testb_func(100), "testb_func"));

            testb_base base;
            CHECK(Poco::startsWith<std::string>(base.func1("hello"), "testb_base::func1"));

            testb b;
            CHECK(Poco::startsWith<std::string>(b.func1("hello"), "testb::func1"));
            CHECK(Poco::startsWith<std::string>(b.func2("hello"), "testb::func2"));
            CHECK(Poco::startsWith<std::string>(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            CHECK(Poco::startsWith<std::string>(base_ref.func1("hello"), "testb::func1"));
        }
    }
}
