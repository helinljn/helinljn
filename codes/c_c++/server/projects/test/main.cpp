#include "fmt/format.h"
#include "asio.hpp"
#include "asio/ssl.hpp"
#include "spdlog/spdlog.h"
#include "tinyxml2.h"
#include "json.h"
#include "server/version.h"
#include "common/version.h"

int main(void)
{
    static_assert(__cplusplus == 201703);

    fmt::print("Hello, I am test project test.\n");

    fmt::print("\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}