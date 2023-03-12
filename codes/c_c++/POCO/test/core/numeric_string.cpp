#include "numeric_string.h"
#include "Poco/Poco.h"
#include "Poco/Stopwatch.h"

#include "fmt/format.h"

#include <string>       // for cpp11_numeric_string
#include <type_traits>  // for cpp11_numeric_string

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "Poco/NumericString.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace cpp11_numeric_string {
namespace details              {

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
        return static_cast<int8_t>(std::stoi(from));
    }
};

// string to uint8_t
template <>
struct converter<uint8_t, std::string>
{
    static uint8_t convert(const std::string& from)
    {
        return static_cast<uint8_t>(std::stoi(from));
    }
};

// string to int16_t
template <>
struct converter<int16_t, std::string>
{
    static int16_t convert(const std::string& from)
    {
        return static_cast<int16_t>(std::stoi(from));
    }
};

// string to uint16_t
template <>
struct converter<uint16_t, std::string>
{
    static uint16_t convert(const std::string& from)
    {
        return static_cast<uint16_t>(std::stoi(from));
    }
};

// string to int32_t
template <>
struct converter<int32_t, std::string>
{
    static int32_t convert(const std::string& from)
    {
        return static_cast<int32_t>(std::stol(from));
    }
};

// string to uint32_t
template <>
struct converter<uint32_t, std::string>
{
    static uint32_t convert(const std::string& from)
    {
        return static_cast<uint32_t>(std::stoul(from));
    }
};

// string to int64_t
template <>
struct converter<int64_t, std::string>
{
    static int64_t convert(const std::string& from)
    {
        return static_cast<int64_t>(std::stoll(from));
    }
};

// string to uint64_t
template <>
struct converter<uint64_t, std::string>
{
    static uint64_t convert(const std::string& from)
    {
        return static_cast<uint64_t>(std::stoull(from));
    }
};

// string to float
template <>
struct converter<float, std::string>
{
    static float convert(const std::string& from)
    {
        return std::stof(from);
    }
};

// string to double
template <>
struct converter<double, std::string>
{
    static double convert(const std::string& from)
    {
        return std::stod(from);
    }
};

// int8_t to string
template<>
struct converter<std::string, int8_t>
{
    static std::string convert(const int8_t from)
    {
        return std::to_string(from);
    }
};

// uint8_t to string
template<>
struct converter<std::string, uint8_t>
{
    static std::string convert(const uint8_t from)
    {
        return std::to_string(from);
    }
};

// int16_t to string
template<>
struct converter<std::string, int16_t>
{
    static std::string convert(const int16_t from)
    {
        return std::to_string(from);
    }
};

// uint16_t to string
template<>
struct converter<std::string, uint16_t>
{
    static std::string convert(const uint16_t from)
    {
        return std::to_string(from);
    }
};

// int32_t to string
template<>
struct converter<std::string, int32_t>
{
    static std::string convert(const int32_t from)
    {
        return std::to_string(from);
    }
};

// uint32_t to string
template<>
struct converter<std::string, uint32_t>
{
    static std::string convert(const uint32_t from)
    {
        return std::to_string(from);
    }
};

// int64_t to string
template<>
struct converter<std::string, int64_t>
{
    static std::string convert(const int64_t from)
    {
        return std::to_string(from);
    }
};

// uint64_t to string
template<>
struct converter<std::string, uint64_t>
{
    static std::string convert(const uint64_t from)
    {
        return std::to_string(from);
    }
};

// float to string
template<>
struct converter<std::string, float>
{
    static std::string convert(const float from)
    {
        return std::to_string(from);
    }
};

// double to string
template<>
struct converter<std::string, double>
{
    static std::string convert(const double from)
    {
        return std::to_string(from);
    }
};

} // namespace details

// string to numeric
template <typename To, typename Dummy = std::enable_if_t<std::is_integral_v<To> || std::is_floating_point_v<To>>>
inline To from_string(const std::string& from)
{
    return details::converter<To, std::string>::convert(from);
}

// numeric to string
template <typename From, typename Dummy = std::enable_if_t<std::is_integral_v<From> || std::is_floating_point_v<From>>>
inline std::string to_string(const From from)
{
    return details::converter<std::string, From>::convert(from);
}

