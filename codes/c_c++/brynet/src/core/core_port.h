#ifndef __CORE_PORT_H__
#define __CORE_PORT_H__

#include "brynet/base/Platform.hpp"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #if defined(CORE_LIB_EXPORT)
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#else
    #if defined(CORE_LIB_EXPORT)
        #define CORE_API __attribute__((visibility("default")))
    #else
        #define CORE_API
    #endif
#endif

#endif // __CORE_PORT_H__