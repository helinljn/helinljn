#include "core.h"
#include "Poco/Poco.h"
#include "Poco/Environment.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Ascii.h"

#include "fmt/core.h"
#include "fmt/format.h"

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

void test_core_all(void)
{
    test_platform();
    test_fixed_length();
    test_environment();
    test_atomic_counter();
    test_ascii();
}