// string to integral
template <typename To, typename Dummy = std::enable_if_t<std::is_integral_v<To>>>
bool str_to_int(const std::string& from, To& out)
{
    try
    {
        out = details::converter<To, std::string>::convert(from);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

// string to float
bool str_to_float(const std::string& from, float& out)
{
    try
    {
        out = details::converter<float, std::string>::convert(from);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

// string to double
bool str_to_double(const std::string& from, double& out)
{
    try
    {
        out = details::converter<double, std::string>::convert(from);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

// integral to string
template <typename From, typename Dummy = std::enable_if_t<std::is_integral_v<From>>>
bool int_to_str(const From from, std::string& out)
{
    out = details::converter<std::string, From>::convert(from);
    return true;
}

// float to string
bool float_to_str(const float from, std::string& out)
{
    out = details::converter<std::string, float>::convert(from);
    return true;
}

// double to string
bool double_to_str(const double from, std::string& out)
{
    out = details::converter<std::string, double>::convert(from);
    return true;
}

} // namespace cpp11_numeric_string

void test_poco_str_to_numeric(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    poco_assert(Poco::strToInt("0", i8, 10) && i8 == 0);
    poco_assert(Poco::strToInt("000", i8, 10) && i8 == 0);
    poco_assert(Poco::strToInt("123", i8, 10) && i8 == 123);
    poco_assert(Poco::strToInt("-123", i8, 10) && i8 == -123);
    poco_assert(!Poco::strToInt("1234", i8, 10));

    poco_assert(Poco::strToInt("0", u8, 10) && u8 == 0);
    poco_assert(Poco::strToInt("000", u8, 10) && u8 == 0);
    poco_assert(Poco::strToInt("123", u8, 10) && u8 == 123);
    poco_assert(!Poco::strToInt("-123", u8, 10));
    poco_assert(!Poco::strToInt("1234", u8, 10));

    poco_assert(Poco::strToInt("0", i16, 10) && i16 == 0);
    poco_assert(Poco::strToInt("000", i16, 10) && i16 == 0);
    poco_assert(Poco::strToInt("12345", i16, 10) && i16 == 12345);
    poco_assert(Poco::strToInt("-12345", i16, 10) && i16 == -12345);
    poco_assert(!Poco::strToInt("123456", i16, 10));

    poco_assert(Poco::strToInt("0", u16, 10) && u16 == 0);
    poco_assert(Poco::strToInt("000", u16, 10) && u16 == 0);
    poco_assert(Poco::strToInt("12345", u16, 10) && u16 == 12345);
    poco_assert(!Poco::strToInt("-12345", u16, 10));
    poco_assert(!Poco::strToInt("123456", u16, 10));

    poco_assert(Poco::strToInt("0", i32, 10) && i32 == 0);
    poco_assert(Poco::strToInt("000", i32, 10) && i32 == 0);
    poco_assert(Poco::strToInt("1234567890", i32, 10) && i32 == 1234567890);
    poco_assert(Poco::strToInt("-1234567890", i32, 10) && i32 == -1234567890);
    poco_assert(!Poco::strToInt("12345678909", i32, 10));

    poco_assert(Poco::strToInt("0", u32, 10) && u32 == 0);
    poco_assert(Poco::strToInt("000", u32, 10) && u32 == 0);
    poco_assert(Poco::strToInt("1234567890", u32, 10) && u32 == 1234567890);
    poco_assert(!Poco::strToInt("-1234567890", u32, 10));
    poco_assert(!Poco::strToInt("12345678909", u32, 10));

    poco_assert(Poco::strToInt("0", i64, 10) && i64 == 0);
    poco_assert(Poco::strToInt("000", i64, 10) && i64 == 0);
    poco_assert(Poco::strToInt("1234567890987654321", i64, 10) && i64 == 1234567890987654321);
    poco_assert(Poco::strToInt("-1234567890987654321", i64, 10) && i64 == -1234567890987654321);
    poco_assert(!Poco::strToInt("12345678909876543210123456789", i64, 10));

    poco_assert(Poco::strToInt("0", u64, 10) && u64 == 0);
    poco_assert(Poco::strToInt("000", u64, 10) && u64 == 0);
    poco_assert(Poco::strToInt("1234567890987654321", u64, 10) && u64 == 1234567890987654321);
    poco_assert(!Poco::strToInt("-1234567890987654321", u64, 10));
    poco_assert(!Poco::strToInt("12345678909876543210123456789", u64, 10));

    poco_assert(Poco::strToFloat("0", f32) && f32 == 0.f);
    poco_assert(Poco::strToFloat("0.f", f32) && f32 == 0.f);
    poco_assert(Poco::strToFloat("0.0f", f32) && f32 == 0.f);
    poco_assert(Poco::strToFloat("0.00", f32) && f32 == 0.f);
    poco_assert(Poco::strToFloat("0.000001", f32) && f32 == 0.000001f);
    poco_assert(Poco::strToFloat("3.141593", f32) && f32 == 3.141593f);
    poco_assert(Poco::strToFloat("-1234567", f32) && f32 == -1234567.f);
    poco_assert(Poco::strToFloat("1234567", f32) && f32 == 1234567.f);

    poco_assert(Poco::strToDouble("0", d64) && d64 == 0.0);
    poco_assert(Poco::strToDouble("0.00", d64) && d64 == 0.0);
    poco_assert(Poco::strToDouble("0.000000000000001", d64) && d64 == 0.000000000000001);
    poco_assert(Poco::strToDouble("3.141592653589793", d64) && d64 == 3.141592653589793);
    poco_assert(Poco::strToDouble("-1234567890123456", d64) && d64 == -1234567890123456.0);
    poco_assert(Poco::strToDouble("1234567890123456", d64) && d64 == 1234567890123456.0);
}

void test_std_str_to_numeric(void)
{
    // cpp11_numeric_string::from_string
    {
        poco_assert(cpp11_numeric_string::from_string<int8_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<int8_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<int8_t>("123") == 123);
        poco_assert(cpp11_numeric_string::from_string<int8_t>("-123") == -123);
        poco_assert(cpp11_numeric_string::from_string<int8_t>("1234") == -46);

        poco_assert(cpp11_numeric_string::from_string<uint8_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint8_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint8_t>("123") == 123);
        poco_assert(cpp11_numeric_string::from_string<uint8_t>("-123") == 133);
        poco_assert(cpp11_numeric_string::from_string<uint8_t>("1234") == 210);

        poco_assert(cpp11_numeric_string::from_string<int16_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<int16_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<int16_t>("12345") == 12345);
        poco_assert(cpp11_numeric_string::from_string<int16_t>("-12345") == -12345);
        poco_assert(cpp11_numeric_string::from_string<int16_t>("123456") == -7616);

        poco_assert(cpp11_numeric_string::from_string<uint16_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint16_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint16_t>("12345") == 12345);
        poco_assert(cpp11_numeric_string::from_string<uint16_t>("-12345") == 53191);
        poco_assert(cpp11_numeric_string::from_string<uint16_t>("123456") == 57920);

        poco_assert(cpp11_numeric_string::from_string<int32_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<int32_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<int32_t>("1234567890") == 1234567890);
        poco_assert(cpp11_numeric_string::from_string<int32_t>("-1234567890") == -1234567890);

        poco_assert(cpp11_numeric_string::from_string<uint32_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint32_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint32_t>("1234567890") == 1234567890);
        poco_assert(cpp11_numeric_string::from_string<uint32_t>("-1234567890") == 3060399406);

        poco_assert(cpp11_numeric_string::from_string<int64_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<int64_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<int64_t>("1234567890987654321") == 1234567890987654321ll);
        poco_assert(cpp11_numeric_string::from_string<int64_t>("-1234567890987654321") == -1234567890987654321ll);

        poco_assert(cpp11_numeric_string::from_string<uint64_t>("0") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint64_t>("000") == 0);
        poco_assert(cpp11_numeric_string::from_string<uint64_t>("1234567890987654321") == 1234567890987654321ull);
        poco_assert(cpp11_numeric_string::from_string<uint64_t>("-1234567890987654321") == 17212176182721897295ull);

        poco_assert(cpp11_numeric_string::from_string<float>("0") == 0.f);
        poco_assert(cpp11_numeric_string::from_string<float>("0.f") == 0.f);
        poco_assert(cpp11_numeric_string::from_string<float>("0.0f") == 0.f);
        poco_assert(cpp11_numeric_string::from_string<float>("0.00") == 0.f);
        poco_assert(cpp11_numeric_string::from_string<float>("0.000001") == 0.000001f);
        poco_assert(cpp11_numeric_string::from_string<float>("3.141593") == 3.141593f);
        poco_assert(cpp11_numeric_string::from_string<float>("-1234567") == -1234567.f);
        poco_assert(cpp11_numeric_string::from_string<float>("1234567") == 1234567.f);

        poco_assert(cpp11_numeric_string::from_string<double>("0") == 0.0);
        poco_assert(cpp11_numeric_string::from_string<double>("0.00") == 0.0);
        poco_assert(cpp11_numeric_string::from_string<double>("0.000000000000001") == 0.000000000000001);
        poco_assert(cpp11_numeric_string::from_string<double>("3.141592653589793") == 3.141592653589793);
        poco_assert(cpp11_numeric_string::from_string<double>("-1234567890123456") == -1234567890123456.0);
        poco_assert(cpp11_numeric_string::from_string<double>("1234567890123456") == 1234567890123456.0);
    }

    // cpp11_numeric_string::str_to_*
    {
        int8_t   i8  = 0;
        uint8_t  u8  = 0;
        int16_t  i16 = 0;
        uint16_t u16 = 0;
        int32_t  i32 = 0;
        uint32_t u32 = 0;
        int64_t  i64 = 0;
        uint64_t u64 = 0;
        float    f32 = 0.f;
        double   d64 = 0.0;

        poco_assert(cpp11_numeric_string::str_to_int("0", i8) && i8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", i8) && i8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("123", i8) && i8 == 123);
        poco_assert(cpp11_numeric_string::str_to_int("-123", i8) && i8 == -123);
        poco_assert(cpp11_numeric_string::str_to_int("1234", i8) && i8 == -46);

        poco_assert(cpp11_numeric_string::str_to_int("0", u8) && u8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", u8) && u8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("123", u8) && u8 == 123);
        poco_assert(cpp11_numeric_string::str_to_int("-123", u8) && u8 == 133);
        poco_assert(cpp11_numeric_string::str_to_int("1234", u8) && u8 == 210);

        poco_assert(cpp11_numeric_string::str_to_int("0", i16) && i16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", i16) && i16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("12345", i16) && i16 == 12345);
        poco_assert(cpp11_numeric_string::str_to_int("-12345", i16) && i16 == -12345);
        poco_assert(cpp11_numeric_string::str_to_int("123456", i16) && i16 == -7616);

        poco_assert(cpp11_numeric_string::str_to_int("0", u16) && u16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", u16) && u16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("12345", u16) && u16 == 12345);
        poco_assert(cpp11_numeric_string::str_to_int("-12345", u16) && u16 == 53191);
        poco_assert(cpp11_numeric_string::str_to_int("123456", u16) && u16 == 57920);

        poco_assert(cpp11_numeric_string::str_to_int("0", i32) && i32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", i32) && i32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890", i32) && i32 == 1234567890);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890", i32) && i32 == -1234567890);

        poco_assert(cpp11_numeric_string::str_to_int("0", u32) && u32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", u32) && u32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890", u32) && u32 == 1234567890);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890", u32) && u32 == 3060399406);

        poco_assert(cpp11_numeric_string::str_to_int("0", i64) && i64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", i64) && i64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890987654321", i64) && i64 == 1234567890987654321ll);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890987654321", i64) && i64 == -1234567890987654321ll);

        poco_assert(cpp11_numeric_string::str_to_int("0", u64) && u64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("000", u64) && u64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890987654321", u64) && u64 == 1234567890987654321ull);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890987654321", u64) && u64 == 17212176182721897295ull);

        poco_assert(cpp11_numeric_string::str_to_float("0", f32) && f32 == 0.f);
        poco_assert(cpp11_numeric_string::str_to_float("0.f", f32) && f32 == 0.f);
        poco_assert(cpp11_numeric_string::str_to_float("0.0f", f32) && f32 == 0.f);
        poco_assert(cpp11_numeric_string::str_to_float("0.00", f32) && f32 == 0.f);
        poco_assert(cpp11_numeric_string::str_to_float("0.000001", f32) && f32 == 0.000001f);
        poco_assert(cpp11_numeric_string::str_to_float("3.141593", f32) && f32 == 3.141593f);
        poco_assert(cpp11_numeric_string::str_to_float("-1234567", f32) && f32 == -1234567.f);
        poco_assert(cpp11_numeric_string::str_to_float("1234567", f32) && f32 == 1234567.f);

        poco_assert(cpp11_numeric_string::str_to_double("0", d64) && d64 == 0.0);
        poco_assert(cpp11_numeric_string::str_to_double("0.00", d64) && d64 == 0.0);
        poco_assert(cpp11_numeric_string::str_to_double("0.000000000000001", d64) && d64 == 0.000000000000001);
        poco_assert(cpp11_numeric_string::str_to_double("3.141592653589793", d64) && d64 == 3.141592653589793);
        poco_assert(cpp11_numeric_string::str_to_double("-1234567890123456", d64) && d64 == -1234567890123456.0);
        poco_assert(cpp11_numeric_string::str_to_double("1234567890123456", d64) && d64 == 1234567890123456.0);
    }
}

void test_poco_numeric_to_str(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    std::string res;

    i8 = 0;
    poco_assert(Poco::intToStr(i8, 10, res) && res == "0");
    i8 = 123;
    poco_assert(Poco::intToStr(i8, 10, res) && res == "123");
    i8 = -123;
    poco_assert(Poco::intToStr(i8, 10, res) && res == "-123");

    u8 = 0;
    poco_assert(Poco::intToStr(u8, 10, res) && res == "0");
    u8 = 123;
    poco_assert(Poco::intToStr(u8, 10, res) && res == "123");

    i16 = 0;
    poco_assert(Poco::intToStr(i16, 10, res) && res == "0");
    i16 = 12345;
    poco_assert(Poco::intToStr(i16, 10, res) && res == "12345");
    i16 = -12345;
    poco_assert(Poco::intToStr(i16, 10, res) && res == "-12345");

    u16 = 0;
    poco_assert(Poco::intToStr(u16, 10, res) && res == "0");
    u16 = 12345;
    poco_assert(Poco::intToStr(u16, 10, res) && res == "12345");

    i32 = 0;
    poco_assert(Poco::intToStr(i32, 10, res) && res == "0");
    i32 = 1234567890;
    poco_assert(Poco::intToStr(i32, 10, res) && res == "1234567890");
    i32 = -1234567890;
    poco_assert(Poco::intToStr(i32, 10, res) && res == "-1234567890");

    u32 = 0;
    poco_assert(Poco::intToStr(u32, 10, res) && res == "0");
    u32 = 1234567890;
    poco_assert(Poco::intToStr(u32, 10, res) && res == "1234567890");

    i64 = 0;
    poco_assert(Poco::intToStr(i64, 10, res) && res == "0");
    i64 = 1234567890987654321;
    poco_assert(Poco::intToStr(i64, 10, res) && res == "1234567890987654321");
    i64 = -1234567890987654321;
    poco_assert(Poco::intToStr(i64, 10, res) && res == "-1234567890987654321");

    u64 = 0;
    poco_assert(Poco::intToStr(u64, 10, res) && res == "0");
    u64 = 1234567890987654321;
    poco_assert(Poco::intToStr(u64, 10, res) && res == "1234567890987654321");

    f32 = 0.f;
    poco_assert(Poco::floatToStr(res, f32) == "0" && res == "0");
    f32 = 0.000001f;
    poco_assert(Poco::floatToStr(res, f32) == "0.000001" && res == "0.000001");
    f32 = 3.141593f;
    poco_assert(Poco::floatToStr(res, f32) == "3.141593" && res == "3.141593");
    f32 = -1234567.f;
    poco_assert(Poco::floatToStr(res, f32) == "-1.234567e+6" && res == "-1.234567e+6");
    f32 = 1234567.f;
    poco_assert(Poco::floatToStr(res, f32) == "1.234567e+6" && res == "1.234567e+6");

    d64 = 0.0;
    poco_assert(Poco::doubleToStr(res, d64) == "0" && res == "0");
    d64 = 0.000000000000001;
    poco_assert(Poco::doubleToStr(res, d64) == "0.000000000000001" && res == "0.000000000000001");
    d64 = 3.141592653589793;
    poco_assert(Poco::doubleToStr(res, d64) == "3.141592653589793" && res == "3.141592653589793");
    d64 = -1234567890123456.0;
    poco_assert(Poco::doubleToStr(res, d64) == "-1.234567890123456e+15" && res == "-1.234567890123456e+15");
    d64 = 1234567890123456.0;
    poco_assert(Poco::doubleToStr(res, d64) == "1.234567890123456e+15" && res == "1.234567890123456e+15");
}

void test_std_numeric_to_str(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    std::string res;

    // cpp11_numeric_string::to_string
    {
        i8 = 0;
        poco_assert(cpp11_numeric_string::to_string(i8) == "0");
        i8 = 123;
        poco_assert(cpp11_numeric_string::to_string(i8) == "123");
        i8 = -123;
        poco_assert(cpp11_numeric_string::to_string(i8) == "-123");

        u8 = 0;
        poco_assert(cpp11_numeric_string::to_string(u8) == "0");
        u8 = 123;
        poco_assert(cpp11_numeric_string::to_string(u8) == "123");

        i16 = 0;
        poco_assert(cpp11_numeric_string::to_string(i16) == "0");
        i16 = 12345;
        poco_assert(cpp11_numeric_string::to_string(i16) == "12345");
        i16 = -12345;
        poco_assert(cpp11_numeric_string::to_string(i16) == "-12345");

        u16 = 0;
        poco_assert(cpp11_numeric_string::to_string(u16) == "0");
        u16 = 12345;
        poco_assert(cpp11_numeric_string::to_string(u16) == "12345");

        i32 = 0;
        poco_assert(cpp11_numeric_string::to_string(i32) == "0");
        i32 = 1234567890;
        poco_assert(cpp11_numeric_string::to_string(i32) == "1234567890");
        i32 = -1234567890;
        poco_assert(cpp11_numeric_string::to_string(i32) == "-1234567890");

        u32 = 0;
        poco_assert(cpp11_numeric_string::to_string(u32) == "0");
        u32 = 1234567890;
        poco_assert(cpp11_numeric_string::to_string(u32) == "1234567890");

        i64 = 0;
        poco_assert(cpp11_numeric_string::to_string(i64) == "0");
        i64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::to_string(i64) == "1234567890987654321");
        i64 = -1234567890987654321;
        poco_assert(cpp11_numeric_string::to_string(i64) == "-1234567890987654321");

        u64 = 0;
        poco_assert(cpp11_numeric_string::to_string(u64) == "0");
        u64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::to_string(u64) == "1234567890987654321");

        f32 = 0.f;
        poco_assert(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 0.000001f;
        poco_assert(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 3.141593f;
        poco_assert(!cpp11_numeric_string::to_string(f32).empty());
        f32 = -1234567.f;
        poco_assert(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 1234567.f;
        poco_assert(!cpp11_numeric_string::to_string(f32).empty());

        d64 = 0.0;
        poco_assert(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 0.000000000000001;
        poco_assert(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 3.141592653589793;
        poco_assert(!cpp11_numeric_string::to_string(d64).empty());
        d64 = -1234567890123456.0;
        poco_assert(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 1234567890123456.0;
        poco_assert(!cpp11_numeric_string::to_string(d64).empty());
    }

    // cpp11_numeric_string::*_to_str
    {
        i8 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "0");
        i8 = 123;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "123");
        i8 = -123;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "-123");

        u8 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u8, res) && res == "0");
        u8 = 123;
        poco_assert(cpp11_numeric_string::int_to_str(u8, res) && res == "123");

        i16 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "0");
        i16 = 12345;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "12345");
        i16 = -12345;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "-12345");

        u16 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u16, res) && res == "0");
        u16 = 12345;
        poco_assert(cpp11_numeric_string::int_to_str(u16, res) && res == "12345");

        i32 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "0");
        i32 = 1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "1234567890");
        i32 = -1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "-1234567890");

        u32 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u32, res) && res == "0");
        u32 = 1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(u32, res) && res == "1234567890");

        i64 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "0");
        i64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "1234567890987654321");
        i64 = -1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "-1234567890987654321");

        u64 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u64, res) && res == "0");
        u64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(u64, res) && res == "1234567890987654321");

        f32 = 0.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 0.000001f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 3.141593f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = -1234567.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 1234567.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());

        d64 = 0.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 0.000000000000001;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 3.141592653589793;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = -1234567890123456.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 1234567890123456.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
    }
}

