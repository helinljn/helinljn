#include "gtest/gtest.h"
#include "util/poco.h"
#include "util/hex_binary.h"
#include "Poco/Exception.h"
#include "Poco/SharedLibrary.h"
#include "Poco/SHA1Engine.h"
#include "openssl/sha.h"
#include "mysql.h"

#include <string_view>
#include <string>
#include <array>

GTEST_TEST(PocoSharedLibraryTest, OpenSSL_MySQL)
{
    // OpenSSL
    std::array<uint8_t, Poco::SHA1Engine::DIGEST_SIZE> data = {0};
    std::string_view                                   str  = "message digest";
    SHA1(reinterpret_cast<const unsigned char*>(str.data()), str.size(), data.data());

    std::string digest;
    ASSERT_TRUE(common::to_hex_string(data.data(), data.size(), digest, false));
    ASSERT_TRUE(digest == "c12252ceda8be8994d5fa0290a47231c1d16aae3");

    // MySQL
    ASSERT_TRUE(mysql_get_client_version() == 80033);
}

GTEST_TEST(PocoSharedLibraryTest, SharedLibrary)
{
#if POCO_OS == POCO_OS_WINDOWS_NT
    std::string crypto = "libcrypto-1_1-x64";
    std::string mysql  = "libmysql";
#else
    std::string crypto = "libcrypto";
    std::string mysql  = "libmysqlclient";
#endif

    crypto.append(Poco::SharedLibrary::suffix());
    mysql.append(Poco::SharedLibrary::suffix());

    // OpenSSL
    {
        Poco::SharedLibrary sharedlib;
        ASSERT_TRUE(!sharedlib.isLoaded());

        sharedlib.load(crypto);
        ASSERT_TRUE(sharedlib.isLoaded() && sharedlib.getPath() == crypto);

        ASSERT_TRUE(sharedlib.hasSymbol("SHA1"));
        using SHA1 = unsigned char* (*)(const unsigned char*, size_t, unsigned char*);
        SHA1 sha1  = reinterpret_cast<SHA1>(sharedlib.getSymbol("SHA1"));

        std::array<uint8_t, Poco::SHA1Engine::DIGEST_SIZE> data = {0};
        std::string_view                                   str  = "message digest";
        sha1(reinterpret_cast<const unsigned char*>(str.data()), str.size(), data.data());

        std::string digest;
        ASSERT_TRUE(common::to_hex_string(data.data(), data.size(), digest, false));
        ASSERT_TRUE(digest == "c12252ceda8be8994d5fa0290a47231c1d16aae3");

        ASSERT_TRUE(!sharedlib.hasSymbol("hello_world_not_exist"));
        ASSERT_THROW(sharedlib.getSymbol("hello_world_not_exist"), Poco::NotFoundException);

        sharedlib.unload();
        ASSERT_TRUE(!sharedlib.isLoaded());
    }

    // MySQL
    {
        Poco::SharedLibrary sharedlib;
        ASSERT_TRUE(!sharedlib.isLoaded());

        sharedlib.load(mysql);
        ASSERT_TRUE(sharedlib.isLoaded() && sharedlib.getPath() == mysql);

        ASSERT_TRUE(sharedlib.hasSymbol("mysql_get_client_version"));
        using GETV = unsigned long (*)();
        GETV getv  = reinterpret_cast<GETV>(sharedlib.getSymbol("mysql_get_client_version"));

        ASSERT_TRUE(getv() == 80033);

        ASSERT_TRUE(!sharedlib.hasSymbol("hello_world_not_exist"));
        ASSERT_THROW(sharedlib.getSymbol("hello_world_not_exist"), Poco::NotFoundException);

        sharedlib.unload();
        ASSERT_TRUE(!sharedlib.isLoaded());
    }
}