#ifndef __QUILL_H__
#define __QUILL_H__

#include "core_port.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4003)
    #pragma warning(disable:4324)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "quill/Quill.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif

#endif // __QUILL_H__