void test_benchmark_str_to_numeric(void)
{
    int8_t   i8    = 0;
    uint8_t  u8    = 0;
    int16_t  i16   = 0;
    uint16_t u16   = 0;
    int32_t  i32   = 0;
    uint32_t u32   = 0;
    int64_t  i64   = 0;
    uint64_t u64   = 0;
    float    f32   = 0.f;
    double   d64   = 0.0;
    int      total = 10000;

    Poco::Stopwatch sw;

    // Poco::strToInt
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        poco_assert(Poco::strToInt("0", i8, 10) && i8 == 0);
        poco_assert(Poco::strToInt("123", i8, 10) && i8 == 123);
        poco_assert(Poco::strToInt("-123", i8, 10) && i8 == -123);

        poco_assert(Poco::strToInt("0", u8, 10) && u8 == 0);
        poco_assert(Poco::strToInt("123", u8, 10) && u8 == 123);

        poco_assert(Poco::strToInt("0", i16, 10) && i16 == 0);
        poco_assert(Poco::strToInt("12345", i16, 10) && i16 == 12345);
        poco_assert(Poco::strToInt("-12345", i16, 10) && i16 == -12345);

        poco_assert(Poco::strToInt("0", u16, 10) && u16 == 0);
        poco_assert(Poco::strToInt("12345", u16, 10) && u16 == 12345);

        poco_assert(Poco::strToInt("0", i32, 10) && i32 == 0);
        poco_assert(Poco::strToInt("1234567890", i32, 10) && i32 == 1234567890);
        poco_assert(Poco::strToInt("-1234567890", i32, 10) && i32 == -1234567890);

        poco_assert(Poco::strToInt("0", u32, 10) && u32 == 0);
        poco_assert(Poco::strToInt("1234567890", u32, 10) && u32 == 1234567890);

        poco_assert(Poco::strToInt("0", i64, 10) && i64 == 0);
        poco_assert(Poco::strToInt("1234567890987654321", i64, 10) && i64 == 1234567890987654321);
        poco_assert(Poco::strToInt("-1234567890987654321", i64, 10) && i64 == -1234567890987654321);

        poco_assert(Poco::strToInt("0", u64, 10) && u64 == 0);
        poco_assert(Poco::strToInt("1234567890987654321", u64, 10) && u64 == 1234567890987654321);
    }
    sw.stop();
    fmt::print("Poco::strToInt(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // Poco::strToFloat
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        poco_assert(Poco::strToFloat("0", f32) && f32 == 0.f);
        poco_assert(Poco::strToFloat("-1234567", f32) && f32 == -1234567.f);
        poco_assert(Poco::strToFloat("1234567", f32) && f32 == 1234567.f);

        poco_assert(Poco::strToDouble("0", d64) && d64 == 0.0);
        poco_assert(Poco::strToDouble("-1234567890123456", d64) && d64 == -1234567890123456.0);
        poco_assert(Poco::strToDouble("1234567890123456", d64) && d64 == 1234567890123456.0);
    }
    sw.stop();
    fmt::print("Poco::strToFloat(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // cpp11_numeric_string::str_to_int
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        poco_assert(cpp11_numeric_string::str_to_int("0", i8) && i8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("123", i8) && i8 == 123);
        poco_assert(cpp11_numeric_string::str_to_int("-123", i8) && i8 == -123);

        poco_assert(cpp11_numeric_string::str_to_int("0", u8) && u8 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("123", u8) && u8 == 123);

        poco_assert(cpp11_numeric_string::str_to_int("0", i16) && i16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("12345", i16) && i16 == 12345);
        poco_assert(cpp11_numeric_string::str_to_int("-12345", i16) && i16 == -12345);

        poco_assert(cpp11_numeric_string::str_to_int("0", u16) && u16 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("12345", u16) && u16 == 12345);

        poco_assert(cpp11_numeric_string::str_to_int("0", i32) && i32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890", i32) && i32 == 1234567890);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890", i32) && i32 == -1234567890);

        poco_assert(cpp11_numeric_string::str_to_int("0", u32) && u32 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890", u32) && u32 == 1234567890);

        poco_assert(cpp11_numeric_string::str_to_int("0", i64) && i64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890987654321", i64) && i64 == 1234567890987654321);
        poco_assert(cpp11_numeric_string::str_to_int("-1234567890987654321", i64) && i64 == -1234567890987654321);

        poco_assert(cpp11_numeric_string::str_to_int("0", u64) && u64 == 0);
        poco_assert(cpp11_numeric_string::str_to_int("1234567890987654321", u64) && u64 == 1234567890987654321);
    }
    sw.stop();
    fmt::print("cpp11_numeric_string::str_to_int(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // cpp11_numeric_string::str_to_float
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        poco_assert(cpp11_numeric_string::str_to_float("0", f32) && f32 == 0.f);
        poco_assert(cpp11_numeric_string::str_to_float("-1234567", f32) && f32 == -1234567.f);
        poco_assert(cpp11_numeric_string::str_to_float("1234567", f32) && f32 == 1234567.f);

        poco_assert(cpp11_numeric_string::str_to_double("0", d64) && d64 == 0.0);
        poco_assert(cpp11_numeric_string::str_to_double("-1234567890123456", d64) && d64 == -1234567890123456.0);
        poco_assert(cpp11_numeric_string::str_to_double("1234567890123456", d64) && d64 == 1234567890123456.0);
    }
    sw.stop();
    fmt::print("cpp11_numeric_string::str_to_float(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());
}

void test_benchmark_numeric_to_str(void)
{
    int8_t   i8    = 0;
    uint8_t  u8    = 0;
    int16_t  i16   = 0;
    uint16_t u16   = 0;
    int32_t  i32   = 0;
    uint32_t u32   = 0;
    int64_t  i64   = 0;
    uint64_t u64   = 0;
    float    f32   = 0.f;
    double   d64   = 0.0;
    int      total = 10000;

    std::string     res;
    Poco::Stopwatch sw;

    // Poco::intToStr
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        i8 = 0;
        poco_assert(Poco::intToStr(i8, 10, res) && res == "0");
        i8 = 123;
        poco_assert(Poco::intToStr(i8, 10, res) && res == "123");
        i8 = -123;
        poco_assert(Poco::intToStr(i8, 10, res) && res == "-123");

        u8 = 0;
        poco_assert(Poco::intToStr(u8, 10, res) && res == "0");
        u8 = 123;
        poco_assert(Poco::intToStr(u8, 10, res) && res == "123");

        i16 = 0;
        poco_assert(Poco::intToStr(i16, 10, res) && res == "0");
        i16 = 12345;
        poco_assert(Poco::intToStr(i16, 10, res) && res == "12345");
        i16 = -12345;
        poco_assert(Poco::intToStr(i16, 10, res) && res == "-12345");

        u16 = 0;
        poco_assert(Poco::intToStr(u16, 10, res) && res == "0");
        u16 = 12345;
        poco_assert(Poco::intToStr(u16, 10, res) && res == "12345");

        i32 = 0;
        poco_assert(Poco::intToStr(i32, 10, res) && res == "0");
        i32 = 1234567890;
        poco_assert(Poco::intToStr(i32, 10, res) && res == "1234567890");
        i32 = -1234567890;
        poco_assert(Poco::intToStr(i32, 10, res) && res == "-1234567890");

        u32 = 0;
        poco_assert(Poco::intToStr(u32, 10, res) && res == "0");
        u32 = 1234567890;
        poco_assert(Poco::intToStr(u32, 10, res) && res == "1234567890");

        i64 = 0;
        poco_assert(Poco::intToStr(i64, 10, res) && res == "0");
        i64 = 1234567890987654321;
        poco_assert(Poco::intToStr(i64, 10, res) && res == "1234567890987654321");
        i64 = -1234567890987654321;
        poco_assert(Poco::intToStr(i64, 10, res) && res == "-1234567890987654321");

        u64 = 0;
        poco_assert(Poco::intToStr(u64, 10, res) && res == "0");
        u64 = 1234567890987654321;
        poco_assert(Poco::intToStr(u64, 10, res) && res == "1234567890987654321");
    }
    sw.stop();
    fmt::print("Poco::intToStr(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // Poco::floatToStr
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        f32 = 0.f;
        poco_assert(!Poco::floatToStr(res, f32).empty());
        f32 = -1234567.f;
        poco_assert(!Poco::floatToStr(res, f32).empty());
        f32 = 1234567.f;
        poco_assert(!Poco::floatToStr(res, f32).empty());

        d64 = 0.0;
        poco_assert(!Poco::doubleToStr(res, d64).empty());
        d64 = -1234567890123456.0;
        poco_assert(!Poco::doubleToStr(res, d64).empty());
        d64 = 1234567890123456.0;
        poco_assert(!Poco::doubleToStr(res, d64).empty());
    }
    sw.stop();
    fmt::print("Poco::floatToStr(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // cpp11_numeric_string::int_to_str
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        i8 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "0");
        i8 = 123;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "123");
        i8 = -123;
        poco_assert(cpp11_numeric_string::int_to_str(i8, res) && res == "-123");

        u8 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u8, res) && res == "0");
        u8 = 123;
        poco_assert(cpp11_numeric_string::int_to_str(u8, res) && res == "123");

        i16 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "0");
        i16 = 12345;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "12345");
        i16 = -12345;
        poco_assert(cpp11_numeric_string::int_to_str(i16, res) && res == "-12345");

        u16 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u16, res) && res == "0");
        u16 = 12345;
        poco_assert(cpp11_numeric_string::int_to_str(u16, res) && res == "12345");

        i32 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "0");
        i32 = 1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "1234567890");
        i32 = -1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(i32, res) && res == "-1234567890");

        u32 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u32, res) && res == "0");
        u32 = 1234567890;
        poco_assert(cpp11_numeric_string::int_to_str(u32, res) && res == "1234567890");

        i64 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "0");
        i64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "1234567890987654321");
        i64 = -1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(i64, res) && res == "-1234567890987654321");

        u64 = 0;
        poco_assert(cpp11_numeric_string::int_to_str(u64, res) && res == "0");
        u64 = 1234567890987654321;
        poco_assert(cpp11_numeric_string::int_to_str(u64, res) && res == "1234567890987654321");
    }
    sw.stop();
    fmt::print("cpp11_numeric_string::int_to_str(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());

    // cpp11_numeric_string::float_to_str
    sw.restart();
    for (int idx = 0; idx != total; ++idx)
    {
        f32 = 0.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = -1234567.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 1234567.f;
        poco_assert(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());

        d64 = 0.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = -1234567890123456.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 1234567890123456.0;
        poco_assert(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
    }
    sw.stop();
    fmt::print("cpp11_numeric_string::float_to_str(), total={}, elapsed {} microseconds.\n", total, sw.elapsed());
}

void test_number_formatter(void)
{
    {
        int8_t   i8  = 0;
        uint8_t  u8  = 0;
        int16_t  i16 = 0;
        uint16_t u16 = 0;
        int32_t  i32 = 0;
        uint32_t u32 = 0;
        int64_t  i64 = 0;
        uint64_t u64 = 0;
        float    f32 = 0.f;
        double   d64 = 0.0;

        std::string res;

        i8 = 0;
        poco_assert(Poco::NumberFormatter::format(i8) == "0");
        i8 = 123;
        poco_assert(Poco::NumberFormatter::format(i8) == "123");
        i8 = -123;
        poco_assert(Poco::NumberFormatter::format(i8) == "-123");

        u8 = 0;
        poco_assert(Poco::NumberFormatter::format(u8) == "0");
        u8 = 123;
        poco_assert(Poco::NumberFormatter::format(u8) == "123");

        i16 = 0;
        poco_assert(Poco::NumberFormatter::format(i16) == "0");
        i16 = 12345;
        poco_assert(Poco::NumberFormatter::format(i16) == "12345");
        i16 = -12345;
        poco_assert(Poco::NumberFormatter::format(i16) == "-12345");

        u16 = 0;
        poco_assert(Poco::NumberFormatter::format(u16) == "0");
        u16 = 12345;
        poco_assert(Poco::NumberFormatter::format(u16) == "12345");

        i32 = 0;
        poco_assert(Poco::NumberFormatter::format(i32) == "0");
        i32 = 1234567890;
        poco_assert(Poco::NumberFormatter::format(i32) == "1234567890");
        i32 = -1234567890;
        poco_assert(Poco::NumberFormatter::format(i32) == "-1234567890");

        u32 = 0;
        poco_assert(Poco::NumberFormatter::format(u32) == "0");
        u32 = 1234567890;
        poco_assert(Poco::NumberFormatter::format(u32) == "1234567890");

        i64 = 0;
        poco_assert(Poco::NumberFormatter::format(i64) == "0");
        i64 = 1234567890987654321;
        poco_assert(Poco::NumberFormatter::format(i64) == "1234567890987654321");
        i64 = -1234567890987654321;
        poco_assert(Poco::NumberFormatter::format(i64) == "-1234567890987654321");

        u64 = 0;
        poco_assert(Poco::NumberFormatter::format(u64) == "0");
        u64 = 1234567890987654321;
        poco_assert(Poco::NumberFormatter::format(u64) == "1234567890987654321");

        f32 = 0.f;
        poco_assert(Poco::NumberFormatter::format(f32) == "0");
        f32 = 0.000001f;
        poco_assert(Poco::NumberFormatter::format(f32) == "0.000001");
        f32 = 3.141593f;
        poco_assert(Poco::NumberFormatter::format(f32) == "3.141593");
        f32 = -1234567.f;
        poco_assert(Poco::NumberFormatter::format(f32) == "-1.234567e+6");
        f32 = 1234567.f;
        poco_assert(Poco::NumberFormatter::format(f32) == "1.234567e+6");

        d64 = 0.0;
        poco_assert(Poco::NumberFormatter::format(d64) == "0");
        d64 = 0.000000000000001;
        poco_assert(Poco::NumberFormatter::format(d64) == "0.000000000000001");
        d64 = 3.141592653589793;
        poco_assert(Poco::NumberFormatter::format(d64) == "3.141592653589793");
        d64 = -1234567890123456.0;
        poco_assert(Poco::NumberFormatter::format(d64) == "-1.234567890123456e+15");
        d64 = 1234567890123456.0;
        poco_assert(Poco::NumberFormatter::format(d64) == "1.234567890123456e+15");
    }

    {
        poco_assert(Poco::NumberFormatter::format((int8_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((int8_t)123, 5) == "  123");
        poco_assert(Poco::NumberFormatter::format((int8_t)-123) == "-123");
        poco_assert(Poco::NumberFormatter::format((int8_t)-123, 5) == " -123");

        poco_assert(Poco::NumberFormatter::format((uint8_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((uint8_t)123, 5) == "  123");

        poco_assert(Poco::NumberFormatter::format((int16_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((int16_t)123, 5) == "  123");
        poco_assert(Poco::NumberFormatter::format((int16_t)-123) == "-123");
        poco_assert(Poco::NumberFormatter::format((int16_t)-123, 5) == " -123");

        poco_assert(Poco::NumberFormatter::format((uint16_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((uint16_t)123, 5) == "  123");

        poco_assert(Poco::NumberFormatter::format((int32_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((int32_t)123, 5) == "  123");
        poco_assert(Poco::NumberFormatter::format((int32_t)-123) == "-123");
        poco_assert(Poco::NumberFormatter::format((int32_t)-123, 5) == " -123");

        poco_assert(Poco::NumberFormatter::format((uint32_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((uint32_t)123, 5) == "  123");

        poco_assert(Poco::NumberFormatter::format((int64_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((int64_t)123, 5) == "  123");
        poco_assert(Poco::NumberFormatter::format((int64_t)-123) == "-123");
        poco_assert(Poco::NumberFormatter::format((int64_t)-123, 5) == " -123");

        poco_assert(Poco::NumberFormatter::format((uint64_t)123) == "123");
        poco_assert(Poco::NumberFormatter::format((uint64_t)123, 5) == "  123");

        poco_assert(Poco::NumberFormatter::format(1.23f) == "1.23");
        poco_assert(Poco::NumberFormatter::format(-1.23f) == "-1.23");
        poco_assert(Poco::NumberFormatter::format(1.0f) == "1");
        poco_assert(Poco::NumberFormatter::format(-1.0f) == "-1");
        poco_assert(Poco::NumberFormatter::format(0.1f) == "0.1");
        poco_assert(Poco::NumberFormatter::format(-0.1f) == "-0.1");

        poco_assert(Poco::NumberFormatter::format(1.23) == "1.23");
        poco_assert(Poco::NumberFormatter::format(-1.23) == "-1.23");
        poco_assert(Poco::NumberFormatter::format(1.0) == "1");
        poco_assert(Poco::NumberFormatter::format(-1.0) == "-1");
        poco_assert(Poco::NumberFormatter::format(0.1) == "0.1");
        poco_assert(Poco::NumberFormatter::format(-0.1) == "-0.1");

        poco_assert(Poco::NumberFormatter::format(50.0f, 3) == "50.000");
        poco_assert(Poco::NumberFormatter::format(50.123f, 3) == "50.123");
        poco_assert(Poco::NumberFormatter::format(50.123f, 0) == "50");
        poco_assert(Poco::NumberFormatter::format(50.546f, 0) == "51");
        poco_assert(Poco::NumberFormatter::format(50.546f, 2) == "50.55");

        poco_assert(Poco::NumberFormatter::format(50.0, 3) == "50.000");
        poco_assert(Poco::NumberFormatter::format(50.123, 3) == "50.123");
        poco_assert(Poco::NumberFormatter::format(50.123, 0) == "50");
        poco_assert(Poco::NumberFormatter::format(50.546, 0) == "51");
        poco_assert(Poco::NumberFormatter::format(50.546, 2) == "50.55");
    }

    {
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0x12) == "12");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0xab) == "AB");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0x12, 4) == "0012");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0xab, 4) == "00AB");

        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0x12) == "12");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0xab) == "AB");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4) == "0012");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4) == "00AB");

        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0x12) == "12");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0xab) == "AB");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0x12, 4) == "0012");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0xab, 4) == "00AB");

        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0x12) == "12");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0xab) == "AB");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4) == "0012");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4) == "00AB");
    }

    {
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0x12, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0xab, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0x12, 4, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0xab, 4, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0x12, 6, true) == "0x0012");
        poco_assert(Poco::NumberFormatter::formatHex((int32_t)0xab, 6, true) == "0x00AB");

        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0x12, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0xab, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0x12, 6, true) == "0x0012");
        poco_assert(Poco::NumberFormatter::formatHex((uint32_t)0xab, 6, true) == "0x00AB");

        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0x12, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0xab, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0x12, 4, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0xab, 4, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0x12, 6, true) == "0x0012");
        poco_assert(Poco::NumberFormatter::formatHex((int64_t)0xab, 6, true) == "0x00AB");

        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0x12, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0xab, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4, true) == "0x12");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4, true) == "0xAB");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0x12, 6, true) == "0x0012");
        poco_assert(Poco::NumberFormatter::formatHex((uint64_t)0xab, 6, true) == "0x00AB");
    }
}

