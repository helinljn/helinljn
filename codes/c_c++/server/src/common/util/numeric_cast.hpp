#ifndef __NUMERIC_CAST_HPP__
#define __NUMERIC_CAST_HPP__

#include "util/types.h"

namespace common  {
namespace details {

template <typename To, typename From>
struct converter
{
};

// string to int8
template <>
struct converter<int8, std::string>
{
    static int8 convert(const std::string& from)
    {
        return static_cast<int8>(std::stoi(from));
    }
};

// string to uint8
template <>
struct converter<uint8, std::string>
{
    static uint8 convert(const std::string& from)
    {
        return static_cast<uint8>(std::stoi(from));
    }
};

// string to int16
template <>
struct converter<int16, std::string>
{
    static int16 convert(const std::string& from)
    {
        return static_cast<int16>(std::stoi(from));
    }
};

// string to uint16
template <>
struct converter<uint16, std::string>
{
    static uint16 convert(const std::string& from)
    {
        return static_cast<uint16>(std::stoi(from));
    }
};

// string to int32
template <>
struct converter<int32, std::string>
{
    static int32 convert(const std::string& from)
    {
        return static_cast<int32>(std::stol(from));
    }
};

// string to uint32
template <>
struct converter<uint32, std::string>
{
    static uint32 convert(const std::string& from)
    {
        return static_cast<uint32>(std::stoul(from));
    }
};

// string to int64
template <>
struct converter<int64, std::string>
{
    static int64 convert(const std::string& from)
    {
        return static_cast<int64>(std::stoll(from));
    }
};

// string to uint64
template <>
struct converter<uint64, std::string>
{
    static uint64 convert(const std::string& from)
    {
        return static_cast<uint64>(std::stoull(from));
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

// int8 to string
template<>
struct converter<std::string, int8>
{
    static std::string convert(const int8 from)
    {
        return std::to_string(from);
    }
};

// uint8 to string
template<>
struct converter<std::string, uint8>
{
    static std::string convert(const uint8 from)
    {
        return std::to_string(from);
    }
};

// int16 to string
template<>
struct converter<std::string, int16>
{
    static std::string convert(const int16 from)
    {
        return std::to_string(from);
    }
};

// uint16 to string
template<>
struct converter<std::string, uint16>
{
    static std::string convert(const uint16 from)
    {
        return std::to_string(from);
    }
};

// int32 to string
template<>
struct converter<std::string, int32>
{
    static std::string convert(const int32 from)
    {
        return std::to_string(from);
    }
};

// uint32 to string
template<>
struct converter<std::string, uint32>
{
    static std::string convert(const uint32 from)
    {
        return std::to_string(from);
    }
};

// int64 to string
template<>
struct converter<std::string, int64>
{
    static std::string convert(const int64 from)
    {
        return std::to_string(from);
    }
};

// uint64 to string
template<>
struct converter<std::string, uint64>
{
    static std::string convert(const uint64 from)
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
template <typename To>
To from_string(const std::string& from)
{
    static_assert(std::is_arithmetic_v<To>, "Invalid type!");

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
template <typename From>
std::string to_string(const From from)
{
    static_assert(std::is_arithmetic_v<From>, "Invalid type!");
    return details::converter<std::string, From>::convert(from);
}

} // namespace common

#endif // __NUMERIC_CAST_HPP__