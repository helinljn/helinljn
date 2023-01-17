#include "CppUnit/CppUnit.h"
#include "Poco/Foundation.h"
#include "Poco/XML/XML.h"
#include "Poco/JSON/JSON.h"
#include "Poco/Util/Util.h"
#include "Poco/Net/Net.h"
#include "Poco/Crypto/Crypto.h"
#include "Poco/Net/NetSSL.h"
#include "Poco/Data/Data.h"
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Zip/Zip.h"
#include "Poco/JWT/JWT.h"
#include "Poco/Redis/Redis.h"

int main(void)
{
    static_assert(__cplusplus == 201703);

    fprintf(stdout, "\nPress \'Enter\' to exit...\n");
    while (std::getchar() != '\n');

    return 0;
}