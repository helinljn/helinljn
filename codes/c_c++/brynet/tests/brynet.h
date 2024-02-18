#ifndef __BRYNET_H__
#define __BRYNET_H__

#include "brynet/base/Platform.hpp"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4245)
    #pragma warning(disable:4267)
    #pragma warning(disable:4456)
    #pragma warning(disable:4505)
    #pragma warning(disable:4996)
#elif defined(BRYNET_PLATFORM_LINUX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include "brynet/base/AppStatus.hpp"
#include "brynet/net/http/HttpFormat.hpp"
#include "brynet/net/http/HttpService.hpp"
#include "brynet/net/wrapper/ConnectionBuilder.hpp"
#include "brynet/net/wrapper/ServiceBuilder.hpp"
#include "brynet/net/wrapper/HttpConnectionBuilder.hpp"
#include "brynet/net/wrapper/HttpServiceBuilder.hpp"

#if defined(BRYNET_PLATFORM_WINDOWS)
    #pragma warning(pop)
#elif defined(BRYNET_PLATFORM_LINUX)
    #pragma GCC diagnostic pop
#endif

#endif // __BRYNET_H__