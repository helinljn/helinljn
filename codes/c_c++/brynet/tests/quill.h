#ifndef __QUILL_H__
#define __QUILL_H__

#include "brynet/base/Platform.hpp"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4003)
    #pragma warning(disable:4324)
#elif defined(BRYNET_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "quill/Quill.h"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(BRYNET_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif

#endif // __QUILL_H__