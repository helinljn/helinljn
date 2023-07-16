#include "gtest/gtest.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/RegularExpression.h"

TEST(StringTest, TrimLeft)
{
    // trimLeft
    {
        std::string s1 = "abc";
        ASSERT_TRUE(Poco::trimLeft(s1) == "abc");

        std::string s2 = " abc ";
        ASSERT_TRUE(Poco::trimLeft(s2) == "abc ");

        std::string s3 = "  ab c ";
        ASSERT_TRUE(Poco::trimLeft(s3) == "ab c ");

        std::string s4 = "";
        ASSERT_TRUE(Poco::trimLeft(s4) == "");
    }

    // trimLeftInPlace
    {
        std::string s1 = "abc";
        Poco::trimLeftInPlace(s1);
        ASSERT_TRUE(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimLeftInPlace(s2);
        ASSERT_TRUE(s2 == "abc ");

        std::string s3 = "  ab c ";
        Poco::trimLeftInPlace(s3);
        ASSERT_TRUE(s3 == "ab c ");

        std::string s4 = "";
        Poco::trimLeftInPlace(s4);
        ASSERT_TRUE(s4 == "");
    }
}

TEST(StringTest, TrimRight)
{
    // trimRight
    {
        std::string s1 = "abc";
        ASSERT_TRUE(Poco::trimRight(s1) == "abc");

        std::string s2 = " abc ";
        ASSERT_TRUE(Poco::trimRight(s2) == " abc");

        std::string s3 = "  ab c ";
        ASSERT_TRUE(Poco::trimRight(s3) == "  ab c");

        std::string s4 = "";
        ASSERT_TRUE(Poco::trimRight(s4) == "");
    }

    // trimRightInPlace
    {
        std::string s1 = "abc";
        Poco::trimRightInPlace(s1);
        ASSERT_TRUE(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimRightInPlace(s2);
        ASSERT_TRUE(s2 == " abc");

        std::string s3 = "  ab c ";
        Poco::trimRightInPlace(s3);
        ASSERT_TRUE(s3 == "  ab c");

        std::string s4 = "";
        Poco::trimRightInPlace(s4);
        ASSERT_TRUE(s4 == "");
    }
}

TEST(StringTest, Trim)
{
    // trim
    {
        std::string s1 = "abc";
        ASSERT_TRUE(Poco::trim(s1) == "abc");

        std::string s2 = " abc ";
        ASSERT_TRUE(Poco::trim(s2) == "abc");

        std::string s3 = "  ab c ";
        ASSERT_TRUE(Poco::trim(s3) == "ab c");

        std::string s4 = "";
        ASSERT_TRUE(Poco::trim(s4) == "");
    }

    // trimInPlace
    {
        std::string s1 = "abc";
        Poco::trimInPlace(s1);
        ASSERT_TRUE(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimInPlace(s2);
        ASSERT_TRUE(s2 == "abc");

        std::string s3 = "  ab c ";
        Poco::trimInPlace(s3);
        ASSERT_TRUE(s3 == "ab c");

        std::string s4 = "";
        Poco::trimInPlace(s4);
        ASSERT_TRUE(s4 == "");
    }
}

TEST(StringTest, ToUpper)
{
    // toUpper
    {
        std::string s1 = "abc";
        ASSERT_TRUE(Poco::toUpper(s1) == "ABC");

        std::string s2 = "Abc";
        ASSERT_TRUE(Poco::toUpper(s1) == "ABC");
    }

    // toUpperInPlace
    {
        std::string s1 = "abc";
        Poco::toUpperInPlace(s1);
        ASSERT_TRUE(s1 == "ABC");

        std::string s2 = "Abc";
        Poco::toUpperInPlace(s2);
        ASSERT_TRUE(s2 == "ABC");
    }
}

TEST(StringTest, ToLower)
{
    // toLower
    {
        std::string s1 = "ABC";
        ASSERT_TRUE(Poco::toLower(s1) == "abc");

        std::string s2 = "aBC";
        ASSERT_TRUE(Poco::toLower(s1) == "abc");
    }

    // toLowerInPlace
    {
        std::string s1 = "ABC";
        Poco::toLowerInPlace(s1);
        ASSERT_TRUE(s1 == "abc");

        std::string s2 = "aBC";
        Poco::toLowerInPlace(s2);
        ASSERT_TRUE(s2 == "abc");
    }
}

TEST(StringTest, icompare)
{
    std::string s1 = "AAA";
    std::string s2 = "aaa";
    std::string s3 = "bbb";
    std::string s4 = "cCcCc";
    std::string s5;
    ASSERT_TRUE(Poco::icompare(s1, s2) == 0);
    ASSERT_TRUE(Poco::icompare(s1, s3) < 0);
    ASSERT_TRUE(Poco::icompare(s1, s4) < 0);
    ASSERT_TRUE(Poco::icompare(s3, s1) > 0);
    ASSERT_TRUE(Poco::icompare(s4, s2) > 0);
    ASSERT_TRUE(Poco::icompare(s2, s4) < 0);
    ASSERT_TRUE(Poco::icompare(s1, s5) > 0);
    ASSERT_TRUE(Poco::icompare(s5, s4) < 0);

    std::string ss1 = "xxAAAzz";
    std::string ss2 = "YaaaX";
    std::string ss3 = "YbbbX";
    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, ss2, 1, 3) == 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, ss3, 1, 3) < 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, ss2, 1) == 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, ss3, 1) < 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 2, ss2, 1, 3) < 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 2, ss2, 1, 2) == 0);
    ASSERT_TRUE(Poco::icompare(ss3, 1, 3, ss1, 2, 3) > 0);

    ASSERT_TRUE(Poco::icompare(s1, s2.c_str()) == 0);
    ASSERT_TRUE(Poco::icompare(s1, s3.c_str()) < 0);
    ASSERT_TRUE(Poco::icompare(s1, s4.c_str()) < 0);
    ASSERT_TRUE(Poco::icompare(s3, s1.c_str()) > 0);
    ASSERT_TRUE(Poco::icompare(s4, s2.c_str()) > 0);
    ASSERT_TRUE(Poco::icompare(s2, s4.c_str()) < 0);
    ASSERT_TRUE(Poco::icompare(s1, s5.c_str()) > 0);
    ASSERT_TRUE(Poco::icompare(s5, s4.c_str()) < 0);

    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, "aaa") == 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 2, "aaa") < 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 3, "AAA") == 0);
    ASSERT_TRUE(Poco::icompare(ss1, 2, 2, "bb") < 0);

    ASSERT_TRUE(Poco::icompare(ss1, 2, "aaa") > 0);
}

