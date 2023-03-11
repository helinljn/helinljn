#include "server/asio/asio.h"
#include "common/version.h"
#include "spdlog/spdlog.h"
#include "event2/event.h"
#include "tinyxml2.h"
#include "json.h"
#include "zlib.h"

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#if defined(__cplusplus)
}
#endif // defined(__cplusplus)

#include "LuaBridge/LuaBridge.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4200)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif // defined(_MSC_VER)

#include "hiredis.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif // defined(_MSC_VER)

#include "test_protocol/test_protocol.h"

int main(void)
{
    static_assert(__cplusplus == 201703);

    test_protocol_all();

    fmt::print("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}