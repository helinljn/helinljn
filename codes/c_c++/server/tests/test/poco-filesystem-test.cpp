#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/types.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/TemporaryFile.h"

GTEST_TEST(PocoFilesystemTest, Path)
{
    const std::string curDir        = Poco::Path::current();
    const std::string homeDir       = Poco::Path::home();
    const std::string tempDir       = Poco::Path::temp();
    const std::string configDir     = Poco::Path::config();
    const std::string tempHomeDir   = Poco::Path::tempHome();
    const std::string dataHomeDir   = Poco::Path::dataHome();
    const std::string cacheHomeDir  = Poco::Path::cacheHome();
    const std::string configHomeDir = Poco::Path::configHome();
    const std::string nullDevice    = Poco::Path::null();
    fmt::print(
        "curDir        = {}\n"
        "homeDir       = {}\n"
        "tempDir       = {}\n"
        "configDir     = {}\n"
        "tempHomeDir   = {}\n"
        "dataHomeDir   = {}\n"
        "cacheHomeDir  = {}\n"
        "configHomeDir = {}\n"
        "nullDevice    = {}\n",
        curDir, homeDir, tempDir, configDir, tempHomeDir, dataHomeDir, cacheHomeDir, configHomeDir, nullDevice
    );

    Poco::Path p;

    p = curDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = homeDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = tempDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = configDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = tempHomeDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = dataHomeDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = cacheHomeDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p = configHomeDir;
    ASSERT_TRUE(p.isDirectory() && !p.isFile() && p.isAbsolute() && !p.isRelative());

    p.parse("/usr/home/foo/bar", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar");

    p.makeDirectory();
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 4);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar/");

    p.parseDirectory("/usr/home/foo/bar", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 4);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar/");

    p = Poco::Path::forDirectory("/usr/home/foo/bar", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 4);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar/");

    p.makeFile();
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar");

    p.makeParent();
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/");

    p.pushDirectory("bar");
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 4);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/bar/");

    p.popDirectory();
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/home/foo/");

    std::vector<std::string> roots;
    Poco::Path::listRoots(roots);
    fmt::print("roots:");
    for (const auto& item : roots)
        fmt::print("  {}", item);
    fmt::print("\n");
}

GTEST_TEST(PocoFilesystemTest, PathParseUnix)
{
    Poco::Path p;

    p.parse("", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 0);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "");

    p.parse("/", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 0);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/");

    p.parse("/usr/local/", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 2);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/local/");

    p.parse("usr/local/", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 2);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "usr/local/");

    p.parse("usr/local", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 1);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "usr/local");

    p.parse("/usr//local//./bin//", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "/usr/local/bin/");

    p.parse("./usr//local/bin//./", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "usr/local/bin/");

    p.parse("./usr//local/bin//.", Poco::Path::PATH_UNIX);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_UNIX) == "usr/local/bin/.");
}

GTEST_TEST(PocoFilesystemTest, PathParseWindows)
{
    Poco::Path p;
    p.parse("", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 0);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "");

    p.parse("/", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 0);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "\\");

    p.parse("\\", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 0);
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "\\");

    p.parse("/usr\\local/", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 2);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "\\usr\\local\\");

    p.parse("usr\\local/", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 2);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "usr\\local\\");

    p.parse("usr/local", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 1);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "usr\\local");

    p.parse("/usr\\local//./bin\\\\", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(!p.isRelative());
    ASSERT_TRUE(p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

    p.parse("./usr//local/bin//./", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(p.isDirectory());
    ASSERT_TRUE(!p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "usr\\local\\bin\\");

    p.parse(".\\usr\\\\local\\bin\\\\.", Poco::Path::PATH_WINDOWS);
    ASSERT_TRUE(p.isRelative());
    ASSERT_TRUE(!p.isAbsolute());
    ASSERT_TRUE(p.depth() == 3);
    ASSERT_TRUE(p[0] == "usr");
    ASSERT_TRUE(p[1] == "local");
    ASSERT_TRUE(p[2] == "bin");
    ASSERT_TRUE(!p.isDirectory());
    ASSERT_TRUE(p.isFile());
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "usr\\local\\bin\\.");
}

GTEST_TEST(PocoFilesystemTest, PathTryParse)
{
    Poco::Path p;
#if POCO_OS == POCO_OS_WINDOWS_NT
    ASSERT_TRUE(p.tryParse("c:\\windows\\system32"));
    ASSERT_TRUE(p.toString() == "c:\\windows\\system32");

    ASSERT_TRUE(!p.tryParse("c:foo.bar"));
    ASSERT_TRUE(p.toString() == "c:\\windows\\system32");
#else
    ASSERT_TRUE(p.tryParse("/etc/passwd"));
    ASSERT_TRUE(p.toString() == "/etc/passwd");
#endif

    ASSERT_TRUE(p.tryParse("c:\\windows\\system", Poco::Path::PATH_WINDOWS));
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "c:\\windows\\system");

    ASSERT_TRUE(!p.tryParse("c:foo.bar", Poco::Path::PATH_WINDOWS));
    ASSERT_TRUE(p.toString(Poco::Path::PATH_WINDOWS) == "c:\\windows\\system");
}

