#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Pair.h"

GTEST_TEST(PocoPairTest, Pair)
{
    {
        Poco::Dynamic::Pair<int> aPair;
        ASSERT_TRUE(aPair.first() == 0);
        ASSERT_TRUE(aPair.second().isEmpty());

        Poco::Dynamic::Var va(aPair);
        ASSERT_TRUE(va.convert<std::string>() == "{ \"0\": null }");
        ASSERT_TRUE(va.convert<std::string>() == aPair.toString());

        aPair = Poco::Dynamic::Pair<int>(4, "123");
        ASSERT_TRUE(aPair.first() == 4);
        ASSERT_TRUE(aPair.second() == "123");

        va = aPair;
        ASSERT_TRUE(va.convert<std::string>() == "{ \"4\": \"123\" }");
        ASSERT_TRUE(va.convert<std::string>() == aPair.toString());
    }

    {
        int         i = 1;
        std::string s = "2";

        Poco::Dynamic::Pair<int> iPair(i, s);
        ASSERT_TRUE(iPair.first() == 1);
        ASSERT_TRUE(iPair.second() == "2");

        Poco::Dynamic::Pair<std::string> sPair(s, i);
        ASSERT_TRUE(sPair.first() == "2");
        ASSERT_TRUE(sPair.second() == 1);

        Poco::Dynamic::Pair<int> pPair(std::make_pair(i, s));
        ASSERT_TRUE(pPair.first() == 1);
        ASSERT_TRUE(pPair.second() == "2");

        Poco::Dynamic::Var vp(pPair);
        ASSERT_TRUE(vp.convert<std::string>() == "{ \"1\": \"2\" }");
        ASSERT_TRUE(vp.convert<std::string>() == pPair.toString());

        Poco::Dynamic::Var vs(sPair);
        ASSERT_TRUE(vs.convert<std::string>() == "{ \"2\": 1 }");
        ASSERT_TRUE(vs.convert<std::string>() == sPair.toString());
    }
}