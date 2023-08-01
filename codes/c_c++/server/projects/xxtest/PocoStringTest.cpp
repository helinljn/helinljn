#include "PocoStringTest.h"
#include "CppUnit/TestSuite.h"
#include "CppUnit/TestCaller.h"

#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/RegularExpression.h"

PocoStringTest::PocoStringTest(const std::string& name)
    : CppUnit::TestCase(name)
{
}

void PocoStringTest::setUp(void)
{
    // body
}

void PocoStringTest::tearDown(void)
{
    // body
}

int PocoStringTest::countTestCases(void) const
{
    return 1;
}

void PocoStringTest::testTrimLeft(void)
{
    // trimLeft
    {
        std::string s1 = "abc";
        assertTrue(Poco::trimLeft(s1) == "abc");

        std::string s2 = " abc ";
        assertTrue(Poco::trimLeft(s2) == "abc ");

        std::string s3 = "  ab c ";
        assertTrue(Poco::trimLeft(s3) == "ab c ");

        std::string s4 = "";
        assertTrue(Poco::trimLeft(s4) == "");
    }

    // trimLeftInPlace
    {
        std::string s1 = "abc";
        Poco::trimLeftInPlace(s1);
        assertTrue(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimLeftInPlace(s2);
        assertTrue(s2 == "abc ");

        std::string s3 = "  ab c ";
        Poco::trimLeftInPlace(s3);
        assertTrue(s3 == "ab c ");

        std::string s4 = "";
        Poco::trimLeftInPlace(s4);
        assertTrue(s4 == "");
    }
}

void PocoStringTest::testTrimRight(void)
{
    // trimRight
    {
        std::string s1 = "abc";
        assertTrue(Poco::trimRight(s1) == "abc");

        std::string s2 = " abc ";
        assertTrue(Poco::trimRight(s2) == " abc");

        std::string s3 = "  ab c ";
        assertTrue(Poco::trimRight(s3) == "  ab c");

        std::string s4 = "";
        assertTrue(Poco::trimRight(s4) == "");
    }

    // trimRightInPlace
    {
        std::string s1 = "abc";
        Poco::trimRightInPlace(s1);
        assertTrue(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimRightInPlace(s2);
        assertTrue(s2 == " abc");

        std::string s3 = "  ab c ";
        Poco::trimRightInPlace(s3);
        assertTrue(s3 == "  ab c");

        std::string s4 = "";
        Poco::trimRightInPlace(s4);
        assertTrue(s4 == "");
    }
}

void PocoStringTest::testTrim(void)
{
    // trim
    {
        std::string s1 = "abc";
        assertTrue(Poco::trim(s1) == "abc");

        std::string s2 = " abc ";
        assertTrue(Poco::trim(s2) == "abc");

        std::string s3 = "  ab c ";
        assertTrue(Poco::trim(s3) == "ab c");

        std::string s4 = "";
        assertTrue(Poco::trim(s4) == "");
    }

    // trimInPlace
    {
        std::string s1 = "abc";
        Poco::trimInPlace(s1);
        assertTrue(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimInPlace(s2);
        assertTrue(s2 == "abc");

        std::string s3 = "  ab c ";
        Poco::trimInPlace(s3);
        assertTrue(s3 == "ab c");

        std::string s4 = "";
        Poco::trimInPlace(s4);
        assertTrue(s4 == "");
    }
}

void PocoStringTest::testToUpper(void)
{
    // toUpper
    {
        std::string s1 = "abc";
        assertTrue(Poco::toUpper(s1) == "ABC");

        std::string s2 = "Abc";
        assertTrue(Poco::toUpper(s1) == "ABC");
    }

    // toUpperInPlace
    {
        std::string s1 = "abc";
        Poco::toUpperInPlace(s1);
        assertTrue(s1 == "ABC");

        std::string s2 = "Abc";
        Poco::toUpperInPlace(s2);
        assertTrue(s2 == "ABC");
    }
}

void PocoStringTest::testToLower(void)
{
    // toLower
    {
        std::string s1 = "ABC";
        assertTrue(Poco::toLower(s1) == "abc");

        std::string s2 = "aBC";
        assertTrue(Poco::toLower(s1) == "abc");
    }

    // toLowerInPlace
    {
        std::string s1 = "ABC";
        Poco::toLowerInPlace(s1);
        assertTrue(s1 == "abc");

        std::string s2 = "aBC";
        Poco::toLowerInPlace(s2);
        assertTrue(s2 == "abc");
    }
}

void PocoStringTest::testICompare(void)
{
    std::string s1 = "AAA";
    std::string s2 = "aaa";
    std::string s3 = "bbb";
    std::string s4 = "cCcCc";
    std::string s5;
    assertTrue(Poco::icompare(s1, s2) == 0);
    assertTrue(Poco::icompare(s1, s3) < 0);
    assertTrue(Poco::icompare(s1, s4) < 0);
    assertTrue(Poco::icompare(s3, s1) > 0);
    assertTrue(Poco::icompare(s4, s2) > 0);
    assertTrue(Poco::icompare(s2, s4) < 0);
    assertTrue(Poco::icompare(s1, s5) > 0);
    assertTrue(Poco::icompare(s5, s4) < 0);

    std::string ss1 = "xxAAAzz";
    std::string ss2 = "YaaaX";
    std::string ss3 = "YbbbX";
    assertTrue(Poco::icompare(ss1, 2, 3, ss2, 1, 3) == 0);
    assertTrue(Poco::icompare(ss1, 2, 3, ss3, 1, 3) < 0);
    assertTrue(Poco::icompare(ss1, 2, 3, ss2, 1) == 0);
    assertTrue(Poco::icompare(ss1, 2, 3, ss3, 1) < 0);
    assertTrue(Poco::icompare(ss1, 2, 2, ss2, 1, 3) < 0);
    assertTrue(Poco::icompare(ss1, 2, 2, ss2, 1, 2) == 0);
    assertTrue(Poco::icompare(ss3, 1, 3, ss1, 2, 3) > 0);

    assertTrue(Poco::icompare(s1, s2.c_str()) == 0);
    assertTrue(Poco::icompare(s1, s3.c_str()) < 0);
    assertTrue(Poco::icompare(s1, s4.c_str()) < 0);
    assertTrue(Poco::icompare(s3, s1.c_str()) > 0);
    assertTrue(Poco::icompare(s4, s2.c_str()) > 0);
    assertTrue(Poco::icompare(s2, s4.c_str()) < 0);
    assertTrue(Poco::icompare(s1, s5.c_str()) > 0);
    assertTrue(Poco::icompare(s5, s4.c_str()) < 0);

    assertTrue(Poco::icompare(ss1, 2, 3, "aaa") == 0);
    assertTrue(Poco::icompare(ss1, 2, 2, "aaa") < 0);
    assertTrue(Poco::icompare(ss1, 2, 3, "AAA") == 0);
    assertTrue(Poco::icompare(ss1, 2, 2, "bb") < 0);

    assertTrue(Poco::icompare(ss1, 2, "aaa") > 0);
}

void PocoStringTest::testTranslate(void)
{
    // translate
    {
        std::string s = "aabbccdd";
        assertTrue(Poco::translate(s, "abc", "ABC") == "AABBCCdd");
        assertTrue(Poco::translate(s, "abc", "AB") == "AABBdd");
        assertTrue(Poco::translate(s, "abc", "") == "dd");
        assertTrue(Poco::translate(s, "cba", "CB") == "BBCCdd");
        assertTrue(Poco::translate(s, "", "CB") == "aabbccdd");
    }

    // translateInPlace
    {
        std::string s = "aabbccdd";
        Poco::translateInPlace(s, "abc", "ABC");
        assertTrue(s == "AABBCCdd");
    }
}

void PocoStringTest::testReplace(void)
{
    // replace
    {
        std::string s("aabbccdd");

        assertTrue(Poco::replace(s, std::string("aa"), std::string("xx")) == "xxbbccdd");
        assertTrue(Poco::replace(s, std::string("bb"), std::string("xx")) == "aaxxccdd");
        assertTrue(Poco::replace(s, std::string("dd"), std::string("xx")) == "aabbccxx");
        assertTrue(Poco::replace(s, std::string("bbcc"), std::string("xx")) == "aaxxdd");
        assertTrue(Poco::replace(s, std::string("b"), std::string("xx")) == "aaxxxxccdd");
        assertTrue(Poco::replace(s, std::string("bb"), std::string("")) == "aaccdd");
        assertTrue(Poco::replace(s, std::string("b"), std::string("")) == "aaccdd");
        assertTrue(Poco::replace(s, std::string("ee"), std::string("xx")) == "aabbccdd");
        assertTrue(Poco::replace(s, std::string("dd"), std::string("")) == "aabbcc");

        assertTrue(Poco::replace(s, "aa", "xx") == "xxbbccdd");
        assertTrue(Poco::replace(s, "bb", "xx") == "aaxxccdd");
        assertTrue(Poco::replace(s, "dd", "xx") == "aabbccxx");
        assertTrue(Poco::replace(s, "bbcc", "xx") == "aaxxdd");
        assertTrue(Poco::replace(s, "bb", "") == "aaccdd");
        assertTrue(Poco::replace(s, "b", "") == "aaccdd");
        assertTrue(Poco::replace(s, "ee", "xx") == "aabbccdd");
        assertTrue(Poco::replace(s, "dd", "") == "aabbcc");

        s = "aabbaabb";
        assertTrue(Poco::replace(s, std::string("aa"), std::string("")) == "bbbb");
        assertTrue(Poco::replace(s, std::string("a"), std::string("")) == "bbbb");
        assertTrue(Poco::replace(s, std::string("a"), std::string("x")) == "xxbbxxbb");
        assertTrue(Poco::replace(s, std::string("a"), std::string("xx")) == "xxxxbbxxxxbb");
        assertTrue(Poco::replace(s, std::string("aa"), std::string("xxx")) == "xxxbbxxxbb");

        assertTrue(Poco::replace(s, std::string("aa"), std::string("xx"), 2) == "aabbxxbb");

        assertTrue(Poco::replace(s, "aa", "") == "bbbb");
        assertTrue(Poco::replace(s, "a", "") == "bbbb");
        assertTrue(Poco::replace(s, "a", "x") == "xxbbxxbb");
        assertTrue(Poco::replace(s, "a", "xx") == "xxxxbbxxxxbb");
        assertTrue(Poco::replace(s, "aa", "xxx") == "xxxbbxxxbb");

        assertTrue(Poco::replace(s, "aa", "xx", 2) == "aabbxxbb");
        assertTrue(Poco::replace(s, 'a', 'x', 2) == "aabbxxbb");
        assertTrue(Poco::remove(s, 'a', 2) == "aabbbb");
        assertTrue(Poco::remove(s, 'a') == "bbbb");
        assertTrue(Poco::remove(s, 'b', 2) == "aaaa");
    }

    // replaceInPlace
    {
        std::string s("aabbccdd");

        Poco::replaceInPlace(s, std::string("aa"), std::string("xx"));
        assertTrue(s == "xxbbccdd");

        s = "aabbccdd";

        Poco::replaceInPlace(s, 'a', 'x');
        assertTrue(s == "xxbbccdd");

        Poco::replaceInPlace(s, 'x');
        assertTrue(s == "bbccdd");

        Poco::removeInPlace(s, 'b', 1);
        assertTrue(s == "bccdd");

        Poco::removeInPlace(s, 'd');
        assertTrue(s == "bcc");
    }
}

void PocoStringTest::testStartsWith(void)
{
    // startsWith
    {
        std::string s1("o");

        assertTrue(Poco::startsWith(s1, std::string("o")));
        assertTrue(Poco::startsWith(s1, std::string("")));

        assertTrue(!Poco::startsWith(s1, std::string("O")));
        assertTrue(!Poco::startsWith(s1, std::string("1")));

        std::string s2("");

        assertTrue(Poco::startsWith(s2, std::string("")));

        assertTrue(!Poco::startsWith(s2, std::string("o")));

        std::string s3("oO");

        assertTrue(Poco::startsWith(s3, std::string("o")));

        assertTrue(!Poco::startsWith(s3, std::string(" o")));
    }

    // endsWith
    {
        std::string s1("o");

        assertTrue(Poco::endsWith(s1, std::string("o")));
        assertTrue(Poco::endsWith(s1, std::string("")));

        assertTrue(!Poco::endsWith(s1, std::string("O")));
        assertTrue(!Poco::endsWith(s1, std::string("1")));


        std::string s2("");

        assertTrue(Poco::endsWith(s2, std::string("")));

        assertTrue(!Poco::endsWith(s2, std::string("o")));

        std::string s3("Oo");

        assertTrue(Poco::endsWith(s3, std::string("o")));

        assertTrue(!Poco::endsWith(s3, std::string("o ")));
    }
}

void PocoStringTest::testSplit(void)
{
    {
        Poco::StringTokenizer st("", "");
        assertTrue(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM);
        assertTrue(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.begin() == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc  ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "b");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(st.find("c") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "b", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(st.find("c") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "bc");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(st.find("") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(st.find("") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("a a,c c", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a a") == 0);
        assertTrue(st.find("c c") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c c");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st("a a,c c", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a a") == 0);
        assertTrue(st.find("c c") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c c");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st(" a a , , c c ", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find(" a a ") == 0);
        assertTrue(st.find(" ") == 1);
        assertTrue(st.find(" c c ") == 2);
        assertTrue(it != st.end());
        assertTrue(*it++ == " a a ");
        assertTrue(it != st.end());
        assertTrue(*it++ == " ");
        assertTrue(it != st.end());
        assertTrue(*it++ == " c c ");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a a") == 0);
        assertTrue(st.find("") == 1);
        assertTrue(st.find("c c") == 2);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c c");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a a") == 0);
        assertTrue(st.find("c c") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "c c");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi , jk,  l ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(st.find("def") == 1);
        assertTrue(st.find("ghi") == 2);
        assertTrue(st.find("jk") == 3);
        assertTrue(st.find("l") == 4);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it != st.end());
        assertTrue(*it++ == "def");
        assertTrue(it != st.end());
        assertTrue(*it++ == "ghi");
        assertTrue(it != st.end());
        assertTrue(*it++ == "jk");
        assertTrue(it != st.end());
        assertTrue(*it++ == "l");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("abc") == 0);
        assertTrue(st.find("def") == 1);
        assertTrue(st.find("ghi") == 2);
        assertTrue(st.find("jk") == 3);
        assertTrue(st.find("l") == 4);
        assertTrue(it != st.end());
        assertTrue(*it++ == "abc");
        assertTrue(it != st.end());
        assertTrue(*it++ == "def");
        assertTrue(it != st.end());
        assertTrue(*it++ == "ghi");
        assertTrue(it != st.end());
        assertTrue(*it++ == "jk");
        assertTrue(it != st.end());
        assertTrue(*it++ == "l");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("a/bc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("a") == 0);
        assertTrue(st.find("bc") == 1);
        assertTrue(st.find("def") == 2);
        assertTrue(st.find("ghi") == 3);
        assertTrue(st.find("jk") == 4);
        assertTrue(st.find("l") == 5);
        assertTrue(it != st.end());
        assertTrue(*it++ == "a");
        assertTrue(it != st.end());
        assertTrue(*it++ == "bc");
        assertTrue(it != st.end());
        assertTrue(*it++ == "def");
        assertTrue(it != st.end());
        assertTrue(*it++ == "ghi");
        assertTrue(it != st.end());
        assertTrue(*it++ == "jk");
        assertTrue(it != st.end());
        assertTrue(*it++ == "l");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st(",ab,cd,", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("") == 0);
        assertTrue(st.find("ab") == 1);
        assertTrue(st.find("cd") == 2);
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it != st.end());
        assertTrue(*it++ == "ab");
        assertTrue(it != st.end());
        assertTrue(*it++ == "cd");
        assertTrue(it != st.end());
        assertTrue(*it++ == "");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st(",ab,cd,", ",", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("ab") == 0);
        assertTrue(st.find("cd") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "ab");
        assertTrue(it != st.end());
        assertTrue(*it++ == "cd");
        assertTrue(it == st.end());
    }
    {
        Poco::StringTokenizer st(" , ab , cd , ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        assertTrue(st.find("ab") == 0);
        assertTrue(st.find("cd") == 1);
        assertTrue(it != st.end());
        assertTrue(*it++ == "ab");
        assertTrue(it != st.end());
        assertTrue(*it++ == "cd");
        assertTrue(it == st.end());
    }

    {
        Poco::StringTokenizer st("1 : 2 , : 3 ", ":,", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.count() == 3);
        assertTrue(st[0] == "1");
        assertTrue(st[1] == "2");
        assertTrue(st[2] == "3");
        assertTrue(st.find("1") == 0);
        assertTrue(st.find("2") == 1);
        assertTrue(st.find("3") == 2);
    }

    {
        Poco::StringTokenizer st(" 2- ","-", Poco::StringTokenizer::TOK_TRIM);
        assertTrue(st.count() == 2);
        assertTrue(st[0] == "2");
        assertTrue(st[1] == "");
    }

    {
        std::string           s = "#milan , a, b, c, hello# world, this, is# shmilyl#\"nice\" to # meet #you";
        Poco::StringTokenizer st(s, " ,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.count() == 13);
        assertTrue(st[0] == "milan");
        assertTrue(st[1] == "a");
        assertTrue(st[2] == "b");
        assertTrue(st[3] == "c");
        assertTrue(st[4] == "hello");
        assertTrue(st[5] == "world");
        assertTrue(st[6] == "this");
        assertTrue(st[7] == "is");
        assertTrue(st[8] == "shmilyl");
        assertTrue(st[9] == "\"nice\"");
        assertTrue(st[10] == "to");
        assertTrue(st[11] == "meet");
        assertTrue(st[12] == "you");
    }

    {
        std::string           s = "1#2#3#4#5#6#7#8#9####   ,,# , #hello ,#,192.168.111.1";
        Poco::StringTokenizer st(s, " .,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.count() == 14);
        assertTrue(st[0] == "1");
        assertTrue(st[1] == "2");
        assertTrue(st[2] == "3");
        assertTrue(st[3] == "4");
        assertTrue(st[4] == "5");
        assertTrue(st[5] == "6");
        assertTrue(st[6] == "7");
        assertTrue(st[7] == "8");
        assertTrue(st[8] == "9");
        assertTrue(st[9] == "hello");
        assertTrue(st[10] == "192");
        assertTrue(st[11] == "168");
        assertTrue(st[12] == "111");
        assertTrue(st[13] == "1");
    }

    {
        std::string           s = "This is first line.\nThis is second line.\n\n\nThis is third line.\nEnd line.\n";
        Poco::StringTokenizer st(s, "\n", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        assertTrue(st.count() == 4);
        assertTrue(st[0] == "This is first line.");
        assertTrue(st[1] == "This is second line.");
        assertTrue(st[2] == "This is third line.");
        assertTrue(st[3] == "End line.");
    }
}

void PocoStringTest::testRegularExpression(void)
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

        assertTrue(re.match("AaBbCc"));
        assertTrue(re.match("AaBbCc", match) == 1);

        assertTrue(re.match("AaBbCc123_"));
        assertTrue(re.match("AaBbCc123_", match) == 1);

        assertTrue(!re.match("AaBbCc123-_"));
        assertTrue(re.match("AaBbCc123-_", match) == 0);

        assertTrue(!re.match("a1*"));
        assertTrue(re.match("a1*", match) == 0);
    }

    // 匹配以"fab"开头的所有字符串
    {
        Poco::RegularExpression        re("^fab.*");
        Poco::RegularExpression::Match match;

        assertTrue(re.match("fab"));
        assertTrue(re.match("fab", match) == 1);

        assertTrue(re.match("fabric"));
        assertTrue(re.match("fabric", match) == 1);

        assertTrue(!re.match("fba"));
        assertTrue(re.match("fba", match) == 0);

        assertTrue(!re.match(" fab"));
        assertTrue(re.match(" fab", match) == 0);
    }

    // 匹配包含"aaa"的所有字符串
    {
        Poco::RegularExpression        re(".*aaa.*");
        Poco::RegularExpression::Match match;

        assertTrue(re.match("aaa"));
        assertTrue(re.match("aaa", match) == 1);

        assertTrue(re.match("aaaaaa"));
        assertTrue(re.match("aaaaaa", match) == 1);

        assertTrue(re.match("baaa"));
        assertTrue(re.match("baaa", match) == 1);

        assertTrue(re.match("baaac"));
        assertTrue(re.match("baaac", match) == 1);

        assertTrue(!re.match("aa"));
        assertTrue(re.match("aa", match) == 0);

        assertTrue(!re.match("bbaacc"));
        assertTrue(re.match("bbaacc", match) == 0);
    }

    // 匹配以"ish"结尾的所有字符串
    {
        Poco::RegularExpression        re(".*ish$");
        Poco::RegularExpression::Match match;

        assertTrue(re.match("bish"));
        assertTrue(re.match("bish", match) == 1);

        assertTrue(re.match("rubbish"));
        assertTrue(re.match("rubbish", match) == 1);

        assertTrue(!re.match("bihs"));
        assertTrue(re.match("bihs", match) == 0);

        assertTrue(!re.match("rubbihs"));
        assertTrue(re.match("rubbihs", match) == 0);
    }

    // 匹配包含"bish"或者"fish"的所有字符串
    {
        Poco::RegularExpression        re(".*[bf]ish.*");
        Poco::RegularExpression::Match match;

        assertTrue(re.match("bish"));
        assertTrue(re.match("bish", match) == 1);

        assertTrue(re.match("fish"));
        assertTrue(re.match("fish", match) == 1);

        assertTrue(re.match("abish"));
        assertTrue(re.match("abish", match) == 1);

        assertTrue(re.match("fisherman"));
        assertTrue(re.match("fisherman", match) == 1);

        assertTrue(!re.match("ish"));
        assertTrue(re.match("ish", match) == 0);

        assertTrue(!re.match("fis."));
        assertTrue(re.match("fis.", match) == 0);
    }

    // 匹配以任意5个大写字母开头并以任意5个数字结尾的所有字符串
    {
        Poco::RegularExpression        re("^[A-Z]{5}.*[0-9]{5}$");
        Poco::RegularExpression::Match match;

        assertTrue(re.match("ABXYZclf.c*()#$12345"));
        assertTrue(re.match("ABXYZclf.c*()#$12345", match) == 1);

        assertTrue(re.match("ABXYZ12345"));
        assertTrue(re.match("ABXYZ12345", match) == 1);

        assertTrue(!re.match("ABXYZclf.c*()#$123"));
        assertTrue(re.match("ABXYZclf.c*()#$123", match) == 0);

        assertTrue(!re.match("ABXYclf.c*()#$12345"));
        assertTrue(re.match("ABXYclf.c*()#$12345", match) == 0);
    }

    // 匹配组数据
    {
        Poco::RegularExpression           re("([A-Z]{2}.[0-9]{2}) (?:[0-9]{2}.[0-9]{2}) ([a-z]{2}.[0-9]{2})");
        Poco::RegularExpression::MatchVec matchVec;

        std::string subject = "XY.00 00.00 xy.00";
        assertTrue(re.match(subject));
        assertTrue(re.match(subject, 0, matchVec) == 3);
        assertTrue(matchVec.size() == 3);
        assertTrue(matchVec[0].offset == 0);
        assertTrue(matchVec[0].length == 17);
        assertTrue(matchVec[1].offset == 0);
        assertTrue(matchVec[1].length == 5);
        assertTrue(matchVec[2].offset == 12);
        assertTrue(matchVec[2].length == 5);
        assertTrue(subject.substr(matchVec[0].offset, matchVec[0].length) == subject);
        assertTrue(subject.substr(matchVec[1].offset, matchVec[1].length) == "XY.00");
        assertTrue(subject.substr(matchVec[2].offset, matchVec[2].length) == "xy.00");
    }
}

CppUnit::Test* PocoStringTest::suite(void)
{
    CppUnit::TestSuite* testSuite = new CppUnit::TestSuite("PocoStringTest");

    CppUnit_addTest(testSuite, PocoStringTest, testTrimLeft);
    CppUnit_addTest(testSuite, PocoStringTest, testTrimRight);
    CppUnit_addTest(testSuite, PocoStringTest, testTrim);
    CppUnit_addTest(testSuite, PocoStringTest, testToUpper);
    CppUnit_addTest(testSuite, PocoStringTest, testToLower);
    CppUnit_addTest(testSuite, PocoStringTest, testICompare);
    CppUnit_addTest(testSuite, PocoStringTest, testTranslate);
    CppUnit_addTest(testSuite, PocoStringTest, testReplace);
    CppUnit_addTest(testSuite, PocoStringTest, testStartsWith);
    CppUnit_addTest(testSuite, PocoStringTest, testSplit);
    CppUnit_addTest(testSuite, PocoStringTest, testRegularExpression);

    return testSuite;
}