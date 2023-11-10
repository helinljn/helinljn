#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/Glob.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/SortedDirectoryIterator.h"
#include "Poco/RecursiveDirectoryIterator.h"

#include <map>
#include <set>
#include <vector>
#include <algorithm>

class PocoDirectoryIteratorTest : public testing::Test
{
public:
    PocoDirectoryIteratorTest(void)
        : Test()
        , _testDir()
    {
    }

protected:
    void SetUp(void) override
    {
        /*
        Build Directory Tree like this:

        testdir
          |-- 1
          |-- 2
          |-- A
          |-- B
          |-- c
          |-- d
          `-- first
            |-- 1
            |-- 2
            |-- A
            |-- B
            |-- c
            |-- d
            `-- second
              |-- 1
              |-- 2
              |-- A
              |-- B
              |-- c
              `-- d
        2 directories, 18 files
        */
        auto CreateSubdir = [](const Poco::Path& p)
        {
            Poco::File d(p);

            d.createDirectories();
            ASSERT_TRUE(d.exists() && d.isDirectory());

            const std::string strPath = p.toString();
            {
                Poco::FileStream f1(strPath + "d");
                Poco::FileStream f2(strPath + "1");
                Poco::FileStream f3(strPath + "A");
                Poco::FileStream f4(strPath + "2");
                Poco::FileStream f5(strPath + "B");
                Poco::FileStream f6(strPath + "c");
            }

            d = Poco::Path(strPath + "d");
            ASSERT_TRUE(d.exists() && d.isFile());

            d = Poco::Path(strPath + "1");
            ASSERT_TRUE(d.exists() && d.isFile());

            d = Poco::Path(strPath + "A");
            ASSERT_TRUE(d.exists() && d.isFile());

            d = Poco::Path(strPath + "2");
            ASSERT_TRUE(d.exists() && d.isFile());

            d = Poco::Path(strPath + "B");
            ASSERT_TRUE(d.exists() && d.isFile());

            d = Poco::Path(strPath + "c");
            ASSERT_TRUE(d.exists() && d.isFile());
        };

        _testDir = Poco::Path::current();
        _testDir.pushDirectory("testdir");

        Poco::Path temp = _testDir;
        CreateSubdir(temp);

        temp.pushDirectory("first");
        CreateSubdir(temp);

        temp.pushDirectory("second");
        CreateSubdir(temp);
    }

