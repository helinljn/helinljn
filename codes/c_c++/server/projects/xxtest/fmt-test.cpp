#include "gtest/gtest.h"

#include "fmt/args.h"

GTEST_TEST(FmtTest, Args)
{
    // basic
    {
        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(42);
        store.push_back("abc1");
        store.push_back(1.5f);

        auto result = fmt::vformat("{} and {} and {}", store);
        ASSERT_TRUE(result == "42 and abc1 and 1.5");
    }

    // strings and refs
    {
        char str[] = "1234567890";

        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(str);
        store.push_back(std::cref(str));
        store.push_back(fmt::string_view{str});

        str[0] = 'X';

        auto result = fmt::vformat("{} and {} and {}", store);
        ASSERT_TRUE(result == "1234567890 and X234567890 and X234567890");
    }

    // named int
    {
        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(fmt::arg("a1", 42));

        auto result = fmt::vformat("{a1}", store);
        ASSERT_TRUE(result == "42");
    }

    // names string
    {
        char str[] = "1234567890";

        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(fmt::arg("a1", str));
        store.push_back(fmt::arg("a2", std::cref(str)));

        str[0] = 'X';

        auto result = fmt::vformat("{a1} and {a2}", store);
        ASSERT_TRUE(result == "1234567890 and X234567890");
    }

    // named arg by ref
    {
        char band[] = "Rolling Stones";

        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(fmt::arg("band", std::cref(band)));

        band[9] = 'c';

        auto result = fmt::vformat("{band}", store);
        ASSERT_TRUE(result == "Rolling Scones");
    }

    // clear
    {
        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.push_back(42);

        auto result = fmt::vformat("{}", store);
        ASSERT_TRUE(result == "42");

        store.push_back(43);
        result = fmt::vformat("{} and {}", store);
        ASSERT_TRUE(result == "42 and 43");

        store.clear();
        store.push_back(44);
        result = fmt::vformat("{}", store);
        ASSERT_TRUE(result == "44");
    }

    // reserve
    {
        fmt::dynamic_format_arg_store<fmt::format_context> store;
        store.reserve(4, 4);
        store.push_back(1.5f);
        store.push_back(fmt::arg("a1", 42));

        auto result = fmt::vformat("{a1} and {}", store);
        ASSERT_TRUE(result == "42 and 1.5");
    }
}