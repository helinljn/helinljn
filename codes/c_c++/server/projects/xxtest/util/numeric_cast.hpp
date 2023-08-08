#ifndef __NUMERIC_CAST_HPP__
#define __NUMERIC_CAST_HPP__

#include <string>
#include <type_traits>

namespace numeric_cast {
namespace details      {

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

} // namespace numeric_cast

#endif // __NUMERIC_CAST_HPP__