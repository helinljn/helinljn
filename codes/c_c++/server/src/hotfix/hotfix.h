#ifndef __HOTFIX_H__
#define __HOTFIX_H__

#include "util/poco.h"
#include "Poco/Foundation.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #ifdef HOTFIX_EXPORTS
        #define HOTFIX_API __declspec(dllexport)
    #else
        #define HOTFIX_API __declspec(dllimport)
    #endif
#else
    #ifdef HOTFIX_EXPORTS
        #define HOTFIX_API __attribute__((visibility("default")))
    #else
        #define HOTFIX_API
    #endif
#endif

HOTFIX_API std::string hotfix_test_func(int32_t val);
HOTFIX_API std::string hotfix_test_func_patch(int32_t val);

#endif // __HOTFIX_H__