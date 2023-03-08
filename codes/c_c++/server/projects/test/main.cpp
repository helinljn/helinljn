#include "asio.hpp"
#include "asio/ssl.hpp"

int main(void)
{
    static_assert(__cplusplus == 201703);

    fprintf(stdout, "Hello, I am test project test.\n");

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}