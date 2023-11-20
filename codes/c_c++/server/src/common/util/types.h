#ifndef __TYPES_H__
#define __TYPES_H__

#include "Poco/Platform.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #if defined(COMMON_EXPORTS)
        #define COMMON_API __declspec(dllexport)
    #else
        #define COMMON_API __declspec(dllimport)
    #endif
#else
    #if defined(COMMON_EXPORTS)
        #define COMMON_API __attribute__((visibility("default")))
    #else
        #define COMMON_API
    #endif
#endif

#if defined(POCO_COMPILER_MSVC)
    #pragma warning(push)
    #pragma warning(disable:4100)
    #pragma warning(disable:4244)
#elif defined(POCO_COMPILER_GCC)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "Poco/NumericString.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Pair.h"

#if defined(POCO_COMPILER_MSVC)
    #pragma warning(pop)
#elif defined(POCO_COMPILER_GCC)
    #pragma GCC diagnostic pop
#endif

#include <array>
#include <deque>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <numeric>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace common {

using int8    = Poco::Int8;
using uint8   = Poco::UInt8;
using int16   = Poco::Int16;
using uint16  = Poco::UInt16;
using int32   = Poco::Int32;
using uint32  = Poco::UInt32;
using int64   = Poco::Int64;
using uint64  = Poco::UInt64;
using intptr  = Poco::IntPtr;
using uintptr = Poco::UIntPtr;

} // namespace common

#endif // __TYPES_H__