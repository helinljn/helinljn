#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/Dynamic/Struct.h"

GTEST_TEST(PocoStructTest, Basics)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    ASSERT_TRUE(aStruct.empty() && aStruct.size() == 0);
    ASSERT_TRUE(aStruct.members().empty());

    aStruct.insert("First Name", "Little");
    ASSERT_TRUE(!aStruct.empty() && aStruct.size() == 1);
    ASSERT_TRUE(!aStruct.members().empty());
    ASSERT_TRUE(aStruct.contains("First Name"));
    ASSERT_TRUE(aStruct["First Name"] == "Little");

    aStruct.insert("Last Name", "POCO");
    ASSERT_TRUE(!aStruct.empty() && aStruct.members().size() == 2);
    ASSERT_TRUE(!aStruct.members().empty());
    ASSERT_TRUE(aStruct.contains("First Name"));
    ASSERT_TRUE(aStruct["First Name"] == "Little");
    ASSERT_TRUE(aStruct.contains("Last Name"));
    ASSERT_TRUE(aStruct["Last Name"] == "POCO");

    aStruct.erase("First Name");
    ASSERT_TRUE(!aStruct.empty() && aStruct.size() == 1);
    ASSERT_TRUE(!aStruct.members().empty());
    ASSERT_TRUE(aStruct.contains("Last Name"));
    ASSERT_TRUE(aStruct["Last Name"] == "POCO");

    aStruct.clear();
    ASSERT_TRUE(aStruct.empty() && aStruct.size() == 0);
    ASSERT_TRUE(aStruct.members().empty());
}

GTEST_TEST(PocoStructTest, EmptyString)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    aStruct["Empty"] = "";
    aStruct["Space"] = " ";
    ASSERT_TRUE(aStruct.toString() == "{ \"Empty\": \"\", \"Space\": \" \" }");
}

GTEST_TEST(PocoStructTest, NoEscapeString)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    aStruct["Birthday"] = "{ \"Day\": 12, \"Month\": \"May\", \"Year\": 2005 }";
    ASSERT_TRUE(aStruct.toString() == "{ \"Birthday\": \"{ \\\"Day\\\": 12, \\\"Month\\\": \\\"May\\\", \\\"Year\\\": 2005 }\" }");
}

GTEST_TEST(PocoStructTest, StructString)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    aStruct["First Name"] = "Junior";
    aStruct["Last Name"]  = "POCO";

    Poco::Dynamic::Var a1(aStruct);
    ASSERT_TRUE(a1["First Name"] == "Junior");
    ASSERT_TRUE(a1["Last Name"] == "POCO");

    a1["First Name"] = "Senior";
    ASSERT_TRUE(a1["First Name"] == "Senior");

    Poco::Dynamic::Struct<std::string> s1;
    s1["1"] = 1;
    s1["2"] = 2;
    s1["3"] = 3;

    Poco::Dynamic::Struct<std::string> s2(s1);
    ASSERT_TRUE(s2["1"] == 1);
    ASSERT_TRUE(s2["2"] == 2);
    ASSERT_TRUE(s2["3"] == 3);

    std::map<std::string, int> m1;
    m1["1"] = 1;
    m1["2"] = 2;
    m1["3"] = 3;

    Poco::Dynamic::Struct<std::string> m2(m1);
    ASSERT_TRUE(m2["1"] == 1);
    ASSERT_TRUE(m2["2"] == 2);
    ASSERT_TRUE(m2["3"] == 3);
}

GTEST_TEST(PocoStructTest, StructInt)
{
    Poco::Dynamic::Struct<int> aStruct;
    aStruct[0] = "Junior";
    aStruct[1] = "POCO";
    aStruct[2] = 10;

    Poco::Dynamic::Var a1(aStruct);
    ASSERT_TRUE(a1[0] == "Junior");
    ASSERT_TRUE(a1[1] == "POCO");
    ASSERT_TRUE(a1[2] == 10);

    a1[0] = "Senior";
    ASSERT_TRUE(a1[0] == "Senior");

    Poco::Dynamic::Struct<int> s1;
    s1[1] = "1";
    s1[2] = "2";
    s1[3] = "3";

    Poco::Dynamic::Struct<int> s2(s1);
    ASSERT_TRUE(s2[1] == "1");
    ASSERT_TRUE(s2[2] == "2");
    ASSERT_TRUE(s2[3] == "3");

    std::map<int, std::string> m1;
    m1[1] = "1";
    m1[2] = "2";
    m1[3] = "3";

    Poco::Dynamic::Struct<int> m2(m1);
    ASSERT_TRUE(m2[1] == "1");
    ASSERT_TRUE(m2[2] == "2");
    ASSERT_TRUE(m2[3] == "3");
}