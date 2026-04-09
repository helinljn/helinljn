#pragma once

#ifndef BRYNET_H
#define BRYNET_H

#include "core_port.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4245)
    #pragma warning(disable:4267)
    #pragma warning(disable:4456)
    #pragma warning(disable:4505)
    #pragma warning(disable:4996)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif // defined(CORE_PLATFORM_WINDOWS)

#include "brynet/Version.hpp"
#include "brynet/base/AppStatus.hpp"
#include "brynet/base/WaitGroup.hpp"
#include "brynet/net/AsyncConnector.hpp"
// #include <brynet/net/PromiseReceive.hpp>
#include "brynet/net/http/HttpFormat.hpp"
#include "brynet/net/http/HttpService.hpp"
#include "brynet/net/wrapper/ServiceBuilder.hpp"
#include "brynet/net/wrapper/ConnectionBuilder.hpp"
#include "brynet/net/wrapper/HttpServiceBuilder.hpp"
#include "brynet/net/wrapper/HttpConnectionBuilder.hpp"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif // defined(CORE_PLATFORM_WINDOWS)

#endif // BRYNET_H