    void TearDown(void) override
    {
        Poco::File dir(_testDir);
        ASSERT_TRUE(dir.exists() && dir.isDirectory());

        dir.remove(true);
        ASSERT_TRUE(!dir.exists());
    }

protected:
    Poco::Path _testDir;
};

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobMatchChars)
{
    Poco::Glob g1("a");
    ASSERT_TRUE(g1.match("a"));
    ASSERT_TRUE(!g1.match("b"));
    ASSERT_TRUE(!g1.match("aa"));
    ASSERT_TRUE(!g1.match(""));

    Poco::Glob g2("ab");
    ASSERT_TRUE(g2.match("ab"));
    ASSERT_TRUE(!g2.match("aab"));
    ASSERT_TRUE(!g2.match("abab"));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobMatchQM)
{
    Poco::Glob g1("?");
    ASSERT_TRUE(g1.match("a"));
    ASSERT_TRUE(g1.match("b"));
    ASSERT_TRUE(!g1.match("aa"));
    ASSERT_TRUE(g1.match("."));

    Poco::Glob g2("\\?");
    ASSERT_TRUE(g2.match("?"));
    ASSERT_TRUE(!g2.match("a"));
    ASSERT_TRUE(!g2.match("ab"));

    Poco::Glob g3("a?");
    ASSERT_TRUE(g3.match("aa"));
    ASSERT_TRUE(g3.match("az"));
    ASSERT_TRUE(!g3.match("a"));
    ASSERT_TRUE(!g3.match("aaa"));

    Poco::Glob g4("??");
    ASSERT_TRUE(g4.match("aa"));
    ASSERT_TRUE(g4.match("ab"));
    ASSERT_TRUE(!g4.match("a"));
    ASSERT_TRUE(!g4.match("abc"));

    Poco::Glob g5("?a?");
    ASSERT_TRUE(g5.match("aaa"));
    ASSERT_TRUE(g5.match("bac"));
    ASSERT_TRUE(!g5.match("bbc"));
    ASSERT_TRUE(!g5.match("ba"));
    ASSERT_TRUE(!g5.match("ab"));

    Poco::Glob g6("a\\?");
    ASSERT_TRUE(g6.match("a?"));
    ASSERT_TRUE(!g6.match("az"));
    ASSERT_TRUE(!g6.match("a"));

    Poco::Glob g7("?", Poco::Glob::GLOB_DOT_SPECIAL);
    ASSERT_TRUE(g7.match("a"));
    ASSERT_TRUE(g7.match("b"));
    ASSERT_TRUE(!g7.match("aa"));
    ASSERT_TRUE(!g7.match("."));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobMatchAsterisk)
{
    Poco::Glob g1("*");
    ASSERT_TRUE(g1.match(""));
    ASSERT_TRUE(g1.match("a"));
    ASSERT_TRUE(g1.match("ab"));
    ASSERT_TRUE(g1.match("abc"));
    ASSERT_TRUE(g1.match("."));

    Poco::Glob g2("a*");
    ASSERT_TRUE(g2.match("a"));
    ASSERT_TRUE(g2.match("aa"));
    ASSERT_TRUE(g2.match("abc"));
    ASSERT_TRUE(!g2.match("b"));
    ASSERT_TRUE(!g2.match("ba"));

    Poco::Glob g3("ab*");
    ASSERT_TRUE(g3.match("ab"));
    ASSERT_TRUE(g3.match("abc"));
    ASSERT_TRUE(g3.match("abab"));
    ASSERT_TRUE(!g3.match("ac"));
    ASSERT_TRUE(!g3.match("baab"));

    Poco::Glob g4("*a");
    ASSERT_TRUE(g4.match("a"));
    ASSERT_TRUE(g4.match("ba"));
    ASSERT_TRUE(g4.match("aa"));
    ASSERT_TRUE(g4.match("aaaaaa"));
    ASSERT_TRUE(g4.match("bbbbba"));
    ASSERT_TRUE(!g4.match("b"));
    ASSERT_TRUE(!g4.match("ab"));
    ASSERT_TRUE(!g4.match("aaab"));

    Poco::Glob g5("a*a");
    ASSERT_TRUE(g5.match("aa"));
    ASSERT_TRUE(g5.match("aba"));
    ASSERT_TRUE(g5.match("abba"));
    ASSERT_TRUE(!g5.match("aab"));
    ASSERT_TRUE(!g5.match("aaab"));
    ASSERT_TRUE(!g5.match("baaaa"));

    Poco::Glob g6("a*b*c");
    ASSERT_TRUE(g6.match("abc"));
    ASSERT_TRUE(g6.match("aabbcc"));
    ASSERT_TRUE(g6.match("abcbbc"));
    ASSERT_TRUE(g6.match("aaaabbbbcccc"));
    ASSERT_TRUE(!g6.match("aaaabbbcb"));

    Poco::Glob g7("a*b*");
    ASSERT_TRUE(g7.match("aaabbb"));
    ASSERT_TRUE(g7.match("abababab"));
    ASSERT_TRUE(g7.match("ab"));
    ASSERT_TRUE(g7.match("aaaaab"));
    ASSERT_TRUE(!g7.match("a"));
    ASSERT_TRUE(!g7.match("aa"));
    ASSERT_TRUE(!g7.match("aaa"));

    Poco::Glob g8("**");
    ASSERT_TRUE(g1.match(""));
    ASSERT_TRUE(g1.match("a"));
    ASSERT_TRUE(g1.match("ab"));
    ASSERT_TRUE(g1.match("abc"));

    Poco::Glob g9("a\\*");
    ASSERT_TRUE(g9.match("a*"));
    ASSERT_TRUE(!g9.match("aa"));
    ASSERT_TRUE(!g9.match("a"));

    Poco::Glob g10("a*\\*");
    ASSERT_TRUE(g10.match("a*"));
    ASSERT_TRUE(g10.match("aaa*"));
    ASSERT_TRUE(!g10.match("a"));
    ASSERT_TRUE(!g10.match("aa"));

    Poco::Glob g11("*", Poco::Glob::GLOB_DOT_SPECIAL);
    ASSERT_TRUE(g11.match(""));
    ASSERT_TRUE(g11.match("a"));
    ASSERT_TRUE(g11.match("ab"));
    ASSERT_TRUE(g11.match("abc"));
    ASSERT_TRUE(!g11.match("."));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobMatchRange)
{
    Poco::Glob g1("[a]");
    ASSERT_TRUE(g1.match("a"));
    ASSERT_TRUE(!g1.match("b"));
    ASSERT_TRUE(!g1.match("aa"));

    Poco::Glob g2("[ab]");
    ASSERT_TRUE(g2.match("a"));
    ASSERT_TRUE(g2.match("b"));
    ASSERT_TRUE(!g2.match("c"));
    ASSERT_TRUE(!g2.match("ab"));

    Poco::Glob g3("[abc]");
    ASSERT_TRUE(g3.match("a"));
    ASSERT_TRUE(g3.match("b"));
    ASSERT_TRUE(g3.match("c"));
    ASSERT_TRUE(!g3.match("ab"));

    Poco::Glob g4("[a-z]");
    ASSERT_TRUE(g4.match("a"));
    ASSERT_TRUE(g4.match("z"));
    ASSERT_TRUE(!g4.match("A"));

    Poco::Glob g5("[!a]");
    ASSERT_TRUE(g5.match("b"));
    ASSERT_TRUE(g5.match("c"));
    ASSERT_TRUE(!g5.match("a"));
    ASSERT_TRUE(!g5.match("bb"));

    Poco::Glob g6("[!a-z]");
    ASSERT_TRUE(g6.match("A"));
    ASSERT_TRUE(!g6.match("a"));
    ASSERT_TRUE(!g6.match("z"));

    Poco::Glob g7("[0-9a-zA-Z_]");
    ASSERT_TRUE(g7.match("0"));
    ASSERT_TRUE(g7.match("1"));
    ASSERT_TRUE(g7.match("8"));
    ASSERT_TRUE(g7.match("9"));
    ASSERT_TRUE(g7.match("a"));
    ASSERT_TRUE(g7.match("b"));
    ASSERT_TRUE(g7.match("z"));
    ASSERT_TRUE(g7.match("A"));
    ASSERT_TRUE(g7.match("Z"));
    ASSERT_TRUE(g7.match("_"));
    ASSERT_TRUE(!g7.match("-"));

    Poco::Glob g8("[1-3]");
    ASSERT_TRUE(g8.match("1"));
    ASSERT_TRUE(g8.match("2"));
    ASSERT_TRUE(g8.match("3"));
    ASSERT_TRUE(!g8.match("0"));
    ASSERT_TRUE(!g8.match("4"));

    Poco::Glob g9("[!1-3]");
    ASSERT_TRUE(g9.match("0"));
    ASSERT_TRUE(g9.match("4"));
    ASSERT_TRUE(!g9.match("1"));
    ASSERT_TRUE(!g9.match("2"));
    ASSERT_TRUE(!g9.match("3"));

    Poco::Glob g10("[\\!a]");
    ASSERT_TRUE(g10.match("!"));
    ASSERT_TRUE(g10.match("a"));
    ASSERT_TRUE(!g10.match("x"));

    Poco::Glob g11("[a\\-c]");
    ASSERT_TRUE(g11.match("a"));
    ASSERT_TRUE(g11.match("c"));
    ASSERT_TRUE(g11.match("-"));
    ASSERT_TRUE(!g11.match("b"));

    Poco::Glob g12("[\\]]");
    ASSERT_TRUE(g12.match("]"));
    ASSERT_TRUE(!g12.match("["));

    Poco::Glob g13("[[\\]]");
    ASSERT_TRUE(g13.match("["));
    ASSERT_TRUE(g13.match("]"));
    ASSERT_TRUE(!g13.match("x"));

    Poco::Glob g14("\\[]");
    ASSERT_TRUE(g14.match("[]"));
    ASSERT_TRUE(!g14.match("[["));

    Poco::Glob g15("a[bc]");
    ASSERT_TRUE(g15.match("ab"));
    ASSERT_TRUE(g15.match("ac"));
    ASSERT_TRUE(!g15.match("a"));
    ASSERT_TRUE(!g15.match("aa"));

    Poco::Glob g16("[ab]c");
    ASSERT_TRUE(g16.match("ac"));
    ASSERT_TRUE(g16.match("bc"));
    ASSERT_TRUE(!g16.match("a"));
    ASSERT_TRUE(!g16.match("b"));
    ASSERT_TRUE(!g16.match("c"));
    ASSERT_TRUE(!g16.match("aa"));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobMisc)
{
    Poco::Glob g1("*.cpp");
    ASSERT_TRUE(g1.match("Poco::Glob.cpp"));
    ASSERT_TRUE(!g1.match("Poco::Glob.h"));

    Poco::Glob g2("*.[hc]");
    ASSERT_TRUE(g2.match("foo.c"));
    ASSERT_TRUE(g2.match("foo.h"));
    ASSERT_TRUE(!g2.match("foo.i"));

    Poco::Glob g3("*.*");
    ASSERT_TRUE(g3.match("foo.cpp"));
    ASSERT_TRUE(g3.match("foo.h"));
    ASSERT_TRUE(g3.match("foo."));
    ASSERT_TRUE(!g3.match("foo"));

    Poco::Glob g4("File*.?pp");
    ASSERT_TRUE(g4.match("File.hpp"));
    ASSERT_TRUE(g4.match("File.cpp"));
    ASSERT_TRUE(g4.match("Filesystem.hpp"));
    ASSERT_TRUE(!g4.match("File.h"));

    Poco::Glob g5("File*.[ch]*");
    ASSERT_TRUE(g5.match("File.hpp"));
    ASSERT_TRUE(g5.match("File.cpp"));
    ASSERT_TRUE(g5.match("Filesystem.hpp"));
    ASSERT_TRUE(g5.match("File.h"));
    ASSERT_TRUE(g5.match("Filesystem.cp"));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, GlobCaseless)
{
    Poco::Glob g1("*.cpp", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g1.match("Glob.cpp"));
    ASSERT_TRUE(!g1.match("Glob.h"));
    ASSERT_TRUE(g1.match("Glob.CPP"));
    ASSERT_TRUE(!g1.match("Glob.H"));

    Poco::Glob g2("*.[hc]", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g2.match("foo.c"));
    ASSERT_TRUE(g2.match("foo.h"));
    ASSERT_TRUE(!g2.match("foo.i"));
    ASSERT_TRUE(g2.match("foo.C"));
    ASSERT_TRUE(g2.match("foo.H"));
    ASSERT_TRUE(!g2.match("foo.I"));

    Poco::Glob g4("File*.?pp", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g4.match("file.hpp"));
    ASSERT_TRUE(g4.match("FILE.CPP"));
    ASSERT_TRUE(g4.match("filesystem.hpp"));
    ASSERT_TRUE(g4.match("FILESYSTEM.HPP"));
    ASSERT_TRUE(!g4.match("FILE.H"));
    ASSERT_TRUE(!g4.match("file.h"));

    Poco::Glob g5("File*.[ch]*", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g5.match("file.hpp"));
    ASSERT_TRUE(g5.match("FILE.HPP"));
    ASSERT_TRUE(g5.match("file.cpp"));
    ASSERT_TRUE(g5.match("FILE.CPP"));
    ASSERT_TRUE(g5.match("filesystem.hpp"));
    ASSERT_TRUE(g5.match("FILESYSTEM.HPP"));
    ASSERT_TRUE(g5.match("file.h"));
    ASSERT_TRUE(g5.match("FILE.H"));
    ASSERT_TRUE(g5.match("filesystem.cp"));
    ASSERT_TRUE(g5.match("FILESYSTEM.CP"));

    Poco::Glob g6("[abc]", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g6.match("a"));
    ASSERT_TRUE(g6.match("b"));
    ASSERT_TRUE(g6.match("c"));
    ASSERT_TRUE(g6.match("A"));
    ASSERT_TRUE(g6.match("B"));
    ASSERT_TRUE(g6.match("C"));

    Poco::Glob g7("[a-f]", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g7.match("a"));
    ASSERT_TRUE(g7.match("b"));
    ASSERT_TRUE(g7.match("f"));
    ASSERT_TRUE(!g7.match("g"));
    ASSERT_TRUE(g7.match("A"));
    ASSERT_TRUE(g7.match("B"));
    ASSERT_TRUE(g7.match("F"));
    ASSERT_TRUE(!g7.match("G"));

    Poco::Glob g8("[A-F]", Poco::Glob::GLOB_CASELESS);
    ASSERT_TRUE(g8.match("a"));
    ASSERT_TRUE(g8.match("b"));
    ASSERT_TRUE(g8.match("f"));
    ASSERT_TRUE(!g8.match("g"));
    ASSERT_TRUE(g8.match("A"));
    ASSERT_TRUE(g8.match("B"));
    ASSERT_TRUE(g8.match("F"));
    ASSERT_TRUE(!g8.match("G"));
}

