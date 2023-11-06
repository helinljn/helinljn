#ifndef __HOTFIX_H__
#define __HOTFIX_H__

#include "util/poco.h"
#include "Poco/Foundation.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #ifdef HOTFIX_EXPORT
        #define HOTFIX_API __declspec(dllexport)
    #else
        #define HOTFIX_API __declspec(dllimport)
    #endif
#else
    #ifdef HOTFIX_EXPORT
        #define HOTFIX_API __attribute__((visibility("default")))
    #else
        #define HOTFIX_API
    #endif
#endif

std::string HOTFIX_API test_global_func_patch(int32_t val);

#endif // __HOTFIX_H__