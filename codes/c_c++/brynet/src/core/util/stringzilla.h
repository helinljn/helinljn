#ifndef __STRING_ZILLA_H__
#define __STRING_ZILLA_H__

#include "core_port.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4068)
    #pragma warning(disable:4146)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC system_header
    //#pragma GCC diagnostic ignored "-Wunknown-pragmas"
    //#pragma GCC diagnostic ignored "-Wunused-function"
    //#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#include "stringzilla/stringzilla.h"
#include "stringzilla/stringzilla.hpp"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif

namespace strzilla = ashvardanian::stringzilla;

#endif // __STRING_ZILLA_H__