GTEST_TEST_F(PocoDirectoryIteratorTest, Glob)
{
    auto CreateFile = [](const std::string& path)
    {
        Poco::Path p(path);
        Poco::File f(p.parent());

        f.createDirectories();

        const std::string      pstr(p.toString());
        Poco::FileOutputStream f1(pstr);
        f1 << pstr;
        f1.close();
    };

    auto TranslatePaths = [](std::set<std::string>& paths)
    {
        std::set<std::string> translated;
        for (const auto& path : paths)
        {
            Poco::Path p(path);
            translated.insert(p.toString(Poco::Path::PATH_UNIX));
        }

        paths = translated;
    };

    CreateFile("globtest/Makefile");
    CreateFile("globtest/.hidden");
    CreateFile("globtest/include/one.h");
    CreateFile("globtest/include/two.h");
    CreateFile("globtest/src/one.c");
    CreateFile("globtest/src/two.c");
    CreateFile("globtest/src/main.c");
    CreateFile("globtest/testsuite/src/test.h");
    CreateFile("globtest/testsuite/src/test.c");
    CreateFile("globtest/testsuite/src/main.c");

    {
        std::set<std::string> files;
        Poco::Glob::glob("globtest/*", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 5);
        ASSERT_TRUE(files.find("globtest/Makefile") != files.end());
        ASSERT_TRUE(files.find("globtest/.hidden") != files.end());
        ASSERT_TRUE(files.find("globtest/include/") != files.end());
        ASSERT_TRUE(files.find("globtest/src/") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/") != files.end());

        files.clear();
        Poco::Glob::glob("GlobTest/*", files, Poco::Glob::GLOB_CASELESS);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 5);
        ASSERT_TRUE(files.find("globtest/Makefile") != files.end());
        ASSERT_TRUE(files.find("globtest/.hidden") != files.end());
        ASSERT_TRUE(files.find("globtest/include/") != files.end());
        ASSERT_TRUE(files.find("globtest/src/") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/") != files.end());

        files.clear();
        Poco::Glob::glob("globtest/*/*.[hc]", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 5);
        ASSERT_TRUE(files.find("globtest/include/one.h") != files.end());
        ASSERT_TRUE(files.find("globtest/include/two.h") != files.end());
        ASSERT_TRUE(files.find("globtest/src/one.c") != files.end());
        ASSERT_TRUE(files.find("globtest/src/one.c") != files.end());
        ASSERT_TRUE(files.find("globtest/src/main.c") != files.end());

        files.clear();
        Poco::Glob::glob("gl?bt?st/*/*/*.c", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 2);
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.c") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/main.c") != files.end());

        files.clear();
        Poco::Glob::glob("Gl?bT?st/*/*/*.C", files, Poco::Glob::GLOB_CASELESS);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 2);
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.c") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/main.c") != files.end());

        files.clear();
        Poco::Glob::glob("globtest/*/src/*", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 3);
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.h") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.c") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/main.c") != files.end());

        files.clear();
        Poco::Glob::glob("globtest/*/", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 3);
        ASSERT_TRUE(files.find("globtest/include/") != files.end());
        ASSERT_TRUE(files.find("globtest/src/") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/") != files.end());

        files.clear();
        Poco::Glob::glob("globtest/testsuite/src/*", "globtest/testsuite/", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 3);
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.h") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/test.c") != files.end());
        ASSERT_TRUE(files.find("globtest/testsuite/src/main.c") != files.end());

        files.clear();
        Poco::Glob::glob("globtest/../*/testsuite/*/", files);
        TranslatePaths(files);
        ASSERT_TRUE(files.size() == 1);
    }

    Poco::File dir("globtest");
    ASSERT_TRUE(dir.exists() && dir.isDirectory());

    dir.remove(true);
    ASSERT_TRUE(!dir.exists());
}

