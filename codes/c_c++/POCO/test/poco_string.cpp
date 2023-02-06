#include "poco_string.h"
#include "Poco/Poco.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"

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
        Poco::StringTokenizer st(s, " ,#", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
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
        Poco::StringTokenizer st(s, " .,#", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
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
        Poco::StringTokenizer st(s, "\n", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        poco_assert(st.count() == 4);
        poco_assert(st[0] == "This is first line.");
        poco_assert(st[1] == "This is second line.");
        poco_assert(st[2] == "This is third line.");
        poco_assert(st[3] == "End line.");
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
}