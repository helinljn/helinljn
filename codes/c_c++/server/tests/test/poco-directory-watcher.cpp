#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/types.h"
#include "Poco/Path.h"
#include "Poco/Mutex.h"
#include "Poco/Event.h"
#include "Poco/Thread.h"
#include "Poco/Delegate.h"
#include "Poco/DirectoryWatcher.h"

struct DirEvent
{
public:
    DirEvent(void)
        : type(Poco::DirectoryWatcher::DirectoryEventType::DW_ITEM_ADDED)
        , callback()
        , path()
    {
    }

public:
    Poco::DirectoryWatcher::DirectoryEventType type;
    std::string                                callback;
    std::string                                path;
};

class PocoDirectoryWatcherTest : public testing::Test
{
public:
    PocoDirectoryWatcherTest(void)
        : Test()
        , _curDir()
        , _mutex()
        , _event()
        , _eventList()
    {
    }

    void OnItemAdded(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemAdded";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
        _event.set();
    }

    void OnItemRemoved(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemRemoved";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
        _event.set();
    }

    void OnItemModified(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemModified";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
        _event.set();
    }

    void OnItemMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemMovedFrom";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
        _event.set();
    }

    void OnItemMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemMovedTo";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
        _event.set();
    }

    void OnItemScanError(const Poco::Exception& exc)
    {
        fmt::print("ONItemScanError -> {}\n", exc.displayText());
    }

protected:
    void SetUp(void) override
    {
        _curDir = Poco::Path::current();
        _curDir.pushDirectory("test_dir_watch");

        Poco::File dir(_curDir.toString());
        if (dir.exists())
            dir.remove(true);
        dir.createDirectories();
    }

    void TearDown(void) override
    {
        Poco::File dir(_curDir.toString());
        if (dir.exists())
            dir.remove(true);

        _eventList.clear();
        _curDir.clear();
    }

protected:
    Poco::Path            _curDir;
    Poco::Mutex           _mutex;
    Poco::Event           _event;
    std::vector<DirEvent> _eventList;
};

GTEST_TEST_F(PocoDirectoryWatcherTest, DirectoryWatcher)
{
    Poco::DirectoryWatcher dw(_curDir.toString(), Poco::DirectoryWatcher::DW_FILTER_ENABLE_ALL, 1);

    dw.itemAdded     += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemAdded);
    dw.itemRemoved   += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemRemoved);
    dw.itemModified  += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemModified);
    dw.itemMovedFrom += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemMovedFrom);
    dw.itemMovedTo   += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemMovedTo);
    dw.scanError     += Poco::delegate(dynamic_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemScanError);

    Poco::Thread::sleep(1);

    // add file
    {
        Poco::Path p(_curDir);
        p.setFileName("test.txt");

        std::ofstream ofs(p.toString(), std::ios::app);
        ofs << "Hello, world!";
        ofs.close();

        _event.wait();
        _eventList.clear();
    }

    // modified file
    {
        Poco::Path p(_curDir);
        p.setFileName("test.txt");

        std::ofstream ofs(p.toString(), std::ios::app);
        ofs << "Hello, world!";
        ofs.close();

        _event.wait();
        _eventList.clear();
    }

    // remove file
    {
        Poco::Path p(_curDir);
        p.setFileName("test.txt");

        Poco::File f(p);
        ASSERT_TRUE(f.exists() && f.isFile());
        f.remove();

        _event.wait();
        _eventList.clear();
    }

    // move file
    {
        Poco::Path p1(_curDir);
        p1.setFileName("test1.txt");

        std::ofstream ofs(p1.toString(), std::ios::app);
        ofs << "Hello, world!";
        ofs.close();

        Poco::Path p2(_curDir);
        p2.setFileName("test2.txt");

        Poco::File f(p1);
        f.renameTo(p2.toString());

        _event.wait();
        _eventList.clear();
    }
}