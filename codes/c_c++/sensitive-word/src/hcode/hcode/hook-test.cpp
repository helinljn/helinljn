#include "doctest.h"
#include "core/common.h"
#include "core/symbol_loader.h"
#include "core/light_hook.h"
#include "testa/testa.h"
#include "testb.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#if defined(CORE_PLATFORM_WINDOWS)
    #define LIGHT_HOOK_NOINLINE __declspec(noinline)
#elif defined(CORE_PLATFORM_LINUX)
    #define LIGHT_HOOK_NOINLINE __attribute__((noinline))
#else
    #define LIGHT_HOOK_NOINLINE
#endif

namespace {

volatile int g_light_hook_probe_seed = 7;

extern "C" LIGHT_HOOK_NOINLINE int light_hook_concurrent_func(int value)
{
    volatile int values[8] = {
        value,
        g_light_hook_probe_seed,
        value + 1,
        g_light_hook_probe_seed + 1,
        value + 2,
        g_light_hook_probe_seed + 2,
        value + 3,
        g_light_hook_probe_seed + 3
    };
    return values[0] + values[1] + 1;
}

extern "C" LIGHT_HOOK_NOINLINE int light_hook_concurrent_patch_func(int value)
{
    volatile int values[8] = {
        value,
        g_light_hook_probe_seed,
        value + 1,
        g_light_hook_probe_seed + 1,
        value + 2,
        g_light_hook_probe_seed + 2,
        value + 3,
        g_light_hook_probe_seed + 3
    };
    return values[0] + values[1] + 2;
}

} // namespace

TEST_SUITE("Hook")
{
    TEST_CASE("SymbolLoader")
    {
        // 测试加载可执行文件中的main函数
        {
            core::symbol_loader main_loader;
            CHECK(main_loader.load(""));

            const void* main_addr = main_loader.get_symbol("main");
            CHECK(main_addr != nullptr);
        }

        // 测试加载动态链接库中的函数
        {
            core::symbol_loader lib_loader;
#if defined(CORE_PLATFORM_WINDOWS)
            CHECK(lib_loader.load("core.dll"));

            const void* to_upper_addr = lib_loader.get_symbol("?to_upper@core@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$basic_string_view@DU?$char_traits@D@std@@@3@@Z");
            CHECK(to_upper_addr != nullptr);
#elif defined(CORE_PLATFORM_LINUX)
            CHECK(lib_loader.load("libcore.so"));

            const void* to_upper_addr = lib_loader.get_symbol("_ZN4core8to_upperB5cxx11ESt17basic_string_viewIcSt11char_traitsIcEE");
            CHECK(to_upper_addr != nullptr);
#endif // defined(CORE_PLATFORM_WINDOWS)

            auto func = reinterpret_cast<std::string(*)(std::string_view)>(to_upper_addr);
            CHECK(func != nullptr);
            CHECK(func("hello") == "HELLO");
        }
    }

    TEST_CASE("PatchSharedTestLightHook")
    {
        // Before patch
        {
            CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
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

            // detach shared(放弃所有权，避免析构时卸载补丁库)
            patch_loader.detach();

            // Patch testa_func to patch_testa_func
            auto hook_func = create_hook(func, patch_func);
            CHECK(enable_hook(&hook_func) != 0);

            // Patch testa_base::func1 to patch_testa_base::func1
            auto hook_base_func1 = create_hook(base_func1, patch_base_func1);
            CHECK(enable_hook(&hook_base_func1) != 0);

            // Patch testa::func1 to patch_testa::func1
            auto hook_func1 = create_hook(func1, patch_func1);
            CHECK(enable_hook(&hook_func1) != 0);

            // Patch testa::func2 to patch_testa::func2
            auto hook_func2 = create_hook(func2, patch_func2);
            CHECK(enable_hook(&hook_func2) != 0);

            // Patch testa::func3 to patch_testa::func3
            auto hook_func3 = create_hook(func3, patch_func3);
            CHECK(enable_hook(&hook_func3) != 0);

            // Check output
            CHECK(core::starts_with(testa_func(100), "patch_testa_func"));

            testa_base base;
            CHECK(core::starts_with(base.func1("hello"), "patch_testa_base::func1"));

            testa a;
            CHECK(core::starts_with(a.func1("hello"), "patch_testa::func1"));
            CHECK(core::starts_with(a.func2("hello"), "patch_testa::func2"));
            CHECK(core::starts_with(a.func3("hello"), "patch_testa::func3"));

            testa_base& base_ref = a;
            CHECK(core::starts_with(base_ref.func1("hello"), "patch_testa::func1"));

            // Disable hooks
            CHECK(disable_hook(&hook_func) != 0);
            CHECK(disable_hook(&hook_base_func1) != 0);
            CHECK(disable_hook(&hook_func1) != 0);
            CHECK(disable_hook(&hook_func2) != 0);
            CHECK(disable_hook(&hook_func3) != 0);

            // Check output
            CHECK(core::starts_with(testa_func(100), "testa_func"));
            CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));
            CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            CHECK(core::starts_with(a.func3("hello"), "testa::func3"));
            CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }

        // After patch
        {
            CHECK(core::starts_with(testa_func(100), "testa_func"));

            testa_base base;
            CHECK(core::starts_with(base.func1("hello"), "testa_base::func1"));

            testa a;
            CHECK(core::starts_with(a.func1("hello"), "testa::func1"));
            CHECK(core::starts_with(a.func2("hello"), "testa::func2"));
            CHECK(core::starts_with(a.func3("hello"), "testa::func3"));

            testa_base& base_ref = a;
            CHECK(core::starts_with(base_ref.func1("hello"), "testa::func1"));
        }
    }

    TEST_CASE("PatchExecutableTestLightHook")
    {
        // Before patch
        {
            CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
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

            // Patch testb_func to patch_testb_func
            auto hook_func = create_hook(func, patch_func);
            CHECK(enable_hook(&hook_func) != 0);

            // Patch testb_base::func1 to patch_testb_base::func1
            auto hook_base_func1 = create_hook(base_func1, patch_base_func1);
            CHECK(enable_hook(&hook_base_func1) != 0);

            // Patch testb::func1 to patch_testb::func1
            auto hook_func1 = create_hook(func1, patch_func1);
            CHECK(enable_hook(&hook_func1) != 0);

            // Patch testb::func2 to patch_testb::func2
            auto hook_func2 = create_hook(func2, patch_func2);
            CHECK(enable_hook(&hook_func2) != 0);

            // Patch testb::func3 to patch_testb::func3
            auto hook_func3 = create_hook(func3, patch_func3);
            CHECK(enable_hook(&hook_func3) != 0);

            // Check output
            CHECK(core::starts_with(testb_func(100), "patch_testb_func"));

            testb_base base;
            CHECK(core::starts_with(base.func1("hello"), "patch_testb_base::func1"));

            testb b;
            CHECK(core::starts_with(b.func1("hello"), "patch_testb::func1"));
            CHECK(core::starts_with(b.func2("hello"), "patch_testb::func2"));
            CHECK(core::starts_with(b.func3("hello"), "patch_testb::func3"));

            testb_base& base_ref = b;
            CHECK(core::starts_with(base_ref.func1("hello"), "patch_testb::func1"));

            // Disable hooks
            CHECK(disable_hook(&hook_func) != 0);
            CHECK(disable_hook(&hook_base_func1) != 0);
            CHECK(disable_hook(&hook_func1) != 0);
            CHECK(disable_hook(&hook_func2) != 0);
            CHECK(disable_hook(&hook_func3) != 0);

            // Check output
            CHECK(core::starts_with(testb_func(100), "testb_func"));
            CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));
            CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            CHECK(core::starts_with(b.func3("hello"), "testb::func3"));
            CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }

        // After patch
        {
            CHECK(core::starts_with(testb_func(100), "testb_func"));

            testb_base base;
            CHECK(core::starts_with(base.func1("hello"), "testb_base::func1"));

            testb b;
            CHECK(core::starts_with(b.func1("hello"), "testb::func1"));
            CHECK(core::starts_with(b.func2("hello"), "testb::func2"));
            CHECK(core::starts_with(b.func3("hello"), "testb::func3"));

            testb_base& base_ref = b;
            CHECK(core::starts_with(base_ref.func1("hello"), "testb::func1"));
        }
    }

    TEST_CASE("ConcurrentPatchLightHook")
    {
        using func_t = int (*)(int);

        auto* original_func = reinterpret_cast<void*>(&light_hook_concurrent_func);
        auto* patch_func    = reinterpret_cast<void*>(&light_hook_concurrent_patch_func);
        auto  call_func     = reinterpret_cast<func_t>(original_func);

        auto hook_func = create_hook(original_func, patch_func);
        CHECK(hook_func.bytes_to_copy >= 14);

        std::atomic<bool>        stop{false};
        std::atomic<int>         bad_result_count{0};
        std::vector<std::thread> callers;

        const int input          = 100;
        const int original_value = input + g_light_hook_probe_seed + 1;
        const int patch_value    = input + g_light_hook_probe_seed + 2;

        for (int i = 0; i < 8; ++i)
        {
            callers.emplace_back([&]() {
                while (!stop.load(std::memory_order_acquire))
                {
                    const int result = call_func(input);
                    if (result != original_value && result != patch_value)
                        bad_result_count.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }

        for (int i = 0; i < 20; ++i)
        {
            CHECK(enable_hook(&hook_func) != 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            CHECK(disable_hook(&hook_func) != 0);
        }

        stop.store(true, std::memory_order_release);
        for (auto& caller : callers)
            caller.join();

        CHECK(bad_result_count.load(std::memory_order_acquire) == 0);
        CHECK(call_func(input) == original_value);
    }
}