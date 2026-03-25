#include "doctest/doctest.h"
#include "fmt/args.h"

DOCTEST_TEST_SUITE("fmt")
{
    DOCTEST_TEST_CASE("args-test")
    {
        DOCTEST_SUBCASE("basic")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(42);
            store.push_back("abc1");
            store.push_back(1.5f);
            DOCTEST_CHECK_EQ("42 and abc1 and 1.5", fmt::vformat("{} and {} and {}", store));
        }

        DOCTEST_SUBCASE("strings_and_refs")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            char str[] = "1234567890";
            store.push_back(str);
            store.push_back(std::cref(str));
            store.push_back(fmt::string_view{str});
            str[0] = 'X';

            auto result = fmt::vformat("{} and {} and {}", store);
            DOCTEST_CHECK_EQ("1234567890 and X234567890 and X234567890", result);
        }

        DOCTEST_SUBCASE("named_int")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(fmt::arg("a1", 42));
            DOCTEST_CHECK_EQ("42", fmt::vformat("{a1}", store));
        }

        DOCTEST_SUBCASE("named_strings")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            char str[] = "1234567890";
            store.push_back(fmt::arg("a1", str));
            store.push_back(fmt::arg("a2", std::cref(str)));
            str[0] = 'X';
            DOCTEST_CHECK_EQ("1234567890 and X234567890", fmt::vformat("{a1} and {a2}", store));
        }

        DOCTEST_SUBCASE("named_arg_by_ref")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            char band[] = "Rolling Stones";
            store.push_back(fmt::arg("band", std::cref(band)));
            band[9] = 'c';  // Changing band affects the output.
            DOCTEST_CHECK_EQ(fmt::vformat("{band}", store), "Rolling Scones");
        }

        DOCTEST_SUBCASE("clear")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(42);

            auto result = fmt::vformat("{}", store);
            DOCTEST_CHECK_EQ("42", result);

            store.push_back(43);
            result = fmt::vformat("{} and {}", store);
            DOCTEST_CHECK_EQ("42 and 43", result);

            store.clear();
            store.push_back(44);
            result = fmt::vformat("{}", store);
            DOCTEST_CHECK_EQ("44", result);
        }

        DOCTEST_SUBCASE("reserve")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.reserve(2, 1);
            store.push_back(1.5f);
            store.push_back(fmt::arg("a", 42));
            auto result = fmt::vformat("{} and {a}", store);
            DOCTEST_CHECK_EQ("1.5 and 42", result);
        }

        DOCTEST_SUBCASE("move_constructor")
        {
            using store_type = fmt::dynamic_format_arg_store<fmt::format_context>;
            auto store = std::unique_ptr<store_type>(new store_type());
            store->push_back(42);
            store->push_back(std::string("foo"));
            store->push_back(fmt::arg("a1", "foo"));
            auto moved_store = std::move(*store);
            store.reset();
            DOCTEST_CHECK_EQ(fmt::vformat("{} {} {a1}", moved_store), "42 foo foo");
        }

        DOCTEST_SUBCASE("size")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            DOCTEST_CHECK_EQ(store.size(), 0);

            store.push_back(42);
            DOCTEST_CHECK_EQ(store.size(), 1);

            store.push_back("Molybdenum");
            DOCTEST_CHECK_EQ(store.size(), 2);

            store.clear();
            DOCTEST_CHECK_EQ(store.size(), 0);
        }
    }
}