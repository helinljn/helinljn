#pragma once

#ifndef NUMERIC_CAST_HPP
#define NUMERIC_CAST_HPP

#include "core_port.h"
#include <string>
#include <type_traits>

namespace core    {
namespace details {

template <typename To, typename From>
struct converter
{
};

// numeric to string converter
#define DECLARE_INT_TO_STRING_CONVERTER(Type) \
template<>                                    \
struct converter<std::string, Type>           \
{                                             \
    static std::string convert(Type from)     \
    {                                         \
        return std::to_string(from);          \
    }                                         \
};

// string to numeric converter
#define DECLARE_STRING_TO_INT_CONVERTER(IntType, StoxFunc) \
template<>                                                 \
struct converter<IntType, std::string>                     \
{                                                          \
    static IntType convert(const std::string& from)        \
    {                                                      \
        return static_cast<IntType>(StoxFunc(from));       \
    }                                                      \
};

// numeric to string
DECLARE_INT_TO_STRING_CONVERTER(int8_t)
DECLARE_INT_TO_STRING_CONVERTER(uint8_t)
DECLARE_INT_TO_STRING_CONVERTER(int16_t)
DECLARE_INT_TO_STRING_CONVERTER(uint16_t)
DECLARE_INT_TO_STRING_CONVERTER(int32_t)
DECLARE_INT_TO_STRING_CONVERTER(uint32_t)
DECLARE_INT_TO_STRING_CONVERTER(int64_t)
DECLARE_INT_TO_STRING_CONVERTER(uint64_t)
DECLARE_INT_TO_STRING_CONVERTER(float)
DECLARE_INT_TO_STRING_CONVERTER(double)
DECLARE_INT_TO_STRING_CONVERTER(long double)

// string to numeric
DECLARE_STRING_TO_INT_CONVERTER(int8_t,      std::stoi)
DECLARE_STRING_TO_INT_CONVERTER(uint8_t,     std::stoul)
DECLARE_STRING_TO_INT_CONVERTER(int16_t,     std::stoi)
DECLARE_STRING_TO_INT_CONVERTER(uint16_t,    std::stoul)
DECLARE_STRING_TO_INT_CONVERTER(int32_t,     std::stol)
DECLARE_STRING_TO_INT_CONVERTER(uint32_t,    std::stoul)
DECLARE_STRING_TO_INT_CONVERTER(int64_t,     std::stoll)
DECLARE_STRING_TO_INT_CONVERTER(uint64_t,    std::stoull)
DECLARE_STRING_TO_INT_CONVERTER(float,       std::stof)
DECLARE_STRING_TO_INT_CONVERTER(double,      std::stod)
DECLARE_STRING_TO_INT_CONVERTER(long double, std::stold)

#if defined(CORE_PLATFORM_WINDOWS)
    DECLARE_INT_TO_STRING_CONVERTER(long)
    DECLARE_INT_TO_STRING_CONVERTER(unsigned long)

    DECLARE_STRING_TO_INT_CONVERTER(long,          std::stol)
    DECLARE_STRING_TO_INT_CONVERTER(unsigned long, std::stoul)
#elif defined(CORE_PLATFORM_LINUX)
    DECLARE_INT_TO_STRING_CONVERTER(long long)
    DECLARE_INT_TO_STRING_CONVERTER(unsigned long long)

    DECLARE_STRING_TO_INT_CONVERTER(long long,          std::stoll)
    DECLARE_STRING_TO_INT_CONVERTER(unsigned long long, std::stoull)
#endif // defined(CORE_PLATFORM_WINDOWS)

#undef DECLARE_INT_TO_STRING_CONVERTER
#undef DECLARE_STRING_TO_INT_CONVERTER

} // namespace details

// string to numeric
template <typename To>
To to_numeric(const std::string& from)
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

} // namespace core

#endif // NUMERIC_CAST_HPP