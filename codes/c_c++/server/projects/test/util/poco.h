#ifndef __POCO_H__
#define __POCO_H__

#include "Poco/Platform.h"

#if defined(POCO_COMPILER_MSVC)
    #pragma warning(push)
    #pragma warning(disable:4100)
    #pragma warning(disable:4244)
#elif defined(POCO_COMPILER_GCC)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "Poco/NumericString.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Pair.h"

#if defined(POCO_COMPILER_MSVC)
    #pragma warning(pop)
#elif defined(POCO_COMPILER_GCC)
    #pragma GCC diagnostic pop
#endif

#endif // __POCO_H__