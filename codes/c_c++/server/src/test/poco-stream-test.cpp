#include "gtest/gtest.h"
#include "util/poco.h"
#include "util/base32.h"
#include "util/base64.h"
#include "util/hex_binary.h"
#include "Poco/Base32Encoder.h"
#include "Poco/Base32Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"

#include <string_view>

GTEST_TEST(PocoStreamTest, Base32)
{
    const std::string_view text   = {"!@#$%^&*()_~<>"};
    const std::string_view base32 = {"EFACGJBFLYTCUKBJL57DYPQ="};

    // Encode
    {
        std::ostringstream  oss;
        Poco::Base32Encoder encoder(oss);

        encoder.write(text.data(), text.size());
        encoder.close();

        ASSERT_TRUE(oss.str() == base32);
        ASSERT_TRUE(oss.good() && encoder.good());
    }

    // Decode
    {
        std::istringstream  iss(base32.data());
        Poco::Base32Decoder decoder(iss);

        std::string str;
        char        buff[128];
        while (true)
        {
            decoder.read(buff, 128);

            if (decoder.gcount() > 0)
                str.append(buff, decoder.gcount());

            if (!decoder.good())
                break;
        }

        ASSERT_TRUE(str == text);
        ASSERT_TRUE(iss.good() && decoder.eof());
    }

    ASSERT_TRUE(common::base32_encode(text) == base32);
    ASSERT_TRUE(common::base32_decode(base32) == text);
}

GTEST_TEST(PocoStreamTest, Base64)
{
    const std::string_view text   = {"!@#$%^&*()_~<>"};
    const std::string_view base64 = {"IUAjJCVeJiooKV9+PD4="};

    // Encode
    {
        std::ostringstream  oss;
        Poco::Base64Encoder encoder(oss);

        encoder.write(text.data(), text.size());
        encoder.close();

        ASSERT_TRUE(oss.str() == base64);
        ASSERT_TRUE(oss.good() && encoder.good());
    }

    // Decode
    {
        std::istringstream  iss(base64.data());
        Poco::Base64Decoder decoder(iss);

        std::string str;
        char        buff[128];
        while (true)
        {
            decoder.read(buff, 128);

            if (decoder.gcount() > 0)
                str.append(buff, decoder.gcount());

            if (!decoder.good())
                break;
        }

        ASSERT_TRUE(str == text);
        ASSERT_TRUE(iss.good() && decoder.eof());
    }

    ASSERT_TRUE(common::base64_encode(text) == base64);
    ASSERT_TRUE(common::base64_decode(base64) == text);
}

GTEST_TEST(PocoStreamTest, HexBinary)
{
    const uint32_t         data{286331153};
    const std::string_view text{reinterpret_cast<const char*>(&data), sizeof(data)};

    uint32_t    temp{};
    std::string hexUpper;
    std::string hexLower;

    // Encode
    {
        std::ostringstream     oss;
        Poco::HexBinaryEncoder encoder(oss);

        encoder.rdbuf()->setUppercase(true);
        encoder.write(text.data(), text.size());
        encoder.close();

        hexUpper = oss.str();

        ASSERT_TRUE(hexUpper == "11111111" && hexUpper.size() == sizeof(data) * 2);
        ASSERT_TRUE(oss.good() && encoder.good());
    }

    // Encode
    {
        std::ostringstream     oss;
        Poco::HexBinaryEncoder encoder(oss);

        encoder.rdbuf()->setUppercase(false);
        encoder.write(text.data(), text.size());
        encoder.close();

        hexLower = oss.str();

        ASSERT_TRUE(hexLower == "11111111" && hexUpper.size() == sizeof(data) * 2);
        ASSERT_TRUE(oss.good() && encoder.good());
    }

    // Decode
    {
        std::istringstream     iss(hexUpper.data());
        Poco::HexBinaryDecoder decoder(iss);

        std::streamsize readSize = 0;
        char     buff[2];
        while (true)
        {
            decoder.read(reinterpret_cast<char*>(&buff), 2);

            if (decoder.gcount() > 0)
            {
                memcpy(reinterpret_cast<char*>(&temp) + readSize, buff, decoder.gcount());
                readSize += decoder.gcount();
            }

            if (!decoder.good())
                break;
        }

        ASSERT_TRUE(temp == data);
        ASSERT_TRUE(iss.good() && decoder.eof());
    }

    // Decode
    {
        std::istringstream     iss(hexLower.data());
        Poco::HexBinaryDecoder decoder(iss);

        std::streamsize readSize = 0;
        char     buff[2];
        while (true)
        {
            decoder.read(reinterpret_cast<char*>(&buff), 2);

            if (decoder.gcount() > 0)
            {
                memcpy(reinterpret_cast<char*>(&temp) + readSize, buff, decoder.gcount());
                readSize += decoder.gcount();
            }

            if (!decoder.good())
                break;
        }

        ASSERT_TRUE(temp == data);
        ASSERT_TRUE(iss.good() && decoder.eof());
    }


    ASSERT_TRUE(common::to_hex_string(data, hexUpper, true)  && hexUpper == "11111111");
    ASSERT_TRUE(common::to_hex_string(data, hexLower, false) && hexLower == "11111111");
    ASSERT_TRUE(common::from_hex_string(hexUpper, temp) && temp == data);
    ASSERT_TRUE(common::from_hex_string(hexLower, temp) && temp == data);
}