#include "gtest/gtest.h"
#include "util/numeric_cast.hpp"
#include "util/types.h"
#include "Poco/NumberFormatter.h"

namespace poco_numeric_cast {
namespace details           {

template <typename To, typename From>
struct converter
{
};

// string to int8_t
template <>
struct converter<int8_t, std::string>
{
    static int8_t convert(const std::string& from)
    {
        int8_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to uint8_t
template <>
struct converter<uint8_t, std::string>
{
    static uint8_t convert(const std::string& from)
    {
        uint8_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to int16_t
template <>
struct converter<int16_t, std::string>
{
    static int16_t convert(const std::string& from)
    {
        int16_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to uint16_t
template <>
struct converter<uint16_t, std::string>
{
    static uint16_t convert(const std::string& from)
    {
        uint16_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to int32_t
template <>
struct converter<int32_t, std::string>
{
    static int32_t convert(const std::string& from)
    {
        int32_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to uint32_t
template <>
struct converter<uint32_t, std::string>
{
    static uint32_t convert(const std::string& from)
    {
        uint32_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to int64_t
template <>
struct converter<int64_t, std::string>
{
    static int64_t convert(const std::string& from)
    {
        int64_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to uint64_t
template <>
struct converter<uint64_t, std::string>
{
    static uint64_t convert(const std::string& from)
    {
        uint64_t result = 0;
        Poco::strToInt(from, result, 10);
        return result;
    }
};

// string to float
template <>
struct converter<float, std::string>
{
    static float convert(const std::string& from)
    {
        float result = 0;
        Poco::strToFloat(from, result);
        return result;
    }
};

// string to double
template <>
struct converter<double, std::string>
{
    static double convert(const std::string& from)
    {
        double result = 0;
        Poco::strToDouble(from, result);
        return result;
    }
};

// int8_t to string
template<>
struct converter<std::string, int8_t>
{
    static std::string convert(const int8_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// uint8_t to string
template<>
struct converter<std::string, uint8_t>
{
    static std::string convert(const uint8_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// int16_t to string
template<>
struct converter<std::string, int16_t>
{
    static std::string convert(const int16_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// uint16_t to string
template<>
struct converter<std::string, uint16_t>
{
    static std::string convert(const uint16_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// int32_t to string
template<>
struct converter<std::string, int32_t>
{
    static std::string convert(const int32_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// uint32_t to string
template<>
struct converter<std::string, uint32_t>
{
    static std::string convert(const uint32_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// int64_t to string
template<>
struct converter<std::string, int64_t>
{
    static std::string convert(const int64_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// uint64_t to string
template<>
struct converter<std::string, uint64_t>
{
    static std::string convert(const uint64_t from)
    {
        std::string result;
        Poco::intToStr(from, 10, result);
        return result;
    }
};

// float to string
template<>
struct converter<std::string, float>
{
    static std::string convert(const float from)
    {
        std::string result;
        Poco::floatToStr(result, from);
        return result;
    }
};

// double to string
template<>
struct converter<std::string, double>
{
    static std::string convert(const double from)
    {
        std::string result;
        Poco::doubleToStr(result, from);
        return result;
    }
};

} // namespace details

// string to numeric
template <typename To, typename Dummy = std::enable_if_t<std::is_arithmetic_v<To>>>
To from_string(const std::string& from)
{
    try
    {
        return details::converter<To, std::string>::convert(from);
    }
    catch(...)
    {
        return To{};
    }
}

// numeric to string
template <typename From, typename Dummy = std::enable_if_t<std::is_arithmetic_v<From>>>
std::string to_string(const From from)
{
    return details::converter<std::string, From>::convert(from);
}

} // namespace poco_numeric_cast

GTEST_TEST(PocoNumericStringTest, PocoFromString)
{
    ASSERT_TRUE(poco_numeric_cast::from_string<int8_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int8_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int8_t>("123") == 123);
    ASSERT_TRUE(poco_numeric_cast::from_string<int8_t>("-123") == -123);
    ASSERT_TRUE(poco_numeric_cast::from_string<int8_t>("1234") == 0); // overflow

    ASSERT_TRUE(poco_numeric_cast::from_string<uint8_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint8_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint8_t>("123") == 123);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint8_t>("-123") == 0); // negative
    ASSERT_TRUE(poco_numeric_cast::from_string<uint8_t>("1234") == 0); // overflow

    ASSERT_TRUE(poco_numeric_cast::from_string<int16_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int16_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int16_t>("12345") == 12345);
    ASSERT_TRUE(poco_numeric_cast::from_string<int16_t>("-12345") == -12345);
    ASSERT_TRUE(poco_numeric_cast::from_string<int16_t>("123456") == 0); // overflow

    ASSERT_TRUE(poco_numeric_cast::from_string<uint16_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint16_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint16_t>("12345") == 12345);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint16_t>("-12345") == 0); // negative
    ASSERT_TRUE(poco_numeric_cast::from_string<uint16_t>("123456") == 0); // overflow

    ASSERT_TRUE(poco_numeric_cast::from_string<int32_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int32_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int32_t>("1234567890") == 1234567890);
    ASSERT_TRUE(poco_numeric_cast::from_string<int32_t>("-1234567890") == -1234567890);

    ASSERT_TRUE(poco_numeric_cast::from_string<uint32_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint32_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint32_t>("1234567890") == 1234567890);

    ASSERT_TRUE(poco_numeric_cast::from_string<int64_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int64_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<int64_t>("1234567890987654321") == 1234567890987654321ll);
    ASSERT_TRUE(poco_numeric_cast::from_string<int64_t>("-1234567890987654321") == -1234567890987654321ll);

    ASSERT_TRUE(poco_numeric_cast::from_string<uint64_t>("0") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint64_t>("000") == 0);
    ASSERT_TRUE(poco_numeric_cast::from_string<uint64_t>("1234567890987654321") == 1234567890987654321ull);

    ASSERT_TRUE(poco_numeric_cast::from_string<float>("0") == 0.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("0.f") == 0.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("0.0f") == 0.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("0.00") == 0.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("0.000001") == 0.000001f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-1.2345678") == -1.2345678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-12.345678") == -12.345678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-123.45678") == -123.45678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-1234.5678") == -1234.5678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-12345.678") == -12345.678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-123456.78") == -123456.78f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-1234567.8") == -1234567.8f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-12345678.") == -12345678.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("-12345678") == -12345678.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("1.2345678") == 1.2345678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("12.345678") == 12.345678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("123.45678") == 123.45678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("1234.5678") == 1234.5678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("12345.678") == 12345.678f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("123456.78") == 123456.78f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("1234567.8") == 1234567.8f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("12345678.") == 12345678.f);
    ASSERT_TRUE(poco_numeric_cast::from_string<float>("12345678") == 12345678.f);

    ASSERT_TRUE(poco_numeric_cast::from_string<double>("0") == 0.0);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("0.00") == 0.0);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("0.000000000000001") == 0.000000000000001);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1.234567890123456") == -1.234567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-12.34567890123456") == -12.34567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-123.4567890123456") == -123.4567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234.567890123456") == -1234.567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-12345.67890123456") == -12345.67890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-123456.7890123456") == -123456.7890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234567.890123456") == -1234567.890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-12345678.90123456") == -12345678.90123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-123456789.0123456") == -123456789.0123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234567890.123456") == -1234567890.123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-12345678901.23456") == -12345678901.23456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-123456789012.3456") == -123456789012.3456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234567890123.456") == -1234567890123.456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-12345678901234.56") == -12345678901234.56);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-123456789012345.6") == -123456789012345.6);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234567890123456.") == -1234567890123456.0);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("-1234567890123456") == -1234567890123456.0);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1.234567890123456") == 1.234567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("12.34567890123456") == 12.34567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("123.4567890123456") == 123.4567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234.567890123456") == 1234.567890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("12345.67890123456") == 12345.67890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("123456.7890123456") == 123456.7890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234567.890123456") == 1234567.890123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("12345678.90123456") == 12345678.90123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("123456789.0123456") == 123456789.0123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234567890.123456") == 1234567890.123456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("12345678901.23456") == 12345678901.23456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("123456789012.3456") == 123456789012.3456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234567890123.456") == 1234567890123.456);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("12345678901234.56") == 12345678901234.56);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("123456789012345.6") == 123456789012345.6);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234567890123456.") == 1234567890123456.0);
    ASSERT_TRUE(poco_numeric_cast::from_string<double>("1234567890123456") == 1234567890123456.0);
}

GTEST_TEST(PocoNumericStringTest, StdFromString)
{
    ASSERT_TRUE(common::from_string<int8_t>("0") == 0);
    ASSERT_TRUE(common::from_string<int8_t>("000") == 0);
    ASSERT_TRUE(common::from_string<int8_t>("123") == 123);
    ASSERT_TRUE(common::from_string<int8_t>("-123") == -123);
    ASSERT_TRUE(common::from_string<int8_t>("1234") == -46); // overflow

    ASSERT_TRUE(common::from_string<uint8_t>("0") == 0);
    ASSERT_TRUE(common::from_string<uint8_t>("000") == 0);
    ASSERT_TRUE(common::from_string<uint8_t>("123") == 123);
    ASSERT_TRUE(common::from_string<uint8_t>("-123") == 133); // negative
    ASSERT_TRUE(common::from_string<uint8_t>("1234") == 210); // overflow

    ASSERT_TRUE(common::from_string<int16_t>("0") == 0);
    ASSERT_TRUE(common::from_string<int16_t>("000") == 0);
    ASSERT_TRUE(common::from_string<int16_t>("12345") == 12345);
    ASSERT_TRUE(common::from_string<int16_t>("-12345") == -12345);
    ASSERT_TRUE(common::from_string<int16_t>("123456") == -7616); // overflow

    ASSERT_TRUE(common::from_string<uint16_t>("0") == 0);
    ASSERT_TRUE(common::from_string<uint16_t>("000") == 0);
    ASSERT_TRUE(common::from_string<uint16_t>("12345") == 12345);
    ASSERT_TRUE(common::from_string<uint16_t>("-12345") == 53191); // negative
    ASSERT_TRUE(common::from_string<uint16_t>("123456") == 57920); // overflow

    ASSERT_TRUE(common::from_string<int32_t>("0") == 0);
    ASSERT_TRUE(common::from_string<int32_t>("000") == 0);
    ASSERT_TRUE(common::from_string<int32_t>("1234567890") == 1234567890);
    ASSERT_TRUE(common::from_string<int32_t>("-1234567890") == -1234567890);

    ASSERT_TRUE(common::from_string<uint32_t>("0") == 0);
    ASSERT_TRUE(common::from_string<uint32_t>("000") == 0);
    ASSERT_TRUE(common::from_string<uint32_t>("1234567890") == 1234567890);

    ASSERT_TRUE(common::from_string<int64_t>("0") == 0);
    ASSERT_TRUE(common::from_string<int64_t>("000") == 0);
    ASSERT_TRUE(common::from_string<int64_t>("1234567890987654321") == 1234567890987654321ll);
    ASSERT_TRUE(common::from_string<int64_t>("-1234567890987654321") == -1234567890987654321ll);

    ASSERT_TRUE(common::from_string<uint64_t>("0") == 0);
    ASSERT_TRUE(common::from_string<uint64_t>("000") == 0);
    ASSERT_TRUE(common::from_string<uint64_t>("1234567890987654321") == 1234567890987654321ull);

    ASSERT_TRUE(common::from_string<float>("0") == 0.f);
    ASSERT_TRUE(common::from_string<float>("0.f") == 0.f);
    ASSERT_TRUE(common::from_string<float>("0.0f") == 0.f);
    ASSERT_TRUE(common::from_string<float>("0.00") == 0.f);
    ASSERT_TRUE(common::from_string<float>("0.000001") == 0.000001f);
    ASSERT_TRUE(common::from_string<float>("-1.2345678") == -1.2345678f);
    ASSERT_TRUE(common::from_string<float>("-12.345678") == -12.345678f);
    ASSERT_TRUE(common::from_string<float>("-123.45678") == -123.45678f);
    ASSERT_TRUE(common::from_string<float>("-1234.5678") == -1234.5678f);
    ASSERT_TRUE(common::from_string<float>("-12345.678") == -12345.678f);
    ASSERT_TRUE(common::from_string<float>("-123456.78") == -123456.78f);
    ASSERT_TRUE(common::from_string<float>("-1234567.8") == -1234567.8f);
    ASSERT_TRUE(common::from_string<float>("-12345678.") == -12345678.f);
    ASSERT_TRUE(common::from_string<float>("-12345678") == -12345678.f);
    ASSERT_TRUE(common::from_string<float>("1.2345678") == 1.2345678f);
    ASSERT_TRUE(common::from_string<float>("12.345678") == 12.345678f);
    ASSERT_TRUE(common::from_string<float>("123.45678") == 123.45678f);
    ASSERT_TRUE(common::from_string<float>("1234.5678") == 1234.5678f);
    ASSERT_TRUE(common::from_string<float>("12345.678") == 12345.678f);
    ASSERT_TRUE(common::from_string<float>("123456.78") == 123456.78f);
    ASSERT_TRUE(common::from_string<float>("1234567.8") == 1234567.8f);
    ASSERT_TRUE(common::from_string<float>("12345678.") == 12345678.f);
    ASSERT_TRUE(common::from_string<float>("12345678") == 12345678.f);

    ASSERT_TRUE(common::from_string<double>("0") == 0.0);
    ASSERT_TRUE(common::from_string<double>("0.00") == 0.0);
    ASSERT_TRUE(common::from_string<double>("0.000000000000001") == 0.000000000000001);
    ASSERT_TRUE(common::from_string<double>("-1.234567890123456") == -1.234567890123456);
    ASSERT_TRUE(common::from_string<double>("-12.34567890123456") == -12.34567890123456);
    ASSERT_TRUE(common::from_string<double>("-123.4567890123456") == -123.4567890123456);
    ASSERT_TRUE(common::from_string<double>("-1234.567890123456") == -1234.567890123456);
    ASSERT_TRUE(common::from_string<double>("-12345.67890123456") == -12345.67890123456);
    ASSERT_TRUE(common::from_string<double>("-123456.7890123456") == -123456.7890123456);
    ASSERT_TRUE(common::from_string<double>("-1234567.890123456") == -1234567.890123456);
    ASSERT_TRUE(common::from_string<double>("-12345678.90123456") == -12345678.90123456);
    ASSERT_TRUE(common::from_string<double>("-123456789.0123456") == -123456789.0123456);
    ASSERT_TRUE(common::from_string<double>("-1234567890.123456") == -1234567890.123456);
    ASSERT_TRUE(common::from_string<double>("-12345678901.23456") == -12345678901.23456);
    ASSERT_TRUE(common::from_string<double>("-123456789012.3456") == -123456789012.3456);
    ASSERT_TRUE(common::from_string<double>("-1234567890123.456") == -1234567890123.456);
    ASSERT_TRUE(common::from_string<double>("-12345678901234.56") == -12345678901234.56);
    ASSERT_TRUE(common::from_string<double>("-123456789012345.6") == -123456789012345.6);
    ASSERT_TRUE(common::from_string<double>("-1234567890123456.") == -1234567890123456.0);
    ASSERT_TRUE(common::from_string<double>("-1234567890123456") == -1234567890123456.0);
    ASSERT_TRUE(common::from_string<double>("1.234567890123456") == 1.234567890123456);
    ASSERT_TRUE(common::from_string<double>("12.34567890123456") == 12.34567890123456);
    ASSERT_TRUE(common::from_string<double>("123.4567890123456") == 123.4567890123456);
    ASSERT_TRUE(common::from_string<double>("1234.567890123456") == 1234.567890123456);
    ASSERT_TRUE(common::from_string<double>("12345.67890123456") == 12345.67890123456);
    ASSERT_TRUE(common::from_string<double>("123456.7890123456") == 123456.7890123456);
    ASSERT_TRUE(common::from_string<double>("1234567.890123456") == 1234567.890123456);
    ASSERT_TRUE(common::from_string<double>("12345678.90123456") == 12345678.90123456);
    ASSERT_TRUE(common::from_string<double>("123456789.0123456") == 123456789.0123456);
    ASSERT_TRUE(common::from_string<double>("1234567890.123456") == 1234567890.123456);
    ASSERT_TRUE(common::from_string<double>("12345678901.23456") == 12345678901.23456);
    ASSERT_TRUE(common::from_string<double>("123456789012.3456") == 123456789012.3456);
    ASSERT_TRUE(common::from_string<double>("1234567890123.456") == 1234567890123.456);
    ASSERT_TRUE(common::from_string<double>("12345678901234.56") == 12345678901234.56);
    ASSERT_TRUE(common::from_string<double>("123456789012345.6") == 123456789012345.6);
    ASSERT_TRUE(common::from_string<double>("1234567890123456.") == 1234567890123456.0);
    ASSERT_TRUE(common::from_string<double>("1234567890123456") == 1234567890123456.0);
}

GTEST_TEST(PocoNumericStringTest, PocoToString)
{
    ASSERT_TRUE(poco_numeric_cast::to_string(int8_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(int8_t{123}) == "123");
    ASSERT_TRUE(poco_numeric_cast::to_string(int8_t{-123}) == "-123");

    ASSERT_TRUE(poco_numeric_cast::to_string(uint8_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(uint8_t{123}) == "123");

    ASSERT_TRUE(poco_numeric_cast::to_string(int16_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(int16_t{12345}) == "12345");
    ASSERT_TRUE(poco_numeric_cast::to_string(int16_t{-12345}) == "-12345");

    ASSERT_TRUE(poco_numeric_cast::to_string(uint16_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(uint16_t{12345}) == "12345");

    ASSERT_TRUE(poco_numeric_cast::to_string(int32_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(int32_t{1234567890}) == "1234567890");
    ASSERT_TRUE(poco_numeric_cast::to_string(int32_t{-1234567890}) == "-1234567890");

    ASSERT_TRUE(poco_numeric_cast::to_string(uint32_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(uint32_t{1234567890}) == "1234567890");

    ASSERT_TRUE(poco_numeric_cast::to_string(int64_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(int64_t{1234567890987654321}) == "1234567890987654321");
    ASSERT_TRUE(poco_numeric_cast::to_string(int64_t{-1234567890987654321}) == "-1234567890987654321");

    ASSERT_TRUE(poco_numeric_cast::to_string(uint64_t{0}) == "0");
    ASSERT_TRUE(poco_numeric_cast::to_string(uint64_t{1234567890987654321}) == "1234567890987654321");

    ASSERT_FALSE(poco_numeric_cast::to_string(float{0.f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{0.000001f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-1.2345678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-12.345678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-123.45678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-1234.5678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-12345.678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-123456.78f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-1234567.8f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-12345678.f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{-12345678}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{1.2345678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{12.345678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{123.45678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{1234.5678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{12345.678f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{123456.78f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{1234567.8f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{12345678.f}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(float{12345678}).empty());

    ASSERT_FALSE(poco_numeric_cast::to_string(double{0.0}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{0.000000000000001}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1.234567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-12.34567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-123.4567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234.567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-12345.67890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-123456.7890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234567.890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-12345678.90123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-123456789.0123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234567890.123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-12345678901.23456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-123456789012.3456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234567890123.456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-12345678901234.56}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-123456789012345.6}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234567890123456.0}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{-1234567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1.234567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{12.34567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{123.4567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234.567890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{12345.67890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{123456.7890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234567.890123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{12345678.90123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{123456789.0123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234567890.123456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{12345678901.23456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{123456789012.3456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234567890123.456}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{12345678901234.56}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{123456789012345.6}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234567890123456.0}).empty());
    ASSERT_FALSE(poco_numeric_cast::to_string(double{1234567890123456}).empty());
}

GTEST_TEST(PocoNumericStringTest, StdToString)
{
    ASSERT_TRUE(common::to_string(int8_t{0}) == "0");
    ASSERT_TRUE(common::to_string(int8_t{123}) == "123");
    ASSERT_TRUE(common::to_string(int8_t{-123}) == "-123");

    ASSERT_TRUE(common::to_string(uint8_t{0}) == "0");
    ASSERT_TRUE(common::to_string(uint8_t{123}) == "123");

    ASSERT_TRUE(common::to_string(int16_t{0}) == "0");
    ASSERT_TRUE(common::to_string(int16_t{12345}) == "12345");
    ASSERT_TRUE(common::to_string(int16_t{-12345}) == "-12345");

    ASSERT_TRUE(common::to_string(uint16_t{0}) == "0");
    ASSERT_TRUE(common::to_string(uint16_t{12345}) == "12345");

    ASSERT_TRUE(common::to_string(int32_t{0}) == "0");
    ASSERT_TRUE(common::to_string(int32_t{1234567890}) == "1234567890");
    ASSERT_TRUE(common::to_string(int32_t{-1234567890}) == "-1234567890");

    ASSERT_TRUE(common::to_string(uint32_t{0}) == "0");
    ASSERT_TRUE(common::to_string(uint32_t{1234567890}) == "1234567890");

    ASSERT_TRUE(common::to_string(int64_t{0}) == "0");
    ASSERT_TRUE(common::to_string(int64_t{1234567890987654321}) == "1234567890987654321");
    ASSERT_TRUE(common::to_string(int64_t{-1234567890987654321}) == "-1234567890987654321");

    ASSERT_TRUE(common::to_string(uint64_t{0}) == "0");
    ASSERT_TRUE(common::to_string(uint64_t{1234567890987654321}) == "1234567890987654321");

    ASSERT_FALSE(common::to_string(float{0.f}).empty());
    ASSERT_FALSE(common::to_string(float{0.000001f}).empty());
    ASSERT_FALSE(common::to_string(float{-1.2345678f}).empty());
    ASSERT_FALSE(common::to_string(float{-12.345678f}).empty());
    ASSERT_FALSE(common::to_string(float{-123.45678f}).empty());
    ASSERT_FALSE(common::to_string(float{-1234.5678f}).empty());
    ASSERT_FALSE(common::to_string(float{-12345.678f}).empty());
    ASSERT_FALSE(common::to_string(float{-123456.78f}).empty());
    ASSERT_FALSE(common::to_string(float{-1234567.8f}).empty());
    ASSERT_FALSE(common::to_string(float{-12345678.f}).empty());
    ASSERT_FALSE(common::to_string(float{-12345678}).empty());
    ASSERT_FALSE(common::to_string(float{1.2345678f}).empty());
    ASSERT_FALSE(common::to_string(float{12.345678f}).empty());
    ASSERT_FALSE(common::to_string(float{123.45678f}).empty());
    ASSERT_FALSE(common::to_string(float{1234.5678f}).empty());
    ASSERT_FALSE(common::to_string(float{12345.678f}).empty());
    ASSERT_FALSE(common::to_string(float{123456.78f}).empty());
    ASSERT_FALSE(common::to_string(float{1234567.8f}).empty());
    ASSERT_FALSE(common::to_string(float{12345678.f}).empty());
    ASSERT_FALSE(common::to_string(float{12345678}).empty());

    ASSERT_FALSE(common::to_string(double{0.0}).empty());
    ASSERT_FALSE(common::to_string(double{0.000000000000001}).empty());
    ASSERT_FALSE(common::to_string(double{-1.234567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-12.34567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-123.4567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-1234.567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-12345.67890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-123456.7890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-1234567.890123456}).empty());
    ASSERT_FALSE(common::to_string(double{-12345678.90123456}).empty());
    ASSERT_FALSE(common::to_string(double{-123456789.0123456}).empty());
    ASSERT_FALSE(common::to_string(double{-1234567890.123456}).empty());
    ASSERT_FALSE(common::to_string(double{-12345678901.23456}).empty());
    ASSERT_FALSE(common::to_string(double{-123456789012.3456}).empty());
    ASSERT_FALSE(common::to_string(double{-1234567890123.456}).empty());
    ASSERT_FALSE(common::to_string(double{-12345678901234.56}).empty());
    ASSERT_FALSE(common::to_string(double{-123456789012345.6}).empty());
    ASSERT_FALSE(common::to_string(double{-1234567890123456.0}).empty());
    ASSERT_FALSE(common::to_string(double{-1234567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{1.234567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{12.34567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{123.4567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{1234.567890123456}).empty());
    ASSERT_FALSE(common::to_string(double{12345.67890123456}).empty());
    ASSERT_FALSE(common::to_string(double{123456.7890123456}).empty());
    ASSERT_FALSE(common::to_string(double{1234567.890123456}).empty());
    ASSERT_FALSE(common::to_string(double{12345678.90123456}).empty());
    ASSERT_FALSE(common::to_string(double{123456789.0123456}).empty());
    ASSERT_FALSE(common::to_string(double{1234567890.123456}).empty());
    ASSERT_FALSE(common::to_string(double{12345678901.23456}).empty());
    ASSERT_FALSE(common::to_string(double{123456789012.3456}).empty());
    ASSERT_FALSE(common::to_string(double{1234567890123.456}).empty());
    ASSERT_FALSE(common::to_string(double{12345678901234.56}).empty());
    ASSERT_FALSE(common::to_string(double{123456789012345.6}).empty());
    ASSERT_FALSE(common::to_string(double{1234567890123456.0}).empty());
    ASSERT_FALSE(common::to_string(double{1234567890123456}).empty());
}