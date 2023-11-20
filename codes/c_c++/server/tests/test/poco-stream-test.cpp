#include "gtest/gtest.h"
#include "util/types.h"
#include "util/base32.h"
#include "util/base64.h"
#include "util/hex_binary.h"
#include "Poco/Base32Encoder.h"
#include "Poco/Base32Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/StreamCopier.h"
#include "Poco/CountingStream.h"
#include "Poco/FileStream.h"
#include "Poco/File.h"
#include "Poco/MemoryStream.h"

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
        while (decoder.good())
        {
            decoder.read(buff, 128);
            if (decoder.gcount() > 0)
                str.append(buff, decoder.gcount());
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
        while (decoder.good())
        {
            decoder.read(buff, 128);
            if (decoder.gcount() > 0)
                str.append(buff, decoder.gcount());
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
        while (decoder.good())
        {
            decoder.read(reinterpret_cast<char*>(&buff), 2);
            if (decoder.gcount() > 0)
            {
                memcpy(reinterpret_cast<char*>(&temp) + readSize, buff, decoder.gcount());
                readSize += decoder.gcount();
            }
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
        while (decoder.good())
        {
            decoder.read(reinterpret_cast<char*>(&buff), 2);
            if (decoder.gcount() > 0)
            {
                memcpy(reinterpret_cast<char*>(&temp) + readSize, buff, decoder.gcount());
                readSize += decoder.gcount();
            }
        }

        ASSERT_TRUE(temp == data);
        ASSERT_TRUE(iss.good() && decoder.eof());
    }

    ASSERT_TRUE(common::to_hex_string(data, hexUpper, true)  && hexUpper == "11111111");
    ASSERT_TRUE(common::to_hex_string(data, hexLower, false) && hexLower == "11111111");
    ASSERT_TRUE(common::from_hex_string(hexUpper, temp) && temp == data);
    ASSERT_TRUE(common::from_hex_string(hexLower, temp) && temp == data);
}

GTEST_TEST(PocoStreamTest, StreamCopier)
{
    const std::string_view text("hello world! 1234567890");

    std::istringstream iss(text.data());
    ASSERT_TRUE(iss.good() && iss.str() == text);

    std::ostringstream oss;
    ASSERT_TRUE(oss.good() && oss.str().empty());

    // copyStream
    {
        std::streamsize sz = Poco::StreamCopier::copyStream(iss, oss);
        ASSERT_TRUE(oss.good() && oss.str() == text && static_cast<size_t>(sz) == text.size());
    }

    ASSERT_TRUE(iss.eof() && iss.str() == text);
    iss.clear();
    ASSERT_TRUE(iss.rdbuf()->pubseekpos(0) == 0 && iss.good() && iss.str() == text);

    oss.clear();
    oss.str("");
    ASSERT_TRUE(oss.good() && oss.str().empty());

    // copyStreamUnbuffered
    {
        std::streamsize sz = Poco::StreamCopier::copyStreamUnbuffered(iss, oss);
        ASSERT_TRUE(oss.good() && oss.str() == text && static_cast<size_t>(sz) == text.size());
    }
}

GTEST_TEST(PocoStreamTest, CountingStream)
{
    // input
    {
        std::istringstream        iss("foo\nbar\n");
        Poco::CountingInputStream ci(iss);

        char ch = '\0';
        while (ci.good()) ci.get(ch);

        ASSERT_TRUE(iss.eof() && iss.str() == "foo\nbar\n");
        ASSERT_TRUE(ci.chars() == 8);
        ASSERT_TRUE(ci.lines() == 2);
        ASSERT_TRUE(ci.pos() == 0);

        iss.clear();
        iss.rdbuf()->pubseekpos(0);
        ASSERT_TRUE(iss.good() && iss.str() == "foo\nbar\n");
    }

    // output
    {
        std::ostringstream         oss;
        Poco::CountingOutputStream co(oss);

        co << "foo\nbar";

        ASSERT_TRUE(oss.good() && oss.str() == "foo\nbar");
        ASSERT_TRUE(co.chars() == 7);
        ASSERT_TRUE(co.lines() == 2);
        ASSERT_TRUE(co.pos() == 3);

        oss.clear();
        oss.str("");
        ASSERT_TRUE(oss.good() && oss.str().empty());
    }
}

GTEST_TEST(PocoStreamTest, FileStream)
{
    Poco::File  test("test.txt");
    std::string data(4096, '1');

    // FileOutputStream
    {
        Poco::FileOutputStream fos(test.path());
        ASSERT_TRUE(fos.good());

        fos.write(data.data(), data.size());
        ASSERT_TRUE(fos.good());

        fos << '1';
        ASSERT_TRUE(fos.good());

        fos.close();
        ASSERT_TRUE(fos.good());
    }

    // FileInputStream
    {
        Poco::FileInputStream fis(test.path());
        ASSERT_TRUE(fis.good());

        std::string str;
        fis >> str;
        ASSERT_TRUE(str.size() - 1 == data.size() && fis.eof());

        fis.close();
        ASSERT_TRUE(fis.eof());
    }

    // FileStream
    {
        if (test.exists())
            test.remove();

        Poco::FileStream fs(test.path());
        ASSERT_TRUE(fs.good());

        std::string_view data1 = {"hello"};
        fs.write(data1.data(), data1.size());
        ASSERT_TRUE(fs.good());

        fs.seekg(0, std::ios::beg);
        ASSERT_TRUE(fs.good());

        std::string str;
        fs >> str;
        ASSERT_TRUE(str == data1 && fs.eof());
    }

    // test open mode in
    {
        if (test.exists())
            test.remove();

        Poco::FileInputStream ifs;
        ASSERT_THROW(ifs.open(test.path(), std::ios::in), Poco::FileNotFoundException);

        test.createFile();
        ifs.open(test.path(), std::ios::in);
        ASSERT_TRUE(ifs.good());
    }

    // test open mode out
    {
        if (test.exists())
            test.remove();

        Poco::FileOutputStream fos(test.path());
        ASSERT_TRUE(fos.good());

        fos.write(data.data(), data.size());
        ASSERT_TRUE(fos.good());

        fos.close();
        ASSERT_TRUE(fos.good());
        ASSERT_TRUE(test.getSize() == data.size());

        Poco::FileOutputStream fos1(test.path());
        ASSERT_TRUE(fos1.good());

        fos1.close();
        ASSERT_TRUE(fos1.good());

        ASSERT_TRUE(test.exists() && test.getSize() == 0);
    }

    if (test.exists())
        test.remove();
}

GTEST_TEST(PocoStreamTest, MemoryStream)
{
    // MemoryInputStream
    {
        std::string_view        data = "This is a test";
        Poco::MemoryInputStream mis(data.data(), data.size());
        ASSERT_TRUE(mis.good());

        ASSERT_TRUE(mis.get() == 'T' && mis.gcount() == 1 && mis.good());
        ASSERT_TRUE(mis.get() == 'h' && mis.gcount() == 1 && mis.good());

        std::string str;
        mis >> str;
        ASSERT_TRUE(str == "is" && mis.good());

        std::array<char, 32> buf = {0};
        mis.read(buf.data(), buf.size());
        ASSERT_TRUE(std::string(" is a test") == buf.data() && mis.gcount() == 10 && mis.eof());
    }

    // MemoryOutputStream
    {
        std::array<char, 32>     buf;
        Poco::MemoryOutputStream mos(buf.data(), buf.size());
        ASSERT_TRUE(mos.good());

        mos << "This is a test " << 42 << std::ends;
        ASSERT_TRUE(std::string("This is a test 42") == buf.data() && mos.good());
    }

    // tell
    {
        std::array<char, 32>     buf;
        Poco::MemoryOutputStream mos(buf.data(), buf.size());
        ASSERT_TRUE(mos.good());

        mos << "This is a test " << 42 << std::ends;
        ASSERT_TRUE(std::string("This is a test 42") == buf.data() && mos.tellp() == 18 && mos.charsWritten() == 18 && mos.good());

        Poco::MemoryInputStream mis(buf.data(), buf.size());
        ASSERT_TRUE(mis.good());

        ASSERT_TRUE(mis.get() == 'T' && mis.gcount() == 1 && mis.good());

        char ch = '\0';
        mis >> ch;
        ASSERT_TRUE(ch == 'h' && mis.gcount() == 1 && mis.tellg() == 2 && mis.good());
    }

    // input seek
    {
        std::string_view        data = "123456789";
        Poco::MemoryInputStream mis(data.data(), data.size());
        ASSERT_TRUE(mis.good());

        ASSERT_TRUE(mis.get() == '1' && mis.good() && mis.gcount() == 1 && mis.tellg() == 1);

        mis.seekg(3, std::ios::beg);
        ASSERT_TRUE(mis.get() == '4' && mis.good() && mis.gcount() == 1 && mis.tellg() == 4);
        ASSERT_TRUE(mis.get() == '5' && mis.good() && mis.gcount() == 1 && mis.tellg() == 5);

        mis.seekg(2, std::ios::cur);
        ASSERT_TRUE(mis.get() == '8' && mis.good() && mis.gcount() == 1 && mis.tellg() == 8);

        mis.seekg(-7, std::ios::end);
        ASSERT_TRUE(mis.get() == '3' && mis.good() && mis.gcount() == 1 && mis.tellg() == 3);

        mis.seekg(data.size(), std::ios::beg);
        ASSERT_TRUE(mis.good() && mis.gcount() == 1 && mis.tellg() == 9);

        mis.seekg(100, std::ios::beg);
        ASSERT_TRUE(mis.fail() && mis.gcount() == 1 && mis.tellg() == -1);

        mis.clear();
        mis.seekg(data.size(), std::ios::beg);
        ASSERT_TRUE(mis.get() == -1 && mis.eof() && mis.gcount() == 0 && mis.tellg() == -1);

        mis.clear();
        mis.seekg(0, std::ios::beg);
        ASSERT_TRUE(mis.get() == '1' && mis.good() && mis.gcount() == 1 && mis.tellg() == 1);
    }

    // output seek
    {
        std::string_view         data = {"123456789"};
        std::array<char, 16>     buf  = {0};
        Poco::MemoryOutputStream mos(buf.data(), buf.size());
        ASSERT_TRUE(mos.good());

        mos << data;
        ASSERT_TRUE(mos.good() && mos.tellp() == 9 && mos.charsWritten() == 9);

        mos.seekp(3, std::ios::beg);
        mos.put('a');
        mos.put('b');
        ASSERT_TRUE(mos.good() && mos.tellp() == 5 && mos.charsWritten() == 5 && buf[3] == 'a' && buf[4] == 'b');

        mos.seekp(2, std::ios::cur);
        mos.put('c');
        ASSERT_TRUE(mos.good() && mos.tellp() == 8 && mos.charsWritten() == 8 && buf[7] == 'c');

        mos.seekp(-7, std::ios::end);
        mos.put('d');
        ASSERT_TRUE(mos.good() && mos.tellp() == 10 && mos.charsWritten() == 10 && buf[9] == 'd');

        mos.seekp(buf.size(), std::ios::beg);
        ASSERT_TRUE(mos.good() && mos.tellp() == 16 && mos.charsWritten() == 16);

        mos.seekp(100, std::ios::beg);
        ASSERT_TRUE(mos.fail() && mos.tellp() == -1 && mos.charsWritten() == 16);

        mos.clear();
        mos.seekp(buf.size(), std::ios::beg);
        mos.put('e');
        ASSERT_TRUE(mos.fail() && mos.tellp() == -1 && mos.charsWritten() == 16);

        mos.clear();
        mos.seekp(9, std::ios::beg);
        mos.put('e');
        ASSERT_TRUE(mos.good() && mos.tellp() == 10 && mos.charsWritten() == 10 && buf[9] == 'e');
    }
}