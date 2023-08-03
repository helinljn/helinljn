#ifndef __POCO_NUMERIC_CAST_HPP__
#define __POCO_NUMERIC_CAST_HPP__

#include "Poco/NumericString.h"
#include <type_traits>

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
template <typename To, typename Dummy = std::enable_if_t<std::is_integral_v<To> || std::is_floating_point_v<To>>>
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
template <typename From, typename Dummy = std::enable_if_t<std::is_integral_v<From> || std::is_floating_point_v<From>>>
std::string to_string(const From from)
{
    try
    {
        return details::converter<std::string, From>::convert(from);
    }
    catch(...)
    {
        return std::string{};
    }
}

} // namespace poco_numeric_cast

#endif // __POCO_NUMERIC_CAST_HPP__