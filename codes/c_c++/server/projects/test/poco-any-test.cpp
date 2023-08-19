#include "gtest/gtest.h"
#include "Poco/Any.h"

GTEST_TEST(PocoAnyTest, AnyDefaultCtor)
{
    Poco::Any value;

    ASSERT_TRUE(value.empty());
    ASSERT_TRUE(value.type() == typeid(void));
    ASSERT_TRUE(Poco::AnyCast<int>(&value) == nullptr);

    const Poco::Any null;
    Poco::Any copied = null, assigned;
    assigned = null;

    ASSERT_TRUE(null.empty());
    ASSERT_TRUE(copied.empty());
    ASSERT_TRUE(assigned.empty());
}

GTEST_TEST(PocoAnyTest, AnyType)
{
    Poco::Any value;

    ASSERT_TRUE(value.empty());
    ASSERT_TRUE(value.type() == typeid(void));

    value = 10;
    ASSERT_TRUE(!value.empty());
    ASSERT_TRUE(value.type() == typeid(int));
    ASSERT_TRUE(Poco::AnyCast<int>(value) == 10);

    value = 3.14;
    ASSERT_TRUE(!value.empty());
    ASSERT_TRUE(value.type() == typeid(double));
    ASSERT_TRUE(Poco::AnyCast<double>(value) == 3.14);

    value = std::string("test message");
    ASSERT_TRUE(!value.empty());
    ASSERT_TRUE(value.type() == typeid(std::string));
    ASSERT_TRUE(Poco::AnyCast<const std::string&>(value) == "test message");
}

GTEST_TEST(PocoAnyTest, AnyCopyCtor)
{
    std::string text     = "test message";
    Poco::Any   original = text;
    Poco::Any   copy     = original;

    ASSERT_TRUE(!original.empty() && !copy.empty());
    ASSERT_TRUE(original.type() == copy.type());
    ASSERT_TRUE(Poco::AnyCast<std::string>(&original) && Poco::AnyCast<std::string>(&copy));
    ASSERT_TRUE(Poco::AnyCast<std::string>(&original) != Poco::AnyCast<std::string>(&copy));

    ASSERT_TRUE(Poco::AnyCast<const std::string&>(original) == text);
    ASSERT_TRUE(Poco::AnyCast<const std::string&>(copy) == text);
    ASSERT_TRUE(Poco::AnyCast<std::string&>(original) == text);
    ASSERT_TRUE(Poco::AnyCast<std::string&>(copy) == text);
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(original) == text);
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(copy) == text);
    ASSERT_TRUE(Poco::RefAnyCast<std::string>(original) == text);
    ASSERT_TRUE(Poco::RefAnyCast<std::string>(copy) == text);
}

GTEST_TEST(PocoAnyTest, AnyCopyAssign)
{
    std::string text         = "test message";
    Poco::Any   original     = text, copy;
    Poco::Any*  assignResult = &(copy = original);

    ASSERT_TRUE(!original.empty() && !copy.empty());
    ASSERT_TRUE(original.type() == copy.type());
    ASSERT_TRUE(Poco::AnyCast<std::string>(&original) && Poco::AnyCast<std::string>(&copy));
    ASSERT_TRUE(Poco::AnyCast<std::string>(&original) != Poco::AnyCast<std::string>(&copy));
    ASSERT_TRUE(assignResult == &copy);

    ASSERT_TRUE(Poco::AnyCast<const std::string&>(original) == text);
    ASSERT_TRUE(Poco::AnyCast<const std::string&>(copy) == text);
    ASSERT_TRUE(Poco::AnyCast<std::string&>(original) == text);
    ASSERT_TRUE(Poco::AnyCast<std::string&>(copy) == text);
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(original) == text);
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(copy) == text);
    ASSERT_TRUE(Poco::RefAnyCast<std::string>(original) == text);
    ASSERT_TRUE(Poco::RefAnyCast<std::string>(copy) == text);

    // test self assignment
    Poco::Any& ref = original;
    original = ref;
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(original) == Poco::RefAnyCast<std::string>(copy));
    original = original;
    ASSERT_TRUE(Poco::AnyCast<const std::string&>(original) == Poco::AnyCast<std::string&>(copy));
}

GTEST_TEST(PocoAnyTest, AnySwap)
{
    Poco::Any empty1, empty2;
    ASSERT_TRUE(empty1.empty());
    ASSERT_TRUE(empty2.empty());

    empty1.swap(empty2);
    ASSERT_TRUE(empty1.empty());
    ASSERT_TRUE(empty2.empty());

    std::string text = "test message";
    empty1 = text;
    ASSERT_TRUE(!empty1.empty());
    ASSERT_TRUE(empty2.empty());
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(empty1) == text);

    empty1.swap(empty2);
    ASSERT_TRUE(empty1.empty());
    ASSERT_TRUE(!empty2.empty());
    ASSERT_TRUE(Poco::RefAnyCast<const std::string>(empty2) == text);
}