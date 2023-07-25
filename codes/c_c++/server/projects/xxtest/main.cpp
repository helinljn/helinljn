#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include "event2/event.h"

#include "Poco/Crypto/Crypto.h"
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

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}