#ifndef __SPDLOG_H__
#define __SPDLOG_H__

#include "brynet/base/Platform.hpp"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4275)
#endif

#include "fmt/core.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(pop)
#endif

#endif // !__SPDLOG_H__