#include "spdlog/fmt/fmt.h"
#include "util/time.h"

//#include "asio.hpp"
//#include "asio/ssl.hpp"
//
//#include "date/date.h"
//#include "date/iso_week.h"
//
//#include "openssl/ssl.h"
//#include "hiredis/hiredis.h"

int main(void)
{
    // size check
    {
        static_assert(1 == sizeof(int8_t));
        static_assert(1 == sizeof(uint8_t));
        static_assert(2 == sizeof(int16_t));
        static_assert(2 == sizeof(uint16_t));
        static_assert(4 == sizeof(int32_t));
        static_assert(4 == sizeof(uint32_t));
        static_assert(8 == sizeof(int64_t));
        static_assert(8 == sizeof(uint64_t));

        static_assert(4 == sizeof(float));
        static_assert(8 == sizeof(double));

        static_assert(8 == sizeof(time_t));
        static_assert(8 == sizeof(size_t));
        static_assert(8 == sizeof(ptrdiff_t));
        static_assert(8 == sizeof(intptr_t));

        static_assert(201703 == __cplusplus);
        static_assert(818    == XXX_VALUE_JUST_FOR_TEST);
    }

    fmt::print("Hello helinljn!\n");

    return 0;
}