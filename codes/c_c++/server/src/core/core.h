#pragma once

#ifndef CORE_H
#define CORE_H

#include "Poco/Platform.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #define CORE_PLATFORM_WINDOWS
#elif POCO_OS == POCO_OS_LINUX
    #define CORE_PLATFORM_LINUX
#else
    #error "Unrecognized os platform!"
#endif // POCO_OS == POCO_OS_WINDOWS_NT

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
#endif // defined(CORE_PLATFORM_WINDOWS)

#endif // CORE_H