void test_number_parser(void)
{
    poco_assert(Poco::NumberParser::parseBool("1") == true);
    poco_assert(Poco::NumberParser::parseBool("0") == false);
    poco_assert(Poco::NumberParser::parseBool("YeS") == true);
    poco_assert(Poco::NumberParser::parseBool("No") == false);
    poco_assert(Poco::NumberParser::parseBool("TRue") == true);
    poco_assert(Poco::NumberParser::parseBool("FAlse") == false);

    poco_assert(Poco::NumberParser::parse("+123") == 123);
    poco_assert(Poco::NumberParser::parse("-123") == -123);
    poco_assert(Poco::NumberParser::parse("0") == 0);
    poco_assert(Poco::NumberParser::parse("000") == 0);
    poco_assert(Poco::NumberParser::parse("0123") == 123);
    poco_assert(Poco::NumberParser::parse("+0123") == 123);
    poco_assert(Poco::NumberParser::parse("-0123") == -123);

    poco_assert(Poco::NumberParser::parseUnsigned("123") == 123);

    poco_assert(Poco::NumberParser::parseHex("12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex("0x12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex("0X12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex("0x000012AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex("0X000012AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex("00") == 0);

    poco_assert(Poco::NumberParser::parse64("+123") == 123);
    poco_assert(Poco::NumberParser::parse64("-123") == -123);
    poco_assert(Poco::NumberParser::parse64("0") == 0);
    poco_assert(Poco::NumberParser::parse64("000") == 0);
    poco_assert(Poco::NumberParser::parse64("0123") == 123);
    poco_assert(Poco::NumberParser::parse64("+0123") == 123);
    poco_assert(Poco::NumberParser::parse64("-0123") == -123);

    poco_assert(Poco::NumberParser::parseUnsigned64("123") == 123);

    poco_assert(Poco::NumberParser::parseHex64("12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex64("0x12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex64("0X12AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex64("0x000012AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex64("0X000012AB") == 0x12ab);
    poco_assert(Poco::NumberParser::parseHex64("00") == 0);

    poco_assert(Poco::NumberParser::parseFloat("50.000") == 50.0);
    poco_assert(Poco::NumberParser::parseFloat("+50.000") == 50.0);
    poco_assert(Poco::NumberParser::parseFloat("-50.000") == -50.0);
    poco_assert(Poco::NumberParser::parseFloat("50.123") == 50.123);
    poco_assert(Poco::NumberParser::parseFloat("+50.123") == 50.123);
    poco_assert(Poco::NumberParser::parseFloat("-50.123") == -50.123);
}

void test_numeric_string_all(void)
{
    test_poco_str_to_numeric();
    test_std_str_to_numeric();
    test_poco_numeric_to_str();
    test_std_numeric_to_str();
    test_benchmark_str_to_numeric();
    test_benchmark_numeric_to_str();
    test_number_formatter();
    test_number_parser();
}