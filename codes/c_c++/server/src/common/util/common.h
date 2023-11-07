#ifndef __COMMON_H__
#define __COMMON_H__

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

#endif // __COMMON_H__