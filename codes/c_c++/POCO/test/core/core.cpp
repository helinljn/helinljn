#include "core.h"
#include "Poco/Poco.h"
#include "Poco/Environment.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Ascii.h"
#include "Poco/FPEnvironment.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/DynamicFactory.h"
#include "Poco/MemoryPool.h"

#include "spdlog/fmt/fmt.h"

#include <memory>

namespace dynamic_factory {

class base_msg
{
public:
    base_msg(void) = default;
    virtual ~base_msg(void) = default;

    base_msg(const base_msg& msg) = delete;
    base_msg(base_msg&& msg) = delete;

    base_msg& operator=(const base_msg& msg) = delete;
    base_msg& operator=(base_msg&& msg) = delete;

    virtual std::string name(void) const = 0;
};

class player_info : public base_msg
{
public:
    player_info(void) = default;
    virtual ~player_info(void) = default;

    std::string name(void) const override
    {
        return "player_info";
    }
};

class friend_info : public base_msg
{
public:
    friend_info(void) = default;
    virtual ~friend_info(void) = default;

    std::string name(void) const override
    {
        return "friend_info";
    }
};

} // namespace dynamic_factory

void test_platform(void)
{
    fmt::print("POCO_OS  : {}\n", POCO_OS);
    fmt::print("POCO_ARCH: {}\n", POCO_ARCH);
}

void test_fixed_length(void)
{
    static_assert(sizeof(Poco::Int8) == 1);
    static_assert(sizeof(Poco::UInt8) == 1);
    static_assert(sizeof(Poco::Int16) == 2);
    static_assert(sizeof(Poco::UInt16) == 2);
    static_assert(sizeof(Poco::Int32) == 4);
    static_assert(sizeof(Poco::UInt32) == 4);
    static_assert(sizeof(Poco::Int64) == 8);
    static_assert(sizeof(Poco::UInt64) == 8);
    static_assert(sizeof(Poco::IntPtr) == 8);
    static_assert(sizeof(Poco::UIntPtr) == 8);

    static_assert(sizeof(int8_t) == 1);
    static_assert(sizeof(uint8_t) == 1);
    static_assert(sizeof(int16_t) == 2);
    static_assert(sizeof(uint16_t) == 2);
    static_assert(sizeof(int32_t) == 4);
    static_assert(sizeof(uint32_t) == 4);
    static_assert(sizeof(int64_t) == 8);
    static_assert(sizeof(uint64_t) == 8);
    static_assert(sizeof(intptr_t) == 8);
    static_assert(sizeof(uintptr_t) == 8);

    static_assert(sizeof(signed char) == 1);
    static_assert(sizeof(unsigned char) == 1);

    static_assert(sizeof(signed short) == 2);
    static_assert(sizeof(unsigned short) == 2);

    static_assert(sizeof(signed int) == 4);
    static_assert(sizeof(unsigned int) == 4);

#if POCO_OS == POCO_OS_WINDOWS_NT
    static_assert(sizeof(signed long int) == 4);
    static_assert(sizeof(unsigned long int) == 4);

    static_assert(sizeof(signed long long int) == 8);
    static_assert(sizeof(unsigned long long int) == 8);
#else
    static_assert(sizeof(signed long int) == 8);
    static_assert(sizeof(unsigned long int) == 8);

    static_assert(sizeof(signed long long int) == 8);
    static_assert(sizeof(unsigned long long int) == 8);
#endif // POCO_OS == POCO_OS_WINDOWS_NT

    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);

    static_assert(sizeof(size_t) == 8);
    static_assert(sizeof(ptrdiff_t) == 8);
}

void test_environment(void)
{
    std::string env_key = "shmilyl_env_test";
    poco_assert(!Poco::Environment::has(env_key));

    Poco::Environment::set(env_key, "milan");
    poco_assert(Poco::Environment::has(env_key));
    fmt::print("env_key: {}, env_val: {}\n", env_key, Poco::Environment::get(env_key));

    std::string env_path = "PATH";
    if (Poco::Environment::has(env_path))
        fmt::print("{} {}: {}\n", Poco::Environment::osName(), env_path, Poco::Environment::get(env_path));

    fmt::print("OS Name        : {}\n", Poco::Environment::osName());
    fmt::print("OS Display Name: {}\n", Poco::Environment::osDisplayName());
    fmt::print("OS Version     : {}\n", Poco::Environment::osVersion());
    fmt::print("OS Architecture: {}\n", Poco::Environment::osArchitecture());
    fmt::print("Node Name      : {}\n", Poco::Environment::nodeName());
    fmt::print("Node ID        : {}\n", Poco::Environment::nodeId());
    fmt::print("Number of CPUs : {}\n", Poco::Environment::processorCount());
}