TEST(StringTest, Translate)
{
    // translate
    {
        std::string s = "aabbccdd";
        ASSERT_TRUE(Poco::translate(s, "abc", "ABC") == "AABBCCdd");
        ASSERT_TRUE(Poco::translate(s, "abc", "AB") == "AABBdd");
        ASSERT_TRUE(Poco::translate(s, "abc", "") == "dd");
        ASSERT_TRUE(Poco::translate(s, "cba", "CB") == "BBCCdd");
        ASSERT_TRUE(Poco::translate(s, "", "CB") == "aabbccdd");
    }

    // translateInPlace
    {
        std::string s = "aabbccdd";
        Poco::translateInPlace(s, "abc", "ABC");
        ASSERT_TRUE(s == "AABBCCdd");
    }
}

TEST(StringTest, Replace)
{
    // replace
    {
        std::string s("aabbccdd");

        ASSERT_TRUE(Poco::replace(s, std::string("aa"), std::string("xx")) == "xxbbccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("bb"), std::string("xx")) == "aaxxccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("dd"), std::string("xx")) == "aabbccxx");
        ASSERT_TRUE(Poco::replace(s, std::string("bbcc"), std::string("xx")) == "aaxxdd");
        ASSERT_TRUE(Poco::replace(s, std::string("b"), std::string("xx")) == "aaxxxxccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("bb"), std::string("")) == "aaccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("b"), std::string("")) == "aaccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("ee"), std::string("xx")) == "aabbccdd");
        ASSERT_TRUE(Poco::replace(s, std::string("dd"), std::string("")) == "aabbcc");

        ASSERT_TRUE(Poco::replace(s, "aa", "xx") == "xxbbccdd");
        ASSERT_TRUE(Poco::replace(s, "bb", "xx") == "aaxxccdd");
        ASSERT_TRUE(Poco::replace(s, "dd", "xx") == "aabbccxx");
        ASSERT_TRUE(Poco::replace(s, "bbcc", "xx") == "aaxxdd");
        ASSERT_TRUE(Poco::replace(s, "bb", "") == "aaccdd");
        ASSERT_TRUE(Poco::replace(s, "b", "") == "aaccdd");
        ASSERT_TRUE(Poco::replace(s, "ee", "xx") == "aabbccdd");
        ASSERT_TRUE(Poco::replace(s, "dd", "") == "aabbcc");

        s = "aabbaabb";
        ASSERT_TRUE(Poco::replace(s, std::string("aa"), std::string("")) == "bbbb");
        ASSERT_TRUE(Poco::replace(s, std::string("a"), std::string("")) == "bbbb");
        ASSERT_TRUE(Poco::replace(s, std::string("a"), std::string("x")) == "xxbbxxbb");
        ASSERT_TRUE(Poco::replace(s, std::string("a"), std::string("xx")) == "xxxxbbxxxxbb");
        ASSERT_TRUE(Poco::replace(s, std::string("aa"), std::string("xxx")) == "xxxbbxxxbb");

        ASSERT_TRUE(Poco::replace(s, std::string("aa"), std::string("xx"), 2) == "aabbxxbb");

        ASSERT_TRUE(Poco::replace(s, "aa", "") == "bbbb");
        ASSERT_TRUE(Poco::replace(s, "a", "") == "bbbb");
        ASSERT_TRUE(Poco::replace(s, "a", "x") == "xxbbxxbb");
        ASSERT_TRUE(Poco::replace(s, "a", "xx") == "xxxxbbxxxxbb");
        ASSERT_TRUE(Poco::replace(s, "aa", "xxx") == "xxxbbxxxbb");

        ASSERT_TRUE(Poco::replace(s, "aa", "xx", 2) == "aabbxxbb");
        ASSERT_TRUE(Poco::replace(s, 'a', 'x', 2) == "aabbxxbb");
        ASSERT_TRUE(Poco::remove(s, 'a', 2) == "aabbbb");
        ASSERT_TRUE(Poco::remove(s, 'a') == "bbbb");
        ASSERT_TRUE(Poco::remove(s, 'b', 2) == "aaaa");
    }

    // replaceInPlace
    {
        std::string s("aabbccdd");

        Poco::replaceInPlace(s, std::string("aa"), std::string("xx"));
        ASSERT_TRUE(s == "xxbbccdd");

        s = "aabbccdd";

        Poco::replaceInPlace(s, 'a', 'x');
        ASSERT_TRUE(s == "xxbbccdd");

        Poco::replaceInPlace(s, 'x');
        ASSERT_TRUE(s == "bbccdd");

        Poco::removeInPlace(s, 'b', 1);
        ASSERT_TRUE(s == "bccdd");

        Poco::removeInPlace(s, 'd');
        ASSERT_TRUE(s == "bcc");
    }
}

