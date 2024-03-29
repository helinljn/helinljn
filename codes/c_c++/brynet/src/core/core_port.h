#ifndef __CORE_PORT_H__
#define __CORE_PORT_H__

#if (defined(_MSC_VER) && !defined(__INTEL_COMPILER)) || defined(_WIN32) || defined(_WIN64)
    #if !defined(CORE_PLATFORM_WINDOWS)
        #define CORE_PLATFORM_WINDOWS
    #endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #if !defined(CORE_PLATFORM_LINUX)
        #define CORE_PLATFORM_LINUX
    #endif
#else
    #error "Unrecognized os platform!"
#endif

#if defined(CORE_PLATFORM_WINDOWS)
    #if defined(CORE_LIB_EXPORT)
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    #if defined(CORE_LIB_EXPORT)
        #define CORE_API __attribute__((visibility("default")))
    #else
        #define CORE_API
    #endif
#endif

#endif // __CORE_PORT_H__