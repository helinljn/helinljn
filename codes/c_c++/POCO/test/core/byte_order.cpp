#include "byte_order.h"
#include "Poco/Poco.h"
#include "Poco/ByteOrder.h"

#include "fmt/format.h"

void test_flip_bytes(void)
{
    // int16_t | uint16_t
    {
        int16_t norm = static_cast<int16_t>(0xAABB);
        int16_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(static_cast<uint16_t>(flip) == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }
    {
        uint16_t norm = static_cast<uint16_t>(0xAABB);
        uint16_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(flip == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }

    // int32_t | uint32_t
    {
        int32_t norm = static_cast<int32_t>(0xAABBCCDD);
        int32_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(static_cast<uint32_t>(flip)  == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }
    {
        uint32_t norm = static_cast<uint32_t>(0xAABBCCDD);
        uint32_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(flip == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }

    // int64_t | uint64_t
    {
        int64_t norm = static_cast<int64_t>(0x8899AABBCCDDEEFF);
        int64_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(static_cast<uint64_t>(flip)  == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }
    {
        uint64_t norm = static_cast<uint64_t>(0x8899AABBCCDDEEFF);
        uint64_t flip = Poco::ByteOrder::flipBytes(norm);
        poco_assert(flip == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(flip == norm);
    }
}

void test_big_endian(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

void test_little_endian(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        poco_assert(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

void test_network_byte_order(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        poco_assert(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        poco_assert(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

void test_byte_order_all(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    fmt::print("Poco big-endian.\n");
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    fmt::print("Poco little-endian.\n");
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)

    test_flip_bytes();
    test_big_endian();
    test_little_endian();
    test_network_byte_order();
}