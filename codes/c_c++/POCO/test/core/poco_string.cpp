#include "poco_string.h"
#include "Poco/Poco.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/RegularExpression.h"

#include "spdlog/fmt/fmt.h"

void test_trim_left(void)
{
    // trimLeft
    {
        std::string s1 = "abc";
        poco_assert(Poco::trimLeft(s1) == "abc");

        std::string s2 = " abc ";
        poco_assert(Poco::trimLeft(s2) == "abc ");

        std::string s3 = "  ab c ";
        poco_assert(Poco::trimLeft(s3) == "ab c ");

        std::string s4 = "";
        poco_assert(Poco::trimLeft(s4) == "");
    }

    // trimLeftInPlace
    {
        std::string s1 = "abc";
        Poco::trimLeftInPlace(s1);
        poco_assert(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimLeftInPlace(s2);
        poco_assert(s2 == "abc ");

        std::string s3 = "  ab c ";
        Poco::trimLeftInPlace(s3);
        poco_assert(s3 == "ab c ");

        std::string s4 = "";
        Poco::trimLeftInPlace(s4);
        poco_assert(s4 == "");
    }
}

void test_trim_right(void)
{
    // trimRight
    {
        std::string s1 = "abc";
        poco_assert(Poco::trimRight(s1) == "abc");

        std::string s2 = " abc ";
        poco_assert(Poco::trimRight(s2) == " abc");

        std::string s3 = "  ab c ";
        poco_assert(Poco::trimRight(s3) == "  ab c");

        std::string s4 = "";
        poco_assert(Poco::trimRight(s4) == "");
    }

    // trimRightInPlace
    {
        std::string s1 = "abc";
        Poco::trimRightInPlace(s1);
        poco_assert(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimRightInPlace(s2);
        poco_assert(s2 == " abc");

        std::string s3 = "  ab c ";
        Poco::trimRightInPlace(s3);
        poco_assert(s3 == "  ab c");

        std::string s4 = "";
        Poco::trimRightInPlace(s4);
        poco_assert(s4 == "");
    }
}

void test_trim(void)
{
    // trim
    {
        std::string s1 = "abc";
        poco_assert(Poco::trim(s1) == "abc");

        std::string s2 = " abc ";
        poco_assert(Poco::trim(s2) == "abc");

        std::string s3 = "  ab c ";
        poco_assert(Poco::trim(s3) == "ab c");

        std::string s4 = "";
        poco_assert(Poco::trim(s4) == "");
    }

    // trimInPlace
    {
        std::string s1 = "abc";
        Poco::trimInPlace(s1);
        poco_assert(s1 == "abc");

        std::string s2 = " abc ";
        Poco::trimInPlace(s2);
        poco_assert(s2 == "abc");

        std::string s3 = "  ab c ";
        Poco::trimInPlace(s3);
        poco_assert(s3 == "ab c");

        std::string s4 = "";
        Poco::trimInPlace(s4);
        poco_assert(s4 == "");
    }
}

void test_to_upper(void)
{
    // toUpper
    {
        std::string s1 = "abc";
        poco_assert(Poco::toUpper(s1) == "ABC");

        std::string s2 = "Abc";
        poco_assert(Poco::toUpper(s1) == "ABC");
    }

    // toUpperInPlace
    {
        std::string s1 = "abc";
        Poco::toUpperInPlace(s1);
        poco_assert(s1 == "ABC");

        std::string s2 = "Abc";
        Poco::toUpperInPlace(s2);
        poco_assert(s2 == "ABC");
    }
}

void test_to_lower(void)
{
    // toLower
    {
        std::string s1 = "ABC";
        poco_assert(Poco::toLower(s1) == "abc");

        std::string s2 = "aBC";
        poco_assert(Poco::toLower(s1) == "abc");
    }

    // toLowerInPlace
    {
        std::string s1 = "ABC";
        Poco::toLowerInPlace(s1);
        poco_assert(s1 == "abc");

        std::string s2 = "aBC";
        Poco::toLowerInPlace(s2);
        poco_assert(s2 == "abc");
    }
}

void test_icompare(void)
{
    std::string s1 = "AAA";
    std::string s2 = "aaa";
    std::string s3 = "bbb";
    std::string s4 = "cCcCc";
    std::string s5;
    poco_assert(Poco::icompare(s1, s2) == 0);
    poco_assert(Poco::icompare(s1, s3) < 0);
    poco_assert(Poco::icompare(s1, s4) < 0);
    poco_assert(Poco::icompare(s3, s1) > 0);
    poco_assert(Poco::icompare(s4, s2) > 0);
    poco_assert(Poco::icompare(s2, s4) < 0);
    poco_assert(Poco::icompare(s1, s5) > 0);
    poco_assert(Poco::icompare(s5, s4) < 0);

    std::string ss1 = "xxAAAzz";
    std::string ss2 = "YaaaX";
    std::string ss3 = "YbbbX";
    poco_assert(Poco::icompare(ss1, 2, 3, ss2, 1, 3) == 0);
    poco_assert(Poco::icompare(ss1, 2, 3, ss3, 1, 3) < 0);
    poco_assert(Poco::icompare(ss1, 2, 3, ss2, 1) == 0);
    poco_assert(Poco::icompare(ss1, 2, 3, ss3, 1) < 0);
    poco_assert(Poco::icompare(ss1, 2, 2, ss2, 1, 3) < 0);
    poco_assert(Poco::icompare(ss1, 2, 2, ss2, 1, 2) == 0);
    poco_assert(Poco::icompare(ss3, 1, 3, ss1, 2, 3) > 0);

    poco_assert(Poco::icompare(s1, s2.c_str()) == 0);
    poco_assert(Poco::icompare(s1, s3.c_str()) < 0);
    poco_assert(Poco::icompare(s1, s4.c_str()) < 0);
    poco_assert(Poco::icompare(s3, s1.c_str()) > 0);
    poco_assert(Poco::icompare(s4, s2.c_str()) > 0);
    poco_assert(Poco::icompare(s2, s4.c_str()) < 0);
    poco_assert(Poco::icompare(s1, s5.c_str()) > 0);
    poco_assert(Poco::icompare(s5, s4.c_str()) < 0);

    poco_assert(Poco::icompare(ss1, 2, 3, "aaa") == 0);
    poco_assert(Poco::icompare(ss1, 2, 2, "aaa") < 0);
    poco_assert(Poco::icompare(ss1, 2, 3, "AAA") == 0);
    poco_assert(Poco::icompare(ss1, 2, 2, "bb") < 0);

    poco_assert(Poco::icompare(ss1, 2, "aaa") > 0);
}

void test_translate(void)
{
    // translate
    {
        std::string s = "aabbccdd";
        poco_assert(Poco::translate(s, "abc", "ABC") == "AABBCCdd");
        poco_assert(Poco::translate(s, "abc", "AB") == "AABBdd");
        poco_assert(Poco::translate(s, "abc", "") == "dd");
        poco_assert(Poco::translate(s, "cba", "CB") == "BBCCdd");
        poco_assert(Poco::translate(s, "", "CB") == "aabbccdd");
    }

    // translateInPlace
    {
        std::string s = "aabbccdd";
        Poco::translateInPlace(s, "abc", "ABC");
        poco_assert(s == "AABBCCdd");
    }
}

void test_replace(void)
{
    // replace
    {
        std::string s("aabbccdd");

        poco_assert(Poco::replace(s, std::string("aa"), std::string("xx")) == "xxbbccdd");
        poco_assert(Poco::replace(s, std::string("bb"), std::string("xx")) == "aaxxccdd");
        poco_assert(Poco::replace(s, std::string("dd"), std::string("xx")) == "aabbccxx");
        poco_assert(Poco::replace(s, std::string("bbcc"), std::string("xx")) == "aaxxdd");
        poco_assert(Poco::replace(s, std::string("b"), std::string("xx")) == "aaxxxxccdd");
        poco_assert(Poco::replace(s, std::string("bb"), std::string("")) == "aaccdd");
        poco_assert(Poco::replace(s, std::string("b"), std::string("")) == "aaccdd");
        poco_assert(Poco::replace(s, std::string("ee"), std::string("xx")) == "aabbccdd");
        poco_assert(Poco::replace(s, std::string("dd"), std::string("")) == "aabbcc");

        poco_assert(Poco::replace(s, "aa", "xx") == "xxbbccdd");
        poco_assert(Poco::replace(s, "bb", "xx") == "aaxxccdd");
        poco_assert(Poco::replace(s, "dd", "xx") == "aabbccxx");
        poco_assert(Poco::replace(s, "bbcc", "xx") == "aaxxdd");
        poco_assert(Poco::replace(s, "bb", "") == "aaccdd");
        poco_assert(Poco::replace(s, "b", "") == "aaccdd");
        poco_assert(Poco::replace(s, "ee", "xx") == "aabbccdd");
        poco_assert(Poco::replace(s, "dd", "") == "aabbcc");

        s = "aabbaabb";
        poco_assert(Poco::replace(s, std::string("aa"), std::string("")) == "bbbb");
        poco_assert(Poco::replace(s, std::string("a"), std::string("")) == "bbbb");
        poco_assert(Poco::replace(s, std::string("a"), std::string("x")) == "xxbbxxbb");
        poco_assert(Poco::replace(s, std::string("a"), std::string("xx")) == "xxxxbbxxxxbb");
        poco_assert(Poco::replace(s, std::string("aa"), std::string("xxx")) == "xxxbbxxxbb");

        poco_assert(Poco::replace(s, std::string("aa"), std::string("xx"), 2) == "aabbxxbb");

        poco_assert(Poco::replace(s, "aa", "") == "bbbb");
        poco_assert(Poco::replace(s, "a", "") == "bbbb");
        poco_assert(Poco::replace(s, "a", "x") == "xxbbxxbb");
        poco_assert(Poco::replace(s, "a", "xx") == "xxxxbbxxxxbb");
        poco_assert(Poco::replace(s, "aa", "xxx") == "xxxbbxxxbb");

        poco_assert(Poco::replace(s, "aa", "xx", 2) == "aabbxxbb");
        poco_assert(Poco::replace(s, 'a', 'x', 2) == "aabbxxbb");
        poco_assert(Poco::remove(s, 'a', 2) == "aabbbb");
        poco_assert(Poco::remove(s, 'a') == "bbbb");
        poco_assert(Poco::remove(s, 'b', 2) == "aaaa");
    }

    // replaceInPlace
    {
        std::string s("aabbccdd");

        Poco::replaceInPlace(s, std::string("aa"), std::string("xx"));
        poco_assert(s == "xxbbccdd");

        s = "aabbccdd";

        Poco::replaceInPlace(s, 'a', 'x');
        poco_assert(s == "xxbbccdd");

        Poco::replaceInPlace(s, 'x');
        poco_assert(s == "bbccdd");

        Poco::removeInPlace(s, 'b', 1);
        poco_assert(s == "bccdd");

        Poco::removeInPlace(s, 'd');
        poco_assert(s == "bcc");
    }
}

void test_starts_ends(void)
{
    // startsWith
    {
        std::string s1("o");

        poco_assert(Poco::startsWith(s1, std::string("o")));
        poco_assert(Poco::startsWith(s1, std::string("")));

        poco_assert(!Poco::startsWith(s1, std::string("O")));
        poco_assert(!Poco::startsWith(s1, std::string("1")));

        std::string s2("");

        poco_assert(Poco::startsWith(s2, std::string("")));

        poco_assert(!Poco::startsWith(s2, std::string("o")));

        std::string s3("oO");

        poco_assert(Poco::startsWith(s3, std::string("o")));

        poco_assert(!Poco::startsWith(s3, std::string(" o")));
    }

    // endsWith
    {
        std::string s1("o");

        poco_assert(Poco::endsWith(s1, std::string("o")));
        poco_assert(Poco::endsWith(s1, std::string("")));

        poco_assert(!Poco::endsWith(s1, std::string("O")));
        poco_assert(!Poco::endsWith(s1, std::string("1")));


        std::string s2("");

        poco_assert(Poco::endsWith(s2, std::string("")));

        poco_assert(!Poco::endsWith(s2, std::string("o")));

        std::string s3("Oo");

        poco_assert(Poco::endsWith(s3, std::string("o")));

        poco_assert(!Poco::endsWith(s3, std::string("o ")));
    }
}

void test_split(void)
{
    {
        Poco::StringTokenizer st("", "");
        poco_assert(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM);
        poco_assert(st.begin() == st.end());
    }
    {
        Poco::StringTokenizer st("", "", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.begin() == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc  ", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("  abc", "", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "b");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(st.find("c") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "b", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(st.find("c") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc", "bc");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(st.find("") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(st.find("") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("abc", "bc", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("a a,c c", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a a") == 0);
        poco_assert(st.find("c c") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c c");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st("a a,c c", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a a") == 0);
        poco_assert(st.find("c c") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c c");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st(" a a , , c c ", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find(" a a ") == 0);
        poco_assert(st.find(" ") == 1);
        poco_assert(st.find(" c c ") == 2);
        poco_assert(it != st.end());
        poco_assert(*it++ == " a a ");
        poco_assert(it != st.end());
        poco_assert(*it++ == " ");
        poco_assert(it != st.end());
        poco_assert(*it++ == " c c ");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a a") == 0);
        poco_assert(st.find("") == 1);
        poco_assert(st.find("c c") == 2);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c c");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st(" a a , , c c ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a a") == 0);
        poco_assert(st.find("c c") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "c c");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi , jk,  l ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(st.find("def") == 1);
        poco_assert(st.find("ghi") == 2);
        poco_assert(st.find("jk") == 3);
        poco_assert(st.find("l") == 4);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it != st.end());
        poco_assert(*it++ == "def");
        poco_assert(it != st.end());
        poco_assert(*it++ == "ghi");
        poco_assert(it != st.end());
        poco_assert(*it++ == "jk");
        poco_assert(it != st.end());
        poco_assert(*it++ == "l");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("abc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("abc") == 0);
        poco_assert(st.find("def") == 1);
        poco_assert(st.find("ghi") == 2);
        poco_assert(st.find("jk") == 3);
        poco_assert(st.find("l") == 4);
        poco_assert(it != st.end());
        poco_assert(*it++ == "abc");
        poco_assert(it != st.end());
        poco_assert(*it++ == "def");
        poco_assert(it != st.end());
        poco_assert(*it++ == "ghi");
        poco_assert(it != st.end());
        poco_assert(*it++ == "jk");
        poco_assert(it != st.end());
        poco_assert(*it++ == "l");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("a/bc,def,,ghi // jk,  l ", ",/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("a") == 0);
        poco_assert(st.find("bc") == 1);
        poco_assert(st.find("def") == 2);
        poco_assert(st.find("ghi") == 3);
        poco_assert(st.find("jk") == 4);
        poco_assert(st.find("l") == 5);
        poco_assert(it != st.end());
        poco_assert(*it++ == "a");
        poco_assert(it != st.end());
        poco_assert(*it++ == "bc");
        poco_assert(it != st.end());
        poco_assert(*it++ == "def");
        poco_assert(it != st.end());
        poco_assert(*it++ == "ghi");
        poco_assert(it != st.end());
        poco_assert(*it++ == "jk");
        poco_assert(it != st.end());
        poco_assert(*it++ == "l");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st(",ab,cd,", ",");
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("") == 0);
        poco_assert(st.find("ab") == 1);
        poco_assert(st.find("cd") == 2);
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it != st.end());
        poco_assert(*it++ == "ab");
        poco_assert(it != st.end());
        poco_assert(*it++ == "cd");
        poco_assert(it != st.end());
        poco_assert(*it++ == "");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st(",ab,cd,", ",", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("ab") == 0);
        poco_assert(st.find("cd") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "ab");
        poco_assert(it != st.end());
        poco_assert(*it++ == "cd");
        poco_assert(it == st.end());
    }
    {
        Poco::StringTokenizer st(" , ab , cd , ", ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        Poco::StringTokenizer::Iterator it = st.begin();
        poco_assert(st.find("ab") == 0);
        poco_assert(st.find("cd") == 1);
        poco_assert(it != st.end());
        poco_assert(*it++ == "ab");
        poco_assert(it != st.end());
        poco_assert(*it++ == "cd");
        poco_assert(it == st.end());
    }

    {
        Poco::StringTokenizer st("1 : 2 , : 3 ", ":,", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.count() == 3);
        poco_assert(st[0] == "1");
        poco_assert(st[1] == "2");
        poco_assert(st[2] == "3");
        poco_assert(st.find("1") == 0);
        poco_assert(st.find("2") == 1);
        poco_assert(st.find("3") == 2);
    }

    {
        Poco::StringTokenizer st(" 2- ","-", Poco::StringTokenizer::TOK_TRIM);
        poco_assert(st.count() == 2);
        poco_assert(st[0] == "2");
        poco_assert(st[1] == "");
    }

    {
        std::string           s = "#milan , a, b, c, hello# world, this, is# shmilyl#\"nice\" to # meet #you";
        Poco::StringTokenizer st(s, " ,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.count() == 13);
        poco_assert(st[0] == "milan");
        poco_assert(st[1] == "a");
        poco_assert(st[2] == "b");
        poco_assert(st[3] == "c");
        poco_assert(st[4] == "hello");
        poco_assert(st[5] == "world");
        poco_assert(st[6] == "this");
        poco_assert(st[7] == "is");
        poco_assert(st[8] == "shmilyl");
        poco_assert(st[9] == "\"nice\"");
        poco_assert(st[10] == "to");
        poco_assert(st[11] == "meet");
        poco_assert(st[12] == "you");
    }

    {
        std::string           s = "1#2#3#4#5#6#7#8#9####   ,,# , #hello ,#,192.168.111.1";
        Poco::StringTokenizer st(s, " .,#", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.count() == 14);
        poco_assert(st[0] == "1");
        poco_assert(st[1] == "2");
        poco_assert(st[2] == "3");
        poco_assert(st[3] == "4");
        poco_assert(st[4] == "5");
        poco_assert(st[5] == "6");
        poco_assert(st[6] == "7");
        poco_assert(st[7] == "8");
        poco_assert(st[8] == "9");
        poco_assert(st[9] == "hello");
        poco_assert(st[10] == "192");
        poco_assert(st[11] == "168");
        poco_assert(st[12] == "111");
        poco_assert(st[13] == "1");
    }

    {
        std::string           s = "This is first line.\nThis is second line.\n\n\nThis is third line.\nEnd line.\n";
        Poco::StringTokenizer st(s, "\n", Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.count() == 4);
        poco_assert(st[0] == "This is first line.");
        poco_assert(st[1] == "This is second line.");
        poco_assert(st[2] == "This is third line.");
        poco_assert(st[3] == "End line.");
    }
}

void test_regular_expression(void)
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

        poco_assert(re.match("AaBbCc"));
        poco_assert(re.match("AaBbCc", match) == 1);

        poco_assert(re.match("AaBbCc123_"));
        poco_assert(re.match("AaBbCc123_", match) == 1);

        poco_assert(!re.match("AaBbCc123-_"));
        poco_assert(re.match("AaBbCc123-_", match) == 0);

        poco_assert(!re.match("a1*"));
        poco_assert(re.match("a1*", match) == 0);
    }

    // 匹配以"fab"开头的所有字符串
    {
        Poco::RegularExpression        re("^fab.*");
        Poco::RegularExpression::Match match;

        poco_assert(re.match("fab"));
        poco_assert(re.match("fab", match) == 1);

        poco_assert(re.match("fabric"));
        poco_assert(re.match("fabric", match) == 1);

        poco_assert(!re.match("fba"));
        poco_assert(re.match("fba", match) == 0);

        poco_assert(!re.match(" fab"));
        poco_assert(re.match(" fab", match) == 0);
    }

    // 匹配包含"aaa"的所有字符串
    {
        Poco::RegularExpression        re(".*aaa.*");
        Poco::RegularExpression::Match match;

        poco_assert(re.match("aaa"));
        poco_assert(re.match("aaa", match) == 1);

        poco_assert(re.match("aaaaaa"));
        poco_assert(re.match("aaaaaa", match) == 1);

        poco_assert(re.match("baaa"));
        poco_assert(re.match("baaa", match) == 1);

        poco_assert(re.match("baaac"));
        poco_assert(re.match("baaac", match) == 1);

        poco_assert(!re.match("aa"));
        poco_assert(re.match("aa", match) == 0);

        poco_assert(!re.match("bbaacc"));
        poco_assert(re.match("bbaacc", match) == 0);
    }

    // 匹配以"ish"结尾的所有字符串
    {
        Poco::RegularExpression        re(".*ish$");
        Poco::RegularExpression::Match match;

        poco_assert(re.match("bish"));
        poco_assert(re.match("bish", match) == 1);

        poco_assert(re.match("rubbish"));
        poco_assert(re.match("rubbish", match) == 1);

        poco_assert(!re.match("bihs"));
        poco_assert(re.match("bihs", match) == 0);

        poco_assert(!re.match("rubbihs"));
        poco_assert(re.match("rubbihs", match) == 0);
    }

    // 匹配包含"bish"或者"fish"的所有字符串
    {
        Poco::RegularExpression        re(".*[bf]ish.*");
        Poco::RegularExpression::Match match;

        poco_assert(re.match("bish"));
        poco_assert(re.match("bish", match) == 1);

        poco_assert(re.match("fish"));
        poco_assert(re.match("fish", match) == 1);

        poco_assert(re.match("abish"));
        poco_assert(re.match("abish", match) == 1);

        poco_assert(re.match("fisherman"));
        poco_assert(re.match("fisherman", match) == 1);

        poco_assert(!re.match("ish"));
        poco_assert(re.match("ish", match) == 0);

        poco_assert(!re.match("fis."));
        poco_assert(re.match("fis.", match) == 0);
    }

    // 匹配以任意5个大写字母开头并以任意5个数字结尾的所有字符串
    {
        Poco::RegularExpression        re("^[A-Z]{5}.*[0-9]{5}$");
        Poco::RegularExpression::Match match;

        poco_assert(re.match("ABXYZclf.c*()#$12345"));
        poco_assert(re.match("ABXYZclf.c*()#$12345", match) == 1);

        poco_assert(re.match("ABXYZ12345"));
        poco_assert(re.match("ABXYZ12345", match) == 1);

        poco_assert(!re.match("ABXYZclf.c*()#$123"));
        poco_assert(re.match("ABXYZclf.c*()#$123", match) == 0);

        poco_assert(!re.match("ABXYclf.c*()#$12345"));
        poco_assert(re.match("ABXYclf.c*()#$12345", match) == 0);
    }

    // 匹配组数据
    {
        Poco::RegularExpression           re("([A-Z]{2}.[0-9]{2}) (?:[0-9]{2}.[0-9]{2}) ([a-z]{2}.[0-9]{2})");
        Poco::RegularExpression::MatchVec matchVec;

        std::string subject = "XY.00 00.00 xy.00";
        poco_assert(re.match(subject));
        poco_assert(re.match(subject, 0, matchVec) == 3);
        poco_assert(matchVec.size() == 3);
        poco_assert(matchVec[0].offset == 0);
        poco_assert(matchVec[0].length == 17);
        poco_assert(matchVec[1].offset == 0);
        poco_assert(matchVec[1].length == 5);
        poco_assert(matchVec[2].offset == 12);
        poco_assert(matchVec[2].length == 5);
        poco_assert(subject.substr(matchVec[0].offset, matchVec[0].length) == subject);
        poco_assert(subject.substr(matchVec[1].offset, matchVec[1].length) == "XY.00");
        poco_assert(subject.substr(matchVec[2].offset, matchVec[2].length) == "xy.00");
    }
}

void test_poco_string_all(void)
{
    test_trim_left();
    test_trim_right();
    test_trim();
    test_to_upper();
    test_to_lower();
    test_icompare();
    test_translate();
    test_replace();
    test_starts_ends();
    test_split();
    test_regular_expression();
}