GTEST_TEST_F(PocoDirectoryIteratorTest, DirectoryIterator)
{
    // file
    {
        Poco::Path path = _testDir;
        path.makeDirectory().setFileName("1");
#if POCO_OS == POCO_OS_WINDOWS_NT
        ASSERT_THROW(Poco::DirectoryIterator iterBegin(path), Poco::PathSyntaxException);
#else
        ASSERT_THROW(Poco::DirectoryIterator iterBegin(path), Poco::OpenFileException);
#endif
    }

    Poco::DirectoryIterator  iterBegin(_testDir);
    Poco::DirectoryIterator  iterEnd;
    std::vector<std::string> result;
    Poco::Path               temp;
    while (iterBegin != iterEnd)
    {
        temp = iterBegin->path();
        result.emplace_back(temp.getFileName());
        ++iterBegin;
    }

    ASSERT_TRUE(result.size() == 7);
    ASSERT_TRUE(std::find(result.begin(), result.end(), "d") != result.end());
    ASSERT_TRUE(std::find(result.begin(), result.end(), "1") != result.end());
    ASSERT_TRUE(std::find(result.begin(), result.end(), "A") != result.end());
    ASSERT_TRUE(std::find(result.begin(), result.end(), "2") != result.end());
    ASSERT_TRUE(std::find(result.begin(), result.end(), "B") != result.end());
    ASSERT_TRUE(std::find(result.begin(), result.end(), "c") != result.end());
}