TEST(StringTest, StartsWith)
{
    // startsWith
    {
        std::string s1("o");

        ASSERT_TRUE(Poco::startsWith(s1, std::string("o")));
        ASSERT_TRUE(Poco::startsWith(s1, std::string("")));

        ASSERT_TRUE(!Poco::startsWith(s1, std::string("O")));
        ASSERT_TRUE(!Poco::startsWith(s1, std::string("1")));

        std::string s2("");

        ASSERT_TRUE(Poco::startsWith(s2, std::string("")));

        ASSERT_TRUE(!Poco::startsWith(s2, std::string("o")));

        std::string s3("oO");

        ASSERT_TRUE(Poco::startsWith(s3, std::string("o")));

        ASSERT_TRUE(!Poco::startsWith(s3, std::string(" o")));
    }

    // endsWith
    {
        std::string s1("o");

        ASSERT_TRUE(Poco::endsWith(s1, std::string("o")));
        ASSERT_TRUE(Poco::endsWith(s1, std::string("")));

        ASSERT_TRUE(!Poco::endsWith(s1, std::string("O")));
        ASSERT_TRUE(!Poco::endsWith(s1, std::string("1")));


        std::string s2("");

        ASSERT_TRUE(Poco::endsWith(s2, std::string("")));

        ASSERT_TRUE(!Poco::endsWith(s2, std::string("o")));

        std::string s3("Oo");

        ASSERT_TRUE(Poco::endsWith(s3, std::string("o")));

        ASSERT_TRUE(!Poco::endsWith(s3, std::string("o ")));
    }
}