void test_atomic_counter(void)
{
    Poco::AtomicCounter ac;

    poco_assert(ac.value() == 0);
    poco_assert(ac++ == 0);
    poco_assert(ac-- == 1);
    poco_assert(++ac == 1);
    poco_assert(--ac == 0);

    ac = 2;
    poco_assert(ac.value() == 2);

    Poco::AtomicCounter ac2{100};

    poco_assert(ac2.value() == 100);
    poco_assert(ac2++ == 100);
    poco_assert(ac2-- == 101);
    poco_assert(++ac2 == 101);
    poco_assert(--ac2 == 100);

    ac2 = 200;
    poco_assert(ac2.value() == 200);

    ac = ac2;
    poco_assert(ac.value() == 200);
    poco_assert(ac.value() == ac2.value());
}

void test_ascii(void)
{
    poco_assert(Poco::Ascii::isAscii('0'));
    poco_assert(Poco::Ascii::isAscii('A'));
    poco_assert(!Poco::Ascii::isAscii(-1));
    poco_assert(!Poco::Ascii::isAscii(128));

    poco_assert(Poco::Ascii::isSpace(' '));
    poco_assert(Poco::Ascii::isSpace('\t'));
    poco_assert(Poco::Ascii::isSpace('\r'));
    poco_assert(Poco::Ascii::isSpace('\n'));
    poco_assert(!Poco::Ascii::isSpace('A'));
    poco_assert(!Poco::Ascii::isSpace(-1));
    poco_assert(!Poco::Ascii::isSpace(222));

    poco_assert(Poco::Ascii::isDigit('0'));
    poco_assert(Poco::Ascii::isDigit('1'));
    poco_assert(Poco::Ascii::isDigit('2'));
    poco_assert(Poco::Ascii::isDigit('3'));
    poco_assert(Poco::Ascii::isDigit('4'));
    poco_assert(Poco::Ascii::isDigit('5'));
    poco_assert(Poco::Ascii::isDigit('6'));
    poco_assert(Poco::Ascii::isDigit('7'));
    poco_assert(Poco::Ascii::isDigit('8'));
    poco_assert(Poco::Ascii::isDigit('9'));
    poco_assert(!Poco::Ascii::isDigit('a'));

    poco_assert(Poco::Ascii::isHexDigit('0'));
    poco_assert(Poco::Ascii::isHexDigit('1'));
    poco_assert(Poco::Ascii::isHexDigit('2'));
    poco_assert(Poco::Ascii::isHexDigit('3'));
    poco_assert(Poco::Ascii::isHexDigit('4'));
    poco_assert(Poco::Ascii::isHexDigit('5'));
    poco_assert(Poco::Ascii::isHexDigit('6'));
    poco_assert(Poco::Ascii::isHexDigit('7'));
    poco_assert(Poco::Ascii::isHexDigit('8'));
    poco_assert(Poco::Ascii::isHexDigit('9'));
    poco_assert(Poco::Ascii::isHexDigit('a'));
    poco_assert(Poco::Ascii::isHexDigit('b'));
    poco_assert(Poco::Ascii::isHexDigit('c'));
    poco_assert(Poco::Ascii::isHexDigit('d'));
    poco_assert(Poco::Ascii::isHexDigit('e'));
    poco_assert(Poco::Ascii::isHexDigit('f'));
    poco_assert(Poco::Ascii::isHexDigit('A'));
    poco_assert(Poco::Ascii::isHexDigit('B'));
    poco_assert(Poco::Ascii::isHexDigit('C'));
    poco_assert(Poco::Ascii::isHexDigit('D'));
    poco_assert(Poco::Ascii::isHexDigit('E'));
    poco_assert(Poco::Ascii::isHexDigit('F'));
    poco_assert(!Poco::Ascii::isHexDigit('G'));

    poco_assert(Poco::Ascii::isPunct('.'));
    poco_assert(Poco::Ascii::isPunct(','));
    poco_assert(!Poco::Ascii::isPunct('A'));

    poco_assert(Poco::Ascii::isAlpha('a'));
    poco_assert(Poco::Ascii::isAlpha('Z'));
    poco_assert(!Poco::Ascii::isAlpha('0'));

    poco_assert(Poco::Ascii::isLower('a'));
    poco_assert(!Poco::Ascii::isLower('A'));

    poco_assert(Poco::Ascii::isUpper('A'));
    poco_assert(!Poco::Ascii::isUpper('a'));

    poco_assert(Poco::Ascii::toLower('A') == 'a');
    poco_assert(Poco::Ascii::toLower('z') == 'z');
    poco_assert(Poco::Ascii::toLower('0') == '0');

    poco_assert(Poco::Ascii::toUpper('a') == 'A');
    poco_assert(Poco::Ascii::toUpper('0') == '0');
    poco_assert(Poco::Ascii::toUpper('Z') == 'Z');
}

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4723)
#endif

