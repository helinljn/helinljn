#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/ByteOrder.h"

GTEST_TEST(ByteOrderTest, FlipBytes)
{
    // int16_t | uint16_t
    {
        int16_t norm = static_cast<int16_t>(0xAABB);
        int16_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(static_cast<uint16_t>(flip) == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }
    {
        uint16_t norm = static_cast<uint16_t>(0xAABB);
        uint16_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(flip == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }

    // int32_t | uint32_t
    {
        int32_t norm = static_cast<int32_t>(0xAABBCCDD);
        int32_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(static_cast<uint32_t>(flip)  == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }
    {
        uint32_t norm = static_cast<uint32_t>(0xAABBCCDD);
        uint32_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(flip == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }

    // int64_t | uint64_t
    {
        int64_t norm = static_cast<int64_t>(0x8899AABBCCDDEEFF);
        int64_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(static_cast<uint64_t>(flip)  == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }
    {
        uint64_t norm = static_cast<uint64_t>(0x8899AABBCCDDEEFF);
        uint64_t flip = Poco::ByteOrder::flipBytes(norm);
        ASSERT_TRUE(flip == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(flip == norm);
    }
}

GTEST_TEST(ByteOrderTest, BigEndian)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

GTEST_TEST(ByteOrderTest, LittleEndian)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        ASSERT_TRUE(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

GTEST_TEST(ByteOrderTest, Network)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        ASSERT_TRUE(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        ASSERT_TRUE(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}