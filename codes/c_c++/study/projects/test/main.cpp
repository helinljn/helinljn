#include "test_all.h"

#include "date/date.h"
#include "spdlog/spdlog.h"
#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/mysql/mysql.h"

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