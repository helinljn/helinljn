#include "server/asio/asio.h"
#include "common/version.h"
#include "spdlog/spdlog.h"
#include "event2/event.h"
#include "gtest/gtest.h"
#include "tinyxml2.h"
#include "json.h"
#include "zlib.h"

#include "lua.hpp"
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

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}