GTEST_TEST_F(PocoDirectoryIteratorTest, SortedDirectoryIterator)
{
    Poco::SortedDirectoryIterator iterBegin(_testDir);
    Poco::SortedDirectoryIterator iterEnd;
    std::vector<std::string>      result;
    Poco::Path                    temp;
    while (iterBegin != iterEnd)
    {
        temp = iterBegin->path();
        result.emplace_back(temp.getFileName());
        ++iterBegin;
    }

    ASSERT_TRUE(result.size() == 7);
    ASSERT_TRUE(result[0] == "first");
    ASSERT_TRUE(result[1] == "1");
    ASSERT_TRUE(result[2] == "2");
    ASSERT_TRUE(result[3] == "A");
    ASSERT_TRUE(result[4] == "B");
    ASSERT_TRUE(result[5] == "c");
    ASSERT_TRUE(result[6] == "d");
}

GTEST_TEST_F(PocoDirectoryIteratorTest, RecursiveDirectoryIterator)
{
    Poco::RecursiveDirectoryIterator iterBegin(_testDir);
    Poco::RecursiveDirectoryIterator iterEnd;
    std::map<std::string, int>       result;
    Poco::Path                       tempPath;
    while (iterBegin != iterEnd)
    {
        tempPath = iterBegin->path();
        if (auto iter = result.find(tempPath.getFileName()); iter != result.end())
            iter->second += 1;
        else
            result.emplace(tempPath.getFileName(), 1);

        ++iterBegin;
    }

    ASSERT_TRUE(result.size() == 8);
    ASSERT_TRUE(result["first"] == 1);
    ASSERT_TRUE(result["second"] == 1);
    ASSERT_TRUE(result["1"] == 3);
    ASSERT_TRUE(result["2"] == 3);
    ASSERT_TRUE(result["A"] == 3);
    ASSERT_TRUE(result["B"] == 3);
    ASSERT_TRUE(result["c"] == 3);
    ASSERT_TRUE(result["d"] == 3);
}