void test_nan_inf(void)
{
    // float
    {
        float a   = 0.f;
        float b   = 0.f;
        float nan = a / b;
        float inf = 1.f / b;

        poco_assert(!Poco::FPEnvironment::isNaN(a));
        poco_assert(!Poco::FPEnvironment::isInfinite(a));

        poco_assert(!Poco::FPEnvironment::isNaN(b));
        poco_assert(!Poco::FPEnvironment::isInfinite(b));

        poco_assert(Poco::FPEnvironment::isNaN(nan));
        poco_assert(!Poco::FPEnvironment::isInfinite(nan));

        poco_assert(!Poco::FPEnvironment::isNaN(inf));
        poco_assert(Poco::FPEnvironment::isInfinite(inf));
    }

    // double
    {
        double a   = 0.0;
        double b   = 0.0;
        double nan = a / b;
        double inf = 1.0 / b;

        poco_assert(!Poco::FPEnvironment::isNaN(a));
        poco_assert(!Poco::FPEnvironment::isInfinite(a));

        poco_assert(!Poco::FPEnvironment::isNaN(b));
        poco_assert(!Poco::FPEnvironment::isInfinite(b));

        poco_assert(Poco::FPEnvironment::isNaN(nan));
        poco_assert(!Poco::FPEnvironment::isInfinite(nan));

        poco_assert(!Poco::FPEnvironment::isNaN(inf));
        poco_assert(Poco::FPEnvironment::isInfinite(inf));
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

void test_ndc(void)
{
    {
        Poco::NDC ndc;
        poco_assert(ndc.depth() == 0);
        poco_assert(ndc.toString().empty());

        ndc.push("item1");
        poco_assert(ndc.depth() == 1);
        poco_assert(ndc.toString() == "item1");

        ndc.push("item2");
        poco_assert(ndc.depth() == 2);
        poco_assert(ndc.toString() == "item1:item2");

        ndc.pop();
        poco_assert(ndc.depth() == 1);
        poco_assert(ndc.toString() == "item1");

        ndc.pop();
        poco_assert(ndc.depth() == 0);
        poco_assert(ndc.toString().empty());
    }

    poco_assert(Poco::NDC::current().depth() == 0);
    poco_assert(Poco::NDC::current().toString().empty());

    {
        poco_assert(Poco::NDC::current().depth() == 0);
        poco_assert(Poco::NDC::current().toString().empty());

        Poco::NDC::current().push("item1");
        poco_assert(Poco::NDC::current().depth() == 1);
        poco_assert(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().push("item2");
        poco_assert(Poco::NDC::current().depth() == 2);
        poco_assert(Poco::NDC::current().toString() == "item1:item2");

        Poco::NDC::current().pop();
        poco_assert(Poco::NDC::current().depth() == 1);
        poco_assert(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().pop();
        poco_assert(Poco::NDC::current().depth() == 0);
        poco_assert(Poco::NDC::current().toString().empty());
    }

    poco_assert(Poco::NDC::current().depth() == 0);
    poco_assert(Poco::NDC::current().toString().empty());
}

void test_dynamic_factory(void)
{
    using namespace dynamic_factory;

    Poco::DynamicFactory<base_msg> df;

    df.registerClass<player_info>("player_info");
    df.registerClass<friend_info>("friend_info");

    poco_assert(df.isClass("player_info"));
    poco_assert(df.isClass("friend_info"));

    std::unique_ptr<player_info> player_ptr(static_cast<player_info*>(df.createInstance("player_info")));
    std::unique_ptr<friend_info> friend_ptr(static_cast<friend_info*>(df.createInstance("friend_info")));

    poco_assert(player_ptr && player_ptr.get() && player_ptr->name() == "player_info");
    poco_assert(friend_ptr && friend_ptr.get() && friend_ptr->name() == "friend_info");

    df.unregisterClass("friend_info");

    poco_assert(df.isClass("player_info"));
    poco_assert(!df.isClass("friend_info"));
}

void test_memory_pool(void)
{
    Poco::MemoryPool pool(sizeof(int), 32);

    poco_assert(pool.blockSize() == sizeof(int));
    poco_assert(pool.allocated() == 32);

    std::vector<void*> ptrs;
    for (int idx = 0; idx != 32; ++idx)
    {
        int* pi = static_cast<int*>(pool.get());
        *pi     = idx + 1;

        ptrs.emplace_back(pi);
    }

    *static_cast<int*>(pool.get()) = 33;

    poco_assert(pool.blockSize() == sizeof(int));
    poco_assert(pool.allocated() == 33);

    for (int idx = 0; idx != ptrs.size(); ++idx)
    {
        int* pi = static_cast<int*>(ptrs[idx]);
        poco_assert(*pi == idx + 1);
        pool.release(pi);
    }
}

void test_core_all(void)
{
    test_platform();
    test_fixed_length();
    test_environment();
    test_atomic_counter();
    test_ascii();
    test_nan_inf();
    test_ndc();
    test_dynamic_factory();
    test_memory_pool();
}