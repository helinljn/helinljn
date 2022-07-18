#include "spdlog/fmt/fmt.h"
#include "test/test_all.h"

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
    fmt::print("Hello helinljn!\n");

    test_all();

    return 0;
}