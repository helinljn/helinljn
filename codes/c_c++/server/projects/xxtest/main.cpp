#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include "event2/event.h"

#include "json.h"
#include "tinyxml2.h"

#include "Poco/Crypto/Crypto.h"
#include "Poco/Foundation.h"
#include "Poco/Zip/Zip.h"
#include "Poco/zlib.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)
    #pragma warning(disable:4200)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif // defined(_MSC_VER)

#include "hiredis.h"

#include "TestMsgDefine.pb.h"
#include "TestMsgStruct.pb.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif // defined(_MSC_VER)

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}