TEST(StringTest, Split)
{
    {
        Poco::StringTokenizer st("", "");
        ASSERT_TRUE(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM);
        ASSERT_TRUE(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.begin() == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc  ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "b");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(st.find("c") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "b", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(st.find("c") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "bc");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(st.find("") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(st.find("") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("a a,c c", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a a") == 0);
        ASSERT_TRUE(st.find("c c") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c c");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st("a a,c c", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a a") == 0);
        ASSERT_TRUE(st.find("c c") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c c");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st(" a a , , c c ", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find(" a a ") == 0);
        ASSERT_TRUE(st.find(" ") == 1);
        ASSERT_TRUE(st.find(" c c ") == 2);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == " a a ");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == " ");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == " c c ");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a a") == 0);
        ASSERT_TRUE(st.find("") == 1);
        ASSERT_TRUE(st.find("c c") == 2);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c c");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a a") == 0);
        ASSERT_TRUE(st.find("c c") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "c c");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi , jk,  l ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(st.find("def") == 1);
        ASSERT_TRUE(st.find("ghi") == 2);
        ASSERT_TRUE(st.find("jk") == 3);
        ASSERT_TRUE(st.find("l") == 4);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "def");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ghi");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "jk");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "l");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("abc") == 0);
        ASSERT_TRUE(st.find("def") == 1);
        ASSERT_TRUE(st.find("ghi") == 2);
        ASSERT_TRUE(st.find("jk") == 3);
        ASSERT_TRUE(st.find("l") == 4);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "abc");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "def");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ghi");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "jk");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "l");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("a/bc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("a") == 0);
        ASSERT_TRUE(st.find("bc") == 1);
        ASSERT_TRUE(st.find("def") == 2);
        ASSERT_TRUE(st.find("ghi") == 3);
        ASSERT_TRUE(st.find("jk") == 4);
        ASSERT_TRUE(st.find("l") == 5);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "a");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "bc");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "def");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ghi");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "jk");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "l");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st(",ab,cd,", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("") == 0);
        ASSERT_TRUE(st.find("ab") == 1);
        ASSERT_TRUE(st.find("cd") == 2);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ab");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "cd");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st(",ab,cd,", ",", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("ab") == 0);
        ASSERT_TRUE(st.find("cd") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ab");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "cd");
        ASSERT_TRUE(it == st.end());
    }
    {
        Poco::StringTokenizer st(" , ab , cd , ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        ASSERT_TRUE(st.find("ab") == 0);
        ASSERT_TRUE(st.find("cd") == 1);
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "ab");
        ASSERT_TRUE(it != st.end());
        ASSERT_TRUE(*it++ == "cd");
        ASSERT_TRUE(it == st.end());
    }

    {
        Poco::StringTokenizer st("1 : 2 , : 3 ", ":,", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.count() == 3);
        ASSERT_TRUE(st[0] == "1");
        ASSERT_TRUE(st[1] == "2");
        ASSERT_TRUE(st[2] == "3");
        ASSERT_TRUE(st.find("1") == 0);
        ASSERT_TRUE(st.find("2") == 1);
        ASSERT_TRUE(st.find("3") == 2);
    }

    {
        Poco::StringTokenizer st(" 2- ","-", Poco::StringTokenizer::TOK_TRIM);
        ASSERT_TRUE(st.count() == 2);
        ASSERT_TRUE(st[0] == "2");
        ASSERT_TRUE(st[1] == "");
    }

    {
        std::string           s = "#milan , a, b, c, hello# world, this, is# shmilyl#\"nice\" to # meet #you";
        Poco::StringTokenizer st(s, " ,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.count() == 13);
        ASSERT_TRUE(st[0] == "milan");
        ASSERT_TRUE(st[1] == "a");
        ASSERT_TRUE(st[2] == "b");
        ASSERT_TRUE(st[3] == "c");
        ASSERT_TRUE(st[4] == "hello");
        ASSERT_TRUE(st[5] == "world");
        ASSERT_TRUE(st[6] == "this");
        ASSERT_TRUE(st[7] == "is");
        ASSERT_TRUE(st[8] == "shmilyl");
        ASSERT_TRUE(st[9] == "\"nice\"");
        ASSERT_TRUE(st[10] == "to");
        ASSERT_TRUE(st[11] == "meet");
        ASSERT_TRUE(st[12] == "you");
    }

    {
        std::string           s = "1#2#3#4#5#6#7#8#9####   ,,# , #hello ,#,192.168.111.1";
        Poco::StringTokenizer st(s, " .,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.count() == 14);
        ASSERT_TRUE(st[0] == "1");
        ASSERT_TRUE(st[1] == "2");
        ASSERT_TRUE(st[2] == "3");
        ASSERT_TRUE(st[3] == "4");
        ASSERT_TRUE(st[4] == "5");
        ASSERT_TRUE(st[5] == "6");
        ASSERT_TRUE(st[6] == "7");
        ASSERT_TRUE(st[7] == "8");
        ASSERT_TRUE(st[8] == "9");
        ASSERT_TRUE(st[9] == "hello");
        ASSERT_TRUE(st[10] == "192");
        ASSERT_TRUE(st[11] == "168");
        ASSERT_TRUE(st[12] == "111");
        ASSERT_TRUE(st[13] == "1");
    }

    {
        std::string           s = "This is first line.\nThis is second line.\n\n\nThis is third line.\nEnd line.\n";
        Poco::StringTokenizer st(s, "\n", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        ASSERT_TRUE(st.count() == 4);
        ASSERT_TRUE(st[0] == "This is first line.");
        ASSERT_TRUE(st[1] == "This is second line.");
        ASSERT_TRUE(st[2] == "This is third line.");
        ASSERT_TRUE(st[3] == "End line.");
    }
}

TEST(StringTest, RegularExpression)
{
    // \            将下一个字符标记为一个特殊字符、或一个原义字符、或一个 后向引用、或一个八进制转义符。
    //              例如，'n' 匹配字符 "n"。'\n' 匹配一个换行符。序列 '\\' 匹配 "\" 而 "\(" 则匹配 "("。
    // ^            匹配输入字符串的开始位置。如果设置了 RegExp 对象的 Multiline 属性，^ 也匹配 '\n' 或 '\r'之后的位置。
    // $            匹配输入字符串的结束位置。如果设置了RegExp 对象的 Multiline 属性，$ 也匹配 '\n' 或 '\r'之前的位置。
    // *            匹配前面的子表达式零次或多次。例如，zo* 能匹配 "z" 以及 "zoo"。 * 等价于{0,}。
    // +            匹配前面的子表达式一次或多次。例如，'zo+' 能匹配 "zo" 以及 "zoo"，但不能匹配 "z"。+ 等价于{1,}。
    // ?            匹配前面的子表达式零次或一次。例如，"do(es)?" 可以匹配 "do" 或 "does" 中的"do" 。? 等价于{0,1}。
    // .            匹配除 "\n" 之外的任何单个字符。要匹配包括 '\n' 在内的任何字符，请使用象 '[.\n]' 的模式。
    // (pattern)    匹配pattern 并获取这一匹配。所获取的匹配可以从产生的 Matches 集合得到，在VBScript 中使用 SubMatches 集合
    //              在Visual Basic Scripting Edition 中则使用 $0…$9 属性。要匹配圆括号字符，请使用 '\(' 或 '\)'。
    // (?:pattern)  匹配 pattern 但不获取匹配结果，也就是说这是一个非获取匹配，不进行存储供以后使用。
    //              这在使用"或" 字符 (|) 来组合一个模式的各个部分 是很有用。
    //              例如， 'industr(?:y|ies) 就是一个比'industry|industries' 更简略的表达式。

    // 匹配由26个英文字母、数字或者下划线组成的字符串
    {
        Poco::RegularExpression        re("^[A-Za-z0-9_]+$");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("AaBbCc"));
        ASSERT_TRUE(re.match("AaBbCc", match) == 1);

        ASSERT_TRUE(re.match("AaBbCc123_"));
        ASSERT_TRUE(re.match("AaBbCc123_", match) == 1);

        ASSERT_TRUE(!re.match("AaBbCc123-_"));
        ASSERT_TRUE(re.match("AaBbCc123-_", match) == 0);

        ASSERT_TRUE(!re.match("a1*"));
        ASSERT_TRUE(re.match("a1*", match) == 0);
    }

    // 匹配以"fab"开头的所有字符串
    {
        Poco::RegularExpression        re("^fab.*");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("fab"));
        ASSERT_TRUE(re.match("fab", match) == 1);

        ASSERT_TRUE(re.match("fabric"));
        ASSERT_TRUE(re.match("fabric", match) == 1);

        ASSERT_TRUE(!re.match("fba"));
        ASSERT_TRUE(re.match("fba", match) == 0);

        ASSERT_TRUE(!re.match(" fab"));
        ASSERT_TRUE(re.match(" fab", match) == 0);
    }

    // 匹配包含"aaa"的所有字符串
    {
        Poco::RegularExpression        re(".*aaa.*");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("aaa"));
        ASSERT_TRUE(re.match("aaa", match) == 1);

        ASSERT_TRUE(re.match("aaaaaa"));
        ASSERT_TRUE(re.match("aaaaaa", match) == 1);

        ASSERT_TRUE(re.match("baaa"));
        ASSERT_TRUE(re.match("baaa", match) == 1);

        ASSERT_TRUE(re.match("baaac"));
        ASSERT_TRUE(re.match("baaac", match) == 1);

        ASSERT_TRUE(!re.match("aa"));
        ASSERT_TRUE(re.match("aa", match) == 0);

        ASSERT_TRUE(!re.match("bbaacc"));
        ASSERT_TRUE(re.match("bbaacc", match) == 0);
    }

    // 匹配以"ish"结尾的所有字符串
    {
        Poco::RegularExpression        re(".*ish$");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("bish"));
        ASSERT_TRUE(re.match("bish", match) == 1);

        ASSERT_TRUE(re.match("rubbish"));
        ASSERT_TRUE(re.match("rubbish", match) == 1);

        ASSERT_TRUE(!re.match("bihs"));
        ASSERT_TRUE(re.match("bihs", match) == 0);

        ASSERT_TRUE(!re.match("rubbihs"));
        ASSERT_TRUE(re.match("rubbihs", match) == 0);
    }

    // 匹配包含"bish"或者"fish"的所有字符串
    {
        Poco::RegularExpression        re(".*[bf]ish.*");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("bish"));
        ASSERT_TRUE(re.match("bish", match) == 1);

        ASSERT_TRUE(re.match("fish"));
        ASSERT_TRUE(re.match("fish", match) == 1);

        ASSERT_TRUE(re.match("abish"));
        ASSERT_TRUE(re.match("abish", match) == 1);

        ASSERT_TRUE(re.match("fisherman"));
        ASSERT_TRUE(re.match("fisherman", match) == 1);

        ASSERT_TRUE(!re.match("ish"));
        ASSERT_TRUE(re.match("ish", match) == 0);

        ASSERT_TRUE(!re.match("fis."));
        ASSERT_TRUE(re.match("fis.", match) == 0);
    }

    // 匹配以任意5个大写字母开头并以任意5个数字结尾的所有字符串
    {
        Poco::RegularExpression        re("^[A-Z]{5}.*[0-9]{5}$");
        Poco::RegularExpression::Match match;

        ASSERT_TRUE(re.match("ABXYZclf.c*()#$12345"));
        ASSERT_TRUE(re.match("ABXYZclf.c*()#$12345", match) == 1);

        ASSERT_TRUE(re.match("ABXYZ12345"));
        ASSERT_TRUE(re.match("ABXYZ12345", match) == 1);

        ASSERT_TRUE(!re.match("ABXYZclf.c*()#$123"));
        ASSERT_TRUE(re.match("ABXYZclf.c*()#$123", match) == 0);

        ASSERT_TRUE(!re.match("ABXYclf.c*()#$12345"));
        ASSERT_TRUE(re.match("ABXYclf.c*()#$12345", match) == 0);
    }

    // 匹配组数据
    {
        Poco::RegularExpression           re("([A-Z]{2}.[0-9]{2}) (?:[0-9]{2}.[0-9]{2}) ([a-z]{2}.[0-9]{2})");
        Poco::RegularExpression::MatchVec matchVec;

        std::string subject = "XY.00 00.00 xy.00";
        ASSERT_TRUE(re.match(subject));
        ASSERT_TRUE(re.match(subject, 0, matchVec) == 3);
        ASSERT_TRUE(matchVec.size() == 3);
        ASSERT_TRUE(matchVec[0].offset == 0);
        ASSERT_TRUE(matchVec[0].length == 17);
        ASSERT_TRUE(matchVec[1].offset == 0);
        ASSERT_TRUE(matchVec[1].length == 5);
        ASSERT_TRUE(matchVec[2].offset == 12);
        ASSERT_TRUE(matchVec[2].length == 5);
        ASSERT_TRUE(subject.substr(matchVec[0].offset, matchVec[0].length) == subject);
        ASSERT_TRUE(subject.substr(matchVec[1].offset, matchVec[1].length) == "XY.00");
        ASSERT_TRUE(subject.substr(matchVec[2].offset, matchVec[2].length) == "xy.00");
    }
}