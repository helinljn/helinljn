#include "doctest.h"
#include "util/numeric_cast.hpp"

#include <cstdint>
#include <limits>

DOCTEST_TEST_SUITE("numeric_cast")
{
    DOCTEST_TEST_CASE("string to int8_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<int8_t>("42") == 42);
        DOCTEST_CHECK(core::to_numeric<int8_t>("0") == 0);
        DOCTEST_CHECK(core::to_numeric<int8_t>("-1") == -1);
        DOCTEST_CHECK(core::to_numeric<int8_t>("127") == 127);
        DOCTEST_CHECK(core::to_numeric<int8_t>("-128") == -128);

        // 转换失败 - 默认版本
        DOCTEST_CHECK(core::to_numeric<int8_t>("") == 0);
        DOCTEST_CHECK(core::to_numeric<int8_t>("abc") == 0);

        // 边界值处理
        DOCTEST_CHECK(core::to_numeric<int8_t>("127") == std::numeric_limits<int8_t>::max());
        DOCTEST_CHECK(core::to_numeric<int8_t>("-128") == std::numeric_limits<int8_t>::min());
    }

    DOCTEST_TEST_CASE("string to uint8_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<uint8_t>("42") == 42);
        DOCTEST_CHECK(core::to_numeric<uint8_t>("0") == 0);
        DOCTEST_CHECK(core::to_numeric<uint8_t>("255") == 255);

        // 转换失败
        DOCTEST_CHECK(core::to_numeric<uint8_t>("") == 0);
        DOCTEST_CHECK(core::to_numeric<uint8_t>("abc") == 0);
    }

    DOCTEST_TEST_CASE("string to int16_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<int16_t>("32767") == 32767);
        DOCTEST_CHECK(core::to_numeric<int16_t>("-32768") == -32768);
        DOCTEST_CHECK(core::to_numeric<int16_t>("0") == 0);

        // 转换失败
        DOCTEST_CHECK(core::to_numeric<int16_t>("abc") == 0);
    }

    DOCTEST_TEST_CASE("string to uint16_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<uint16_t>("65535") == 65535);
        DOCTEST_CHECK(core::to_numeric<uint16_t>("0") == 0);

        // 转换失败
        DOCTEST_CHECK(core::to_numeric<uint16_t>("abc") == 0);
    }

    DOCTEST_TEST_CASE("string to int32_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<int32_t>("2147483647") == 2147483647);
        DOCTEST_CHECK(core::to_numeric<int32_t>("-2147483648") == -2147483648);
        DOCTEST_CHECK(core::to_numeric<int32_t>("0") == 0);
    }

    DOCTEST_TEST_CASE("string to uint32_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<uint32_t>("4294967295") == 4294967295u);
        DOCTEST_CHECK(core::to_numeric<uint32_t>("0") == 0u);
    }

    DOCTEST_TEST_CASE("string to int64_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<int64_t>("9223372036854775807") == 9223372036854775807LL);
        DOCTEST_CHECK(core::to_numeric<int64_t>("-9223372036854775808") == -9223372036854775807LL - 1);
    }

    DOCTEST_TEST_CASE("string to uint64_t")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_numeric<uint64_t>("18446744073709551615") == 18446744073709551615ULL);
        DOCTEST_CHECK(core::to_numeric<uint64_t>("0") == 0ULL);
    }

    DOCTEST_TEST_CASE("string to float")
    {
        // 正常转换
        float val1 = core::to_numeric<float>("3.14");
        DOCTEST_CHECK(val1 == doctest::Approx(3.14f));

        float val2 = core::to_numeric<float>("-2.5");
        DOCTEST_CHECK(val2 == doctest::Approx(-2.5f));

        float val3 = core::to_numeric<float>("0.0");
        DOCTEST_CHECK(val3 == doctest::Approx(0.f));

        float val4 = core::to_numeric<float>("1e-3");
        DOCTEST_CHECK(val4 == doctest::Approx(0.001f));

        // 转换失败
        DOCTEST_CHECK(core::to_numeric<float>("") == 0.f);
        DOCTEST_CHECK(core::to_numeric<float>("abc") == 0.f);
    }

    DOCTEST_TEST_CASE("string to double")
    {
        // 正常转换
        double val1 = core::to_numeric<double>("3.141592653589793");
        DOCTEST_CHECK(val1 == doctest::Approx(3.141592653589793));

        double val2 = core::to_numeric<double>("-2.718281828459045");
        DOCTEST_CHECK(val2 == doctest::Approx(-2.718281828459045));

        // 转换失败
        DOCTEST_CHECK(core::to_numeric<double>("") == 0.0);
    }

    DOCTEST_TEST_CASE("int8_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(static_cast<int8_t>(42)) == "42");
        DOCTEST_CHECK(core::to_string(static_cast<int8_t>(0)) == "0");
        DOCTEST_CHECK(core::to_string(static_cast<int8_t>(-1)) == "-1");
        DOCTEST_CHECK(core::to_string(std::numeric_limits<int8_t>::max()) == "127");
        DOCTEST_CHECK(core::to_string(std::numeric_limits<int8_t>::min()) == "-128");
    }

    DOCTEST_TEST_CASE("uint8_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(static_cast<uint8_t>(42)) == "42");
        DOCTEST_CHECK(core::to_string(static_cast<uint8_t>(0)) == "0");
        DOCTEST_CHECK(core::to_string(static_cast<uint8_t>(255)) == "255");
    }

    DOCTEST_TEST_CASE("int16_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(static_cast<int16_t>(32767)) == "32767");
        DOCTEST_CHECK(core::to_string(static_cast<int16_t>(-32768)) == "-32768");
    }

    DOCTEST_TEST_CASE("uint16_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(static_cast<uint16_t>(65535)) == "65535");
        DOCTEST_CHECK(core::to_string(static_cast<uint16_t>(0)) == "0");
    }

    DOCTEST_TEST_CASE("int32_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(2147483647) == "2147483647");
        DOCTEST_CHECK(core::to_string(-2147483648) == "-2147483648");
        DOCTEST_CHECK(core::to_string(0) == "0");
    }

    DOCTEST_TEST_CASE("uint32_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(4294967295u) == "4294967295");
        DOCTEST_CHECK(core::to_string(0u) == "0");
    }

    DOCTEST_TEST_CASE("int64_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(9223372036854775807LL) == "9223372036854775807");
        DOCTEST_CHECK(core::to_string(-9223372036854775807LL - 1) == "-9223372036854775808");
    }

    DOCTEST_TEST_CASE("uint64_t to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(18446744073709551615ULL) == "18446744073709551615");
        DOCTEST_CHECK(core::to_string(0ULL) == "0");
    }

    DOCTEST_TEST_CASE("float to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(3.14f) == "3.140000");
        DOCTEST_CHECK(core::to_string(-2.5f) == "-2.500000");
        DOCTEST_CHECK(core::to_string(0.f) == "0.000000");
        DOCTEST_CHECK(core::to_string(1.0e-3f) == "0.001000");

        // 特殊值
        DOCTEST_CHECK(core::to_string(std::numeric_limits<float>::infinity()) == "inf");
        DOCTEST_CHECK(core::to_string(-std::numeric_limits<float>::infinity()) == "-inf");
        // NaN 的字符串表示可能是 "nan" 或 "-nan"，这里只检查包含 "nan"
        std::string nan_str = core::to_string(std::numeric_limits<float>::quiet_NaN());
        DOCTEST_CHECK((nan_str.find("nan") != std::string::npos || nan_str.find("NaN") != std::string::npos));
    }

    DOCTEST_TEST_CASE("double to string")
    {
        // 正常转换
        DOCTEST_CHECK(core::to_string(3.141592653589793) == "3.141593");
        DOCTEST_CHECK(core::to_string(-2.718281828459045) == "-2.718282");
    }

    DOCTEST_TEST_CASE("边界和特殊情况")
    {
        // 空字符串
        DOCTEST_CHECK(core::to_numeric<int>("") == 0);
        DOCTEST_CHECK(core::to_numeric<double>("") == 0.0);

        // 前导和后导空格
        DOCTEST_CHECK(core::to_numeric<int>("   42   ") == 42);
        DOCTEST_CHECK(core::to_numeric<double>("   3.14   ") == doctest::Approx(3.14));

        // 科学计数法
        DOCTEST_CHECK(core::to_numeric<double>("1.23e2") == doctest::Approx(123.0));
        DOCTEST_CHECK(core::to_numeric<double>("1.23e-2") == doctest::Approx(0.0123));
    }
}