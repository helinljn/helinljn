#ifndef __HOOK_H__
#define __HOOK_H__

#include "core_port.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4100)
    #pragma warning(disable:4505)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wpointer-arith"
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // defined(CORE_PLATFORM_WINDOWS)

#include "LightHook.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif // defined(CORE_PLATFORM_WINDOWS)

#endif // __HOOK_H__