#include "test_all.h"

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "date/date.h"
#include "spdlog/spdlog.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4018)
    #pragma warning(disable:4267)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/mysql/mysql.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

int main(void)
{
    static_assert(__cplusplus == 201703);

    fprintf(stdout, "Hello, I am test project test.\n");

    test_cpp();
    test_libevent();
    test_protocol();

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}