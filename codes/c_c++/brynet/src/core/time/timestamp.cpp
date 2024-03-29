#include "timestamp.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #include <Windows.h>
#elif defined(CORE_PLATFORM_LINUX)
    #include <sys/time.h>
#endif

namespace core {

void timestamp::update(void)
{
#if defined(CORE_PLATFORM_WINDOWS)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER epoch;
    epoch.LowPart  = 0xD53E8000;
    epoch.HighPart = 0x019DB1DE;

    ULARGE_INTEGER ts;
    ts.LowPart   = ft.dwLowDateTime;
    ts.HighPart  = ft.dwHighDateTime;
    ts.QuadPart -= epoch.QuadPart;

    _tsval = ts.QuadPart / 10;
#elif defined(CORE_PLATFORM_LINUX)
    #if defined(_POSIX_TIMERS) && defined(CLOCK_REALTIME)
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
            _tsval = resolution * ts.tv_sec + ts.tv_nsec / 1000;
    #else
        struct timeval tv;
        if (gettimeofday(&tv, NULL) == 0)
            _tsval = resolution * tv.tv_sec + tv.tv_usec;
    #endif
#endif
}

} // namespace core