#include "fmt/format.h"
#include "asio.hpp"
#include "asio/ssl.hpp"
#include "spdlog/spdlog.h"
#include "tinyxml2.h"
#include "json.h"
#include "server/version.h"
#include "common/version.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4200)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "hiredis.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

int main(void)
{
    static_assert(__cplusplus == 201703);

    fmt::print("Hello, I am test project test.\n");

    fmt::print("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}