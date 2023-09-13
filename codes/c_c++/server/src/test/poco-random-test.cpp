#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/Random.h"
#include "Poco/RandomStream.h"

#include <numeric>

GTEST_TEST(PocoRandomTest, Random)
{
    {
        Poco::Random random1;
        Poco::Random random2;

        random1.seed(12345);
        random2.seed(12345);

        for (int idx = 0; idx != 1000; ++idx)
        {
            ASSERT_TRUE(random1.next() == random2.next());
        }
    }

    {
        Poco::Random random;

        random.seed();

        for (int idx = 0; idx != 1000000; ++idx)
        {
            const uint32_t n = random.next();
            ASSERT_TRUE(n <= 0x7FFFFFFF);
        }

        for (int idx = 0; idx != 1000000; ++idx)
        {
            const int32_t n = random.next();
            ASSERT_TRUE(n >= 0 && n <= 0x7FFFFFFF);
        }
    }

    {
        Poco::Random random;

        random.seed();

        const int        size = 1001;
        std::vector<int> arr(size, 0);
        for (int idx = 0; idx != 1000000; ++idx)
        {
            const int n = random.next(size);
            ASSERT_TRUE(n >= 0 && n <= 0x7FFFFFFF);

            arr[n]++;
        }

        const int sum = std::accumulate(arr.begin(), arr.end(), 0);
        const int avg = static_cast<int>(sum * 1.0 / size);
        ASSERT_TRUE(size - 50 < avg && avg < size + 50);
    }
}

GTEST_TEST(PocoRandomTest, RandomStream)
{
    {
        Poco::RandomInputStream random;

        uint32_t u32 = 0;
        for (int idx = 0; idx != 1000000; ++idx)
        {
            random >> u32;
            ASSERT_TRUE(u32 <= 0x7FFFFFFF);
        }

        int32_t i32 = 0;
        for (int idx = 0; idx != 1000000; ++idx)
        {
            random >> i32;
            ASSERT_TRUE(i32 >= 0 && i32 <= 0x7FFFFFFF);
        }
    }

    {
        Poco::RandomInputStream random;

        const int        size = 1001;
        int              n    = 0;
        std::vector<int> arr(size, 0);
        for (int idx = 0; idx != 1000000; ++idx)
        {
            random >> n;
            ASSERT_TRUE(n >= 0 && n <= 0x7FFFFFFF);

            arr[n]++;
        }

        const int sum = std::accumulate(arr.begin(), arr.end(), 0);
        const int avg = static_cast<int>(sum * 1.0 / size);
        ASSERT_TRUE(size - 50 < avg && avg < size + 50);
    }
}