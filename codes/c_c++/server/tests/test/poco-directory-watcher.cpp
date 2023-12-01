#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/types.h"
#include "Poco/Path.h"
#include "Poco/Mutex.h"
#include "Poco/Thread.h"
#include "Poco/Delegate.h"
#include "Poco/DirectoryWatcher.h"

struct DirEvent
{
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
    }

    void OnItemRemoved(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemRemoved";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
    }

    void OnItemModified(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemModified";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
    }

    void OnItemMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemMovedFrom";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
    }

    void OnItemMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& ev)
    {
        DirEvent de;
        de.type     = ev.event;
        de.callback = "OnItemMovedTo";
        de.path     = ev.item.path();

        Poco::Mutex::ScopedLock holder(_mutex);
        _eventList.emplace_back(std::move(de));
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
    std::vector<DirEvent> _eventList;
};

GTEST_TEST_F(PocoDirectoryWatcherTest, DirectoryWatcher)
{
    Poco::DirectoryWatcher dw(_curDir.toString());

    dw.itemAdded     += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemAdded);
    dw.itemRemoved   += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemRemoved);
    dw.itemModified  += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemModified);
    dw.itemMovedFrom += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemMovedFrom);
    dw.itemMovedTo   += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemMovedTo);
    dw.scanError     += Poco::delegate(static_cast<PocoDirectoryWatcherTest*>(this), &PocoDirectoryWatcherTest::OnItemScanError);

    Poco::Thread::sleep(100);

    Poco::Path p(_curDir);
}