#include "Poco/Foundation.h"
#include "Poco/XML/XML.h"
#include "Poco/JSON/JSON.h"
#include "Poco/Util/Util.h"
#include "Poco/Net/Net.h"
#include "Poco/Crypto/Crypto.h"
#include "Poco/Net/NetSSL.h"
#include "Poco/Zip/Zip.h"

#include "core/core.h"
#include "core/byte_order.h"
#include "core/poco_string.h"
#include "core/numeric_string.h"

int main(void)
{
    static_assert(__cplusplus == 201703);

    test_core_all();
    test_byte_order_all();
    test_poco_string_all();
    test_numeric_string_all();

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}