GTEST_TEST(PocoFilesystemTest, PathBaseNameExt)
{
    Poco::Path p("/usr/home/001/foo.bar");
    ASSERT_TRUE(p.getFileName() == "foo.bar");
    ASSERT_TRUE(p.getBaseName() == "foo");
    ASSERT_TRUE(p.getExtension() == "bar");

    p.setBaseName("readme");
    ASSERT_TRUE(p.getFileName() == "readme.bar");
    ASSERT_TRUE(p.getBaseName() == "readme");
    ASSERT_TRUE(p.getExtension() == "bar");

    p.setExtension("txt");
    ASSERT_TRUE(p.getFileName() == "readme.txt");
    ASSERT_TRUE(p.getBaseName() == "readme");
    ASSERT_TRUE(p.getExtension() == "txt");

    p.setExtension("html");
    ASSERT_TRUE(p.getFileName() == "readme.html");
    ASSERT_TRUE(p.getBaseName() == "readme");
    ASSERT_TRUE(p.getExtension() == "html");

    p.setBaseName("index");
    ASSERT_TRUE(p.getFileName() == "index.html");
    ASSERT_TRUE(p.getBaseName() == "index");
    ASSERT_TRUE(p.getExtension() == "html");
}

GTEST_TEST(PocoFilesystemTest, File)
{
    Poco::File f = Poco::Path("testfile.dat");
    ASSERT_TRUE(!f.exists());
    ASSERT_THROW(f.canRead(), Poco::Exception);
    ASSERT_THROW(f.canWrite(), Poco::Exception);
    ASSERT_THROW(f.isFile(), Poco::Exception);
    ASSERT_THROW(f.isLink(), Poco::Exception);
    ASSERT_THROW(f.isDirectory(), Poco::Exception);
    ASSERT_THROW(f.created(), Poco::Exception);
    ASSERT_THROW(f.getLastModified(), Poco::Exception);
    ASSERT_THROW(f.setLastModified(Poco::Timestamp()), Poco::Exception);
    ASSERT_THROW(f.getSize(), Poco::Exception);
    ASSERT_THROW(f.setSize(0), Poco::Exception);
    ASSERT_THROW(f.setWriteable(), Poco::Exception);
    ASSERT_THROW(f.setReadOnly(), Poco::Exception);
    ASSERT_THROW(f.copyTo(".."), Poco::Exception);
    ASSERT_THROW(f.moveTo(".."), Poco::Exception);
    ASSERT_THROW(f.renameTo(".."), Poco::Exception);
    ASSERT_THROW(f.linkTo(".."), Poco::Exception);
    ASSERT_THROW(f.remove(), Poco::Exception);

#if POCO_OS == POCO_OS_WINDOWS_NT
    ASSERT_TRUE(!f.canExecute());
    ASSERT_TRUE(!f.isDevice());
    ASSERT_THROW(f.isHidden(), Poco::Exception);
#else
    ASSERT_THROW(f.canExecute(), Poco::Exception);
    ASSERT_THROW(f.isDevice(), Poco::Exception);
    ASSERT_THROW(f.setExecutable(), Poco::Exception);
#endif

    ASSERT_TRUE(f.createFile());
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.canRead());
    ASSERT_TRUE(f.canWrite());
    ASSERT_TRUE(!f.canExecute());
    ASSERT_TRUE(f.isFile());
    ASSERT_TRUE(!f.isLink());
    ASSERT_TRUE(!f.isDevice());
    ASSERT_TRUE(!f.isHidden());
    ASSERT_TRUE(!f.isDirectory());
    ASSERT_TRUE(f.getSize() == 0);

    f.setWriteable(false);
    ASSERT_TRUE(f.canRead());
    ASSERT_TRUE(!f.canWrite());

    f.setReadOnly(false);
    ASSERT_TRUE(f.canRead());
    ASSERT_TRUE(f.canWrite());

    ASSERT_TRUE(!f.createFile());
    f.remove();
    ASSERT_TRUE(!f.exists());

    Poco::FileOutputStream fos;
    fos.open(f.path(), std::ios::out | std::ios::app);
    fos << "0123456789";
    fos.close();

    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.getSize() == 10);

    ASSERT_TRUE(!f.createFile());
    f.remove();
    ASSERT_TRUE(!f.exists());
}

GTEST_TEST(PocoFilesystemTest, TemporaryFile)
{
    Poco::File  f;
    std::string tmpFile;

    // system temp dir
    {
        Poco::TemporaryFile tf;
        ASSERT_TRUE(!tf.exists());

        tmpFile = tf.path();
        fmt::print("temp file: {}\n", tmpFile);

        Poco::FileOutputStream fos;
        fos.open(tf.path(), std::ios::out | std::ios::trunc);
        fos << "0123456789";
        fos.close();

        ASSERT_TRUE(tf.isFile());
        ASSERT_TRUE(tf.exists());
        ASSERT_TRUE(tf.getSize() == 10);
    }

    f = tmpFile;
    ASSERT_TRUE(!f.exists());

    // current dir
    {
        Poco::TemporaryFile tf(Poco::Path::current());
        ASSERT_TRUE(!tf.exists());

        tmpFile = tf.path();
        fmt::print("temp file: {}\n", tmpFile);

        Poco::FileOutputStream fos;
        fos.open(tf.path(), std::ios::out | std::ios::trunc);
        fos << "0123456789";
        fos.close();

        ASSERT_TRUE(tf.isFile());
        ASSERT_TRUE(tf.exists());
        ASSERT_TRUE(tf.getSize() == 10);
    }

    f = tmpFile;
    ASSERT_TRUE(!f.exists());

    // keep
    {
        Poco::TemporaryFile tf(Poco::Path::current());

        tf.keep();
        ASSERT_TRUE(!tf.exists());

        tmpFile = tf.path();
        fmt::print("temp file: {}\n", tmpFile);

        Poco::FileOutputStream fos;
        fos.open(tf.path(), std::ios::out | std::ios::trunc);
        fos << "0123456789";
        fos.close();

        ASSERT_TRUE(tf.isFile());
        ASSERT_TRUE(tf.exists());
        ASSERT_TRUE(tf.getSize() == 10);
    }

    f = tmpFile;
    ASSERT_TRUE(f.isFile());
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.getSize() == 10);
    f.remove();
}

GTEST_TEST(PocoFilesystemTest, Dir)
{
    Poco::File d = Poco::Path("testdir");
    ASSERT_TRUE(!d.exists());
    ASSERT_THROW(d.remove(true), Poco::Exception);

    ASSERT_TRUE(d.createDirectory());
    ASSERT_TRUE(d.exists());
    ASSERT_TRUE(d.isDirectory());

    std::vector<std::string> files;
    d.list(files);
    ASSERT_TRUE(files.empty());

    Poco::File f = Poco::Path("testdir/file1");
    ASSERT_TRUE(f.createFile());

    f = Poco::Path("testdir/file2");
    ASSERT_TRUE(f.createFile());

    f = Poco::Path("testdir/file3");
    ASSERT_TRUE(f.createFile());

    f = Poco::Path("testdir/testdir2/testdir3");
    f.createDirectories();
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(d.isDirectory());

    d.remove(true);
}

GTEST_TEST(PocoFilesystemTest, CopyFile)
{
    // copy and rename
    {
        Poco::File f1 = Poco::Path("testfile.dat");
        ASSERT_TRUE(!f1.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        Poco::File f2 = Poco::Path("../testfile1.dat");
        ASSERT_TRUE(!f2.exists());

        f1.copyTo(f2.path());
        ASSERT_TRUE(f2.exists());
        ASSERT_TRUE(f1.getSize() == f2.getSize());

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }

    // copy and keep name
    {
        Poco::File f1 = Poco::Path("testfile.dat");
        ASSERT_TRUE(!f1.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        Poco::File f2 = Poco::Path("../testfile.dat");
        ASSERT_TRUE(!f2.exists());

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == f2.getSize());

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }

    // overwrite
    {
        Poco::File f1 = Poco::Path("testfile.dat");
        ASSERT_TRUE(!f1.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        Poco::File f2 = Poco::Path("../testfile.dat");
        ASSERT_TRUE(!f2.exists());

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 20 && f2.getSize() == 20);

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }

    // copy failed if destination file exists
    {
        Poco::File f1 = Poco::Path("testfile.dat");
        ASSERT_TRUE(!f1.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        Poco::File f2 = Poco::Path("../testfile.dat");
        ASSERT_TRUE(!f2.exists());

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        ASSERT_THROW(f1.copyTo("..", Poco::File::OPT_FAIL_ON_OVERWRITE), Poco::FileExistsException);

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }
}

GTEST_TEST(PocoFilesystemTest, MoveFile)
{
    // move and rename
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile1.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.moveTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 10);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // move and keep name
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.moveTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 10);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // overwrite
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.moveTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 20);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // move failed if destination file exists
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        ASSERT_THROW(f1.moveTo("..", Poco::File::OPT_FAIL_ON_OVERWRITE), Poco::FileExistsException);

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }
}

GTEST_TEST(PocoFilesystemTest, RenameFile)
{
    // move and rename
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "testfile1.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.renameTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 10);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // move and keep name
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.renameTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 10);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // overwrite
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.renameTo(f2.path());
        ASSERT_TRUE(f1.path() == f2.path());
        ASSERT_TRUE(f1.exists() && f1.getSize() == 20);

        f1.remove();
        ASSERT_TRUE(!f1.exists());

        f1 = src;
        ASSERT_TRUE(!f1.exists());
    }

    // move failed if destination file exists
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "../testfile.dat";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.copyTo("..");
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

        ASSERT_THROW(f1.renameTo("..", Poco::File::OPT_FAIL_ON_OVERWRITE), Poco::FileExistsException);

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }
}

GTEST_TEST(PocoFilesystemTest, RenameDir)
{
    // move and rename
    {
        const Poco::Path destDir     = "../testdir111";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.renameTo(dd.path());
        ASSERT_TRUE(sd.path() == dd.path());
        ASSERT_TRUE(sd.exists());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(!sd.exists());
    }

    // move and keep name
    {
        const Poco::Path destDir     = "../testdir";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.renameTo(dd.path());
        ASSERT_TRUE(sd.path() == dd.path());
        ASSERT_TRUE(sd.exists());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(!sd.exists());
    }
}

GTEST_TEST(PocoFilesystemTest, SymbolicLink)
{
    // Windows下需要管理员权限才能创建软链接
#if POCO_OS != POCO_OS_WINDOWS_NT
    // file
    {
        const Poco::Path src  = "testfile.dat";
        const Poco::Path dest = "testfile.dat.slink";

        Poco::File f1 = src, f2 = dest;
        ASSERT_TRUE(!f1.exists() && !f2.exists());

        Poco::FileOutputStream fos;
        fos.open(f1.path(), std::ios::out | std::ios::app);
        fos << "0123456789";
        fos.close();

        f1.linkTo(f2.path());
        ASSERT_THROW(f1.linkTo(f2.path()), Poco::FileExistsException);
        ASSERT_TRUE(f1.exists() && f2.exists());
        ASSERT_TRUE(f1.isFile() && f2.isLink() && f2.isFile());

        f1.remove();
        f2.remove();
        ASSERT_TRUE(!f1.exists() && !f2.exists());
    }

    // dir
    {
        const Poco::Path src  = "testdir";
        const Poco::Path dest = "testdir.slink";

        Poco::File d1 = src, d2 = dest;
        ASSERT_TRUE(!d1.exists() && !d2.exists());

        ASSERT_TRUE(d1.createDirectory());

        d1.linkTo(d2.path());
        ASSERT_THROW(d1.linkTo(d2.path()), Poco::FileExistsException);
        ASSERT_TRUE(d1.exists() && d2.exists());
        ASSERT_TRUE(d1.isDirectory() && d2.isLink() && d2.isDirectory());

        d1.remove();
        d2.remove();
        ASSERT_TRUE(!d1.exists() && !d2.exists());
    }
#endif
}

GTEST_TEST(PocoFilesystemTest, CopyDir)
{
    // copy and rename
    {
        const Poco::Path destDir     = "../testdir111";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo(dd.path());
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());

        std::vector<std::string> srcFileList;
        sd.list(srcFileList);

        std::vector<std::string> destFileList;
        dd.list(destFileList);

        ASSERT_TRUE(srcFileList == destFileList);

        sd.remove(true);
        dd.remove(true);
        ASSERT_TRUE(!sd.exists() && !dd.exists());
    }

    // copy and keep name
    {
        const Poco::Path destDir     = "../testdir";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());

        std::vector<std::string> srcFileList;
        sd.list(srcFileList);

        std::vector<std::string> destFileList;
        dd.list(destFileList);

        ASSERT_TRUE(srcFileList == destFileList);

        sd.remove(true);
        dd.remove(true);
        ASSERT_TRUE(!sd.exists() && !dd.exists());
    }

    // overwrite
    {
        const Poco::Path srcDir       = "testdir";
        const Poco::Path srcDirFile1  = "testdir/file1";
        const Poco::Path srcDirFile2  = "testdir/file2";
        const Poco::Path destDir      = "../testdir";
        const Poco::Path destDirFile1 = "../testdir/file1";
        const Poco::Path destDirFile2 = "../testdir/file2";
        const Poco::Path destDirFile3 = "../testdir/file3";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());
        {
            Poco::FileOutputStream fos;
            fos.open(srcDirFile1.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            fos.open(srcDirFile2.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            Poco::File f1 = srcDirFile1, f2 = srcDirFile2;
            ASSERT_TRUE(f1.exists() && f2.exists());
            ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10);

            Poco::File f3 = destDirFile3;
            ASSERT_TRUE(!f3.exists());
            ASSERT_TRUE(f3.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());

        Poco::File f1 = destDirFile1, f2 = destDirFile2, f3 = destDirFile3;
        ASSERT_TRUE(f1.exists() && f2.exists() && f3.exists());
        ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 10 && f3.getSize() == 0);

        sd.remove(true);
        dd.remove(true);
        ASSERT_TRUE(!sd.exists() && !dd.exists());
    }

    // copy failed if destination file exists
    {
        const Poco::Path srcDir       = "testdir";
        const Poco::Path srcDirFile1  = "testdir/file1";
        const Poco::Path srcDirFile2  = "testdir/file2";
        const Poco::Path srcDirFile3  = "testdir/file3";
        const Poco::Path destDir      = "../testdir";
        const Poco::Path destDirFile1 = "../testdir/file1";
        const Poco::Path destDirFile2 = "../testdir/file2";
        const Poco::Path destDirFile3 = "../testdir/file3";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile3;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());
        {
            Poco::File f1 = destDirFile1, f3 = destDirFile3;
            ASSERT_TRUE(f1.exists() && f3.exists());

            f1.remove();
            f3.remove();
            ASSERT_TRUE(!f1.exists() && !f3.exists());
        }
        // ls testdir
        // :/$ file1 file2 file3
        //
        // ls ../testdir
        // :/$ file2

        ASSERT_THROW(sd.copyTo("..", Poco::File::OPT_FAIL_ON_OVERWRITE), Poco::FileExistsException);

        sd.remove(true);
        dd.remove(true);
        ASSERT_TRUE(!sd.exists() && !dd.exists());
    }
}

GTEST_TEST(PocoFilesystemTest, MoveDir)
{
    // move and rename
    {
        const Poco::Path destDir     = "../testdir111";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.moveTo(dd.path());
        ASSERT_TRUE(sd.path() == dd.path());
        ASSERT_TRUE(sd.exists());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(!sd.exists());
    }

    // move and keep name
    {
        const Poco::Path destDir     = "../testdir";
        const Poco::Path srcDir      = "testdir";
        const Poco::Path srcDirFile1 = "testdir/file1";
        const Poco::Path srcDirFile2 = "testdir/file2";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.moveTo(dd.path());
        ASSERT_TRUE(sd.path() == dd.path());
        ASSERT_TRUE(sd.exists());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(!sd.exists());
    }

    // overwrite
    {
        const Poco::Path srcDir       = "testdir";
        const Poco::Path srcDirFile1  = "testdir/file1";
        const Poco::Path srcDirFile2  = "testdir/file2";
        const Poco::Path srcDirFile3  = "testdir/file3";
        const Poco::Path destDir      = "../testdir";
        const Poco::Path destDirFile1 = "../testdir/file1";
        const Poco::Path destDirFile2 = "../testdir/file2";
        const Poco::Path destDirFile3 = "../testdir/file3";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile3;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());
        {
            Poco::File f1 = destDirFile1, f3 = destDirFile3;
            ASSERT_TRUE(f1.exists() && f3.exists());

            f1.remove();
            f3.remove();
            ASSERT_TRUE(!f1.exists() && !f3.exists());

            Poco::FileOutputStream fos;
            fos.open(srcDirFile1.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            fos.open(srcDirFile3.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            f1 = srcDirFile1, f3 = srcDirFile3;
            ASSERT_TRUE(f1.exists() && f3.exists());
            ASSERT_TRUE(f1.getSize() == 10 && f3.getSize() == 10);
        }
        // ls testdir
        // :/$ file1(10) file2 file3(10)
        //
        // ls ../testdir
        // :/$ file2

        sd.moveTo("..");
        {
            Poco::File f1 = srcDirFile1, f2 = srcDirFile2, f3 = srcDirFile3;
            ASSERT_TRUE(!f1.exists() && !f2.exists() && !f3.exists());

            f1 = destDirFile1, f2 = destDirFile2, f3 = destDirFile3;
            ASSERT_TRUE(f1.exists() && f2.exists() && f3.exists());
            ASSERT_TRUE(f1.getSize() == 10 && f2.getSize() == 0 && f3.getSize() == 10);
        }
        // ls ../testdir
        // :/$ file1(10) file2 file3(10)

        sd = destDir;
        ASSERT_TRUE(sd.exists() && sd.isDirectory());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(!sd.exists());
    }

    // move failed if destination file exists
    {
        const Poco::Path srcDir       = "testdir";
        const Poco::Path srcDirFile1  = "testdir/file1";
        const Poco::Path srcDirFile2  = "testdir/file2";
        const Poco::Path srcDirFile3  = "testdir/file3";
        const Poco::Path destDir      = "../testdir";
        const Poco::Path destDirFile1 = "../testdir/file1";
        const Poco::Path destDirFile2 = "../testdir/file2";
        const Poco::Path destDirFile3 = "../testdir/file3";

        Poco::File sd = srcDir, dd = destDir;
        ASSERT_TRUE(!sd.exists() && !dd.exists());
        ASSERT_TRUE(sd.createDirectory());
        {
            Poco::File f = srcDirFile1;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile2;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());

            f = srcDirFile3;
            ASSERT_TRUE(!f.exists());
            ASSERT_TRUE(f.createFile());
        }

        sd.copyTo("..");
        ASSERT_TRUE(sd.exists() && dd.exists());
        ASSERT_TRUE(sd.isDirectory() && dd.isDirectory());
        {
            Poco::File f1 = destDirFile1, f3 = destDirFile3;
            ASSERT_TRUE(f1.exists() && f3.exists());

            f1.remove();
            f3.remove();
            ASSERT_TRUE(!f1.exists() && !f3.exists());

            Poco::FileOutputStream fos;
            fos.open(srcDirFile1.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            fos.open(srcDirFile3.toString(), std::ios::out | std::ios::app);
            fos << "0123456789";
            fos.close();

            f1 = srcDirFile1, f3 = srcDirFile3;
            ASSERT_TRUE(f1.exists() && f3.exists());
            ASSERT_TRUE(f1.getSize() == 10 && f3.getSize() == 10);
        }
        // ls testdir
        // :/$ file1(10) file2 file3(10)
        //
        // ls ../testdir
        // :/$ file2

        ASSERT_THROW(sd.moveTo("..", Poco::File::OPT_FAIL_ON_OVERWRITE), Poco::FileExistsException);

        sd = destDir;
        ASSERT_TRUE(sd.exists() && sd.isDirectory());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());

        sd = srcDir;
        ASSERT_TRUE(sd.exists() && sd.isDirectory());

        sd.remove(true);
        ASSERT_TRUE(!sd.exists());
    }
}