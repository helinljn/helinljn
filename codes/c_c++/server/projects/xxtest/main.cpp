#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "fmt/format.h"

#include "event2/event.h"
#include "openssl/ssl.h"
#include "mysql/mysql.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "Poco/Dynamic/VarHolder.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include "Poco/Crypto/Crypto.h"
#include "Poco/Data/Data.h"
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Foundation.h"
#include "Poco/JSON/JSON.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/NetSSL.h"
#include "Poco/Redis/Redis.h"
#include "Poco/Util/Util.h"
#include "Poco/XML/XML.h"
#include "Poco/Zip/Zip.h"

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    Poco::Net::initializeNetwork();
    Poco::Net::initializeSSL();

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    const int ret = RUN_ALL_TESTS();

    Poco::Net::uninitializeSSL();
    Poco::Net::uninitializeNetwork();

    return ret;
}