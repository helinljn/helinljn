#include "doctest.h"
#include "core/common.h"
#include "core/stack_trace.h"
#include "core/symbol_loader.h"
#include "core/light_hook.h"
#include "spdlog/fmt/fmt.h"
#include "testa/testa.h"
#include "testb.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>
#include <initializer_list>

#if defined(CORE_PLATFORM_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #define LIGHT_HOOK_NOINLINE __declspec(noinline)
#elif defined(CORE_PLATFORM_LINUX)
    #include <sys/mman.h>
    #include <unistd.h>
    #define LIGHT_HOOK_NOINLINE __attribute__((noinline))
#else
    #define LIGHT_HOOK_NOINLINE
#endif // defined(CORE_PLATFORM_WINDOWS)

LIGHT_HOOK_NOINLINE std::string stack_trace_test_probe()
{
    return core::stack_trace().to_string();
}

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

extern "C" LIGHT_HOOK_NOINLINE int light_hook_machine_patch_func(int value)
{
    return value + 1000;
}

class executable_code
{
public:
    explicit executable_code(size_t size)
        : size_(size)
    {
#if defined(CORE_PLATFORM_WINDOWS)
        data_ = static_cast<unsigned char*>(VirtualAlloc(nullptr, size_, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
#elif defined(CORE_PLATFORM_LINUX)
        void* memory = mmap(nullptr, size_, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        data_        = memory == MAP_FAILED ? nullptr : static_cast<unsigned char*>(memory);
#endif // defined(CORE_PLATFORM_WINDOWS)

        if (data_)
            std::memset(data_, 0xCC, size_);
    }

    executable_code(const executable_code&) = delete;
    executable_code& operator=(const executable_code&) = delete;

    ~executable_code()
    {
        if (!data_)
            return;

#if defined(CORE_PLATFORM_WINDOWS)
        VirtualFree(data_, 0, MEM_RELEASE);
#elif defined(CORE_PLATFORM_LINUX)
        munmap(data_, size_);
#endif // defined(CORE_PLATFORM_WINDOWS)
    }

    [[nodiscard]] bool valid() const
    {
        return data_ != nullptr;
    }

    [[nodiscard]] void* data() const
    {
        return data_;
    }

    void write(size_t offset, std::initializer_list<unsigned char> bytes)
    {
        std::memcpy(data_ + offset, bytes.begin(), bytes.size());
    }

    void write_i32(size_t offset, int32_t value)
    {
        std::memcpy(data_ + offset, &value, sizeof(value));
    }

    void flush()
    {
#if defined(CORE_PLATFORM_WINDOWS)
        FlushInstructionCache(GetCurrentProcess(), data_, size_);
#elif defined(CORE_PLATFORM_LINUX)
        __builtin___clear_cache(reinterpret_cast<char*>(data_), reinterpret_cast<char*>(data_) + size_);
#endif // defined(CORE_PLATFORM_WINDOWS)
    }

private:
    unsigned char* data_{nullptr};
    size_t         size_{0};
};

void write_arg_to_eax_add(executable_code& code, size_t offset)
{
#if defined(CORE_PLATFORM_WINDOWS)
    code.write(offset, {0x01, 0xC8});  // add eax, ecx
#elif defined(CORE_PLATFORM_LINUX)
    code.write(offset, {0x01, 0xF8});  // add eax, edi
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void write_arg_to_eax_mov(executable_code& code, size_t offset)
{
#if defined(CORE_PLATFORM_WINDOWS)
    code.write(offset, {0x8B, 0xC1});  // mov eax, ecx
#elif defined(CORE_PLATFORM_LINUX)
    code.write(offset, {0x89, 0xF8});  // mov eax, edi
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void write_arg_test(executable_code& code, size_t offset)
{
#if defined(CORE_PLATFORM_WINDOWS)
    code.write(offset, {0x85, 0xC9});  // test ecx, ecx
#elif defined(CORE_PLATFORM_LINUX)
    code.write(offset, {0x85, 0xFF});  // test edi, edi
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void write_nops(executable_code& code, size_t offset, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        code.write(offset + i, {0x90});
}

} // namespace

TEST_SUITE("Hook")
{
    TEST_CASE("stack_trace")
    {
        const auto str = stack_trace_test_probe();
        CHECK(!str.empty());
        CHECK(core::contains(str, "stack_trace_test_probe"));
        fmt::print("{}", str);
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

    TEST_CASE("LightHookRelocatesGeneratedX64Instructions")
    {
        using func_t = int (*)(int);

        SUBCASE("RIP relative load")
        {
            executable_code code(4096);
            REQUIRE(code.valid());

            code.write(0, {0x8B, 0x05, 0x00, 0x00, 0x00, 0x00});  // mov eax, [rip+disp32]
            code.write_i32(2, 32 - 6);
            write_arg_to_eax_add(code, 6);
            write_nops(code, 8, 6);
            code.write(14, {0xC3});
            code.write_i32(32, 17);
            code.flush();

            auto call_func = reinterpret_cast<func_t>(code.data());
            CHECK(call_func(5) == 22);

            auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
            CHECK(hook_func.bytes_to_copy >= 14);
            REQUIRE(enable_hook(&hook_func) != 0);

            auto trampoline = reinterpret_cast<func_t>(hook_func.trampoline);
            CHECK(call_func(5) == 1005);
            CHECK(trampoline(5) == 22);

            CHECK(disable_hook(&hook_func) != 0);
            CHECK(call_func(5) == 22);
        }

        SUBCASE("relative call")
        {
            executable_code code(4096);
            REQUIRE(code.valid());

            code.write(0, {0xE8, 0x00, 0x00, 0x00, 0x00});
            code.write_i32(1, 32 - 5);
            write_arg_to_eax_add(code, 5);
            write_nops(code, 7, 7);
            code.write(14, {0xC3});
            code.write(32, {0xB8, 0x0B, 0x00, 0x00, 0x00, 0xC3});  // mov eax, 11; ret
            code.flush();

            auto call_func = reinterpret_cast<func_t>(code.data());
            CHECK(call_func(5) == 16);

            auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
            CHECK(hook_func.bytes_to_copy >= 14);
            REQUIRE(enable_hook(&hook_func) != 0);

            auto trampoline = reinterpret_cast<func_t>(hook_func.trampoline);
            CHECK(call_func(5) == 1005);
            CHECK(trampoline(5) == 16);

            CHECK(disable_hook(&hook_func) != 0);
            CHECK(call_func(5) == 16);
        }

        SUBCASE("relative jmp")
        {
            executable_code code(4096);
            REQUIRE(code.valid());

            code.write(0, {0xEB, 0x0C});
            write_nops(code, 2, 12);
            write_arg_to_eax_mov(code, 14);
            code.write(16, {0x83, 0xC0, 0x1E, 0xC3});  // add eax, 30; ret
            code.flush();

            auto call_func = reinterpret_cast<func_t>(code.data());
            CHECK(call_func(5) == 35);

            auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
            CHECK(hook_func.bytes_to_copy >= 14);
            REQUIRE(enable_hook(&hook_func) != 0);

            auto trampoline = reinterpret_cast<func_t>(hook_func.trampoline);
            CHECK(call_func(5) == 1005);
            CHECK(trampoline(5) == 35);

            CHECK(disable_hook(&hook_func) != 0);
            CHECK(call_func(5) == 35);
        }

        SUBCASE("relative jcc")
        {
            executable_code code(4096);
            REQUIRE(code.valid());

            write_arg_test(code, 0);
            code.write(2, {0x7F, 0x0A});                                  // jg +10
            code.write(4, {0xB8, 0xFB, 0xFF, 0xFF, 0xFF, 0xC3});          // mov eax, -5; ret
            write_nops(code, 10, 4);
            write_arg_to_eax_mov(code, 14);
            code.write(16, {0x83, 0xC0, 0x40, 0xC3});                    // add eax, 64; ret
            code.flush();

            auto call_func = reinterpret_cast<func_t>(code.data());
            CHECK(call_func(5) == 69);
            CHECK(call_func(-1) == -5);

            auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
            CHECK(hook_func.bytes_to_copy >= 14);
            REQUIRE(enable_hook(&hook_func) != 0);

            auto trampoline = reinterpret_cast<func_t>(hook_func.trampoline);
            CHECK(call_func(5) == 1005);
            CHECK(call_func(-1) == 999);
            CHECK(trampoline(5) == 69);
            CHECK(trampoline(-1) == -5);

            CHECK(disable_hook(&hook_func) != 0);
            CHECK(call_func(5) == 69);
            CHECK(call_func(-1) == -5);
        }
    }

    TEST_CASE("LightHookIdempotentEnableDisable")
    {
        using func_t = int (*)(int);

        executable_code code(4096);
        REQUIRE(code.valid());

        write_arg_to_eax_mov(code, 0);
        code.write(2, {0x83, 0xC0, 0x07});
        write_nops(code, 5, 9);
        code.write(14, {0xC3});
        code.flush();

        auto call_func = reinterpret_cast<func_t>(code.data());
        CHECK(call_func(5) == 12);

        auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
        REQUIRE(enable_hook(&hook_func) != 0);
        void* trampoline = hook_func.trampoline;

        CHECK(call_func(5) == 1005);
        CHECK(enable_hook(&hook_func) != 0);
        CHECK(hook_func.trampoline == trampoline);
        CHECK(call_func(5) == 1005);

        CHECK(disable_hook(&hook_func) != 0);
        CHECK(call_func(5) == 12);
        CHECK(hook_func.trampoline == trampoline);

        CHECK(enable_hook(&hook_func) != 0);
        CHECK(hook_func.trampoline == trampoline);
        CHECK(call_func(5) == 1005);

        CHECK(disable_hook(&hook_func) != 0);
        CHECK(call_func(5) == 12);
        CHECK(hook_func.trampoline == trampoline);
        CHECK(disable_hook(&hook_func) != 0);
        CHECK(call_func(5) == 12);
    }

    TEST_CASE("LightHookUnsupportedInstructionLeavesEntryUnchanged")
    {
        executable_code code(4096);
        REQUIRE(code.valid());

        code.write(0, {0xC5, 0xF8, 0x77, 0xC3});  // VEX prefix; unsupported by the lightweight decoder.
        write_nops(code, 4, 12);
        code.flush();

        unsigned char original[16];
        std::memcpy(original, code.data(), sizeof(original));

        auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
        CHECK(hook_func.bytes_to_copy == 0);
        CHECK(enable_hook(&hook_func) == 0);
        CHECK(std::memcmp(original, code.data(), sizeof(original)) == 0);
        CHECK(hook_func.trampoline == nullptr);
        CHECK(hook_func.enabled == 0);
    }

    TEST_CASE("LightHookUnsupportedRelativeLoopLeavesEntryUnchanged")
    {
        executable_code code(4096);
        REQUIRE(code.valid());

        code.write(0, {0xE3, 0x0C});  // jrcxz +12; unsupported relative control flow
        write_nops(code, 2, 12);
        write_arg_to_eax_mov(code, 14);
        code.write(16, {0xC3});
        code.flush();

        unsigned char original[16];
        std::memcpy(original, code.data(), sizeof(original));

        auto hook_func = create_hook(code.data(), reinterpret_cast<void*>(&light_hook_machine_patch_func));
        CHECK(hook_func.bytes_to_copy == 0);
        CHECK(enable_hook(&hook_func) == 0);
        CHECK(std::memcmp(original, code.data(), sizeof(original)) == 0);
        CHECK(hook_func.trampoline == nullptr);
        CHECK(hook_func.enabled == 0);
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

        constexpr int k_caller_thread_count = 4;
        for (int i = 0; i < k_caller_thread_count; ++i)
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

        constexpr int k_patch_round_count = 5;
        for (int i = 0; i < k_patch_round_count; ++i)
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