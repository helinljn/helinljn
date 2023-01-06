#ifndef __COMMON_PLATFORM_H__
#define __COMMON_PLATFORM_H__

#if (defined(_MSC_VER) && !defined(__INTEL_COMPILER)) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    #if !defined(__COMMON_WINDOWS__)
        #define __COMMON_WINDOWS__

        #if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0601)
            #undef  _WIN32_WINNT
            #define _WIN32_WINNT 0x0601
        #endif // !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0601)
    #endif // !defined(__COMMON_WINDOWS__)
#elif defined(__linux__)
    #if !defined(__COMMON_LINUX__)
        #define __COMMON_LINUX__
    #endif // !defined(__COMMON_LINUX__)
#else
    #error "Unrecognized OS Platform!"
#endif // (defined(_MSC_VER) && !defined(__INTEL_COMPILER)) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)

#endif // __COMMON_PLATFORM_H__