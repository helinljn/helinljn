#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/types.h"
#include "util/wrap_runnable.hpp"
#include "Poco/AtomicCounter.h"
#include "Poco/Thread.h"
#include "Poco/Observer.h"
#include "Poco/NObserver.h"
#include "Poco/Notification.h"
#include "Poco/NotificationCenter.h"
#include "Poco/NotificationQueue.h"
#include "Poco/PriorityNotificationQueue.h"

class TestNotification : public Poco::Notification
{
public:
    TestNotification(std::string data)
        : Notification()
        , _data(std::move(data))
    {
        ++_count;
    }

    ~TestNotification(void) override
    {
        --_count;
    }

    const std::string& data(void) const
    {
        return _data;
    }

    static int count(void)
    {
        return _count.value();
    }

private:
    static Poco::AtomicCounter _count;
    std::string                _data;
};

Poco::AtomicCounter TestNotification::_count;

class PocoNotificationTest : public testing::Test
{
public:
    PocoNotificationTest(void)
        : Test()
    {
    }

    void HandleRawPtr(Poco::Notification* pn)
    {
        ASSERT_TRUE(pn && pn->referenceCount() == 2);

        auto iter = _handleList.find("RawPtr");
        if (iter == _handleList.end())
            _handleList.emplace("RawPtr", 1);
        else
            iter->second += 1;

        // must release it when done
        pn->release();
    }

    void HandleAutoPtr(const Poco::AutoPtr<Poco::Notification>& pn)
    {
        ASSERT_TRUE(pn && pn->referenceCount() == 2);

        auto iter = _handleList.find("AutoPtr");
        if (iter == _handleList.end())
            _handleList.emplace("AutoPtr", 1);
        else
            iter->second += 1;
    }

protected:
    void SetUp(void) override
    {
        ASSERT_TRUE(_handleList.empty());
    }

    void TearDown(void) override
    {
        _handleList.clear();
    }

protected:
    std::map<std::string, int> _handleList;
};

GTEST_TEST_F(PocoNotificationTest, NotificationCenter)
{
    Poco::NotificationCenter nc;

    // Observer
    {
        Poco::Observer<PocoNotificationTest, Poco::Notification> o1(*this, &PocoNotificationTest::HandleRawPtr);
        Poco::Observer<PocoNotificationTest, Poco::Notification> o2(*this, &PocoNotificationTest::HandleRawPtr);

        nc.addObserver(o1);
        ASSERT_TRUE(nc.hasObserver(o1) && nc.countObservers() == 1);

        nc.addObserver(o2);
        ASSERT_TRUE(nc.hasObserver(o2) && nc.countObservers() == 2);

        nc.postNotification(new Poco::Notification);
        ASSERT_TRUE(_handleList["RawPtr"] == 2);

        nc.removeObserver(o1);
        ASSERT_TRUE(/* !nc.hasObserver(o1)  && */nc.countObservers() == 1);

        nc.removeObserver(o2);
        ASSERT_TRUE(!nc.hasObserver(o2) && nc.countObservers() == 0);
    }

    // NObserver
    {
        Poco::NObserver<PocoNotificationTest, Poco::Notification> o1(*this, &PocoNotificationTest::HandleAutoPtr);
        Poco::NObserver<PocoNotificationTest, Poco::Notification> o2(*this, &PocoNotificationTest::HandleAutoPtr);

        nc.addObserver(o1);
        ASSERT_TRUE(nc.hasObserver(o1) && nc.countObservers() == 1);

        nc.addObserver(o2);
        ASSERT_TRUE(nc.hasObserver(o2) && nc.countObservers() == 2);

        nc.postNotification(new Poco::Notification);
        ASSERT_TRUE(_handleList["AutoPtr"] == 2);

        nc.removeObserver(o1);
        ASSERT_TRUE(/* !nc.hasObserver(o1)  && */nc.countObservers() == 1);

        nc.removeObserver(o2);
        ASSERT_TRUE(!nc.hasObserver(o2) && nc.countObservers() == 0);
    }

    // Observer & NObserver
    {
        Poco::Observer<PocoNotificationTest, Poco::Notification>  o1(*this, &PocoNotificationTest::HandleRawPtr);
        Poco::NObserver<PocoNotificationTest, Poco::Notification> o2(*this, &PocoNotificationTest::HandleAutoPtr);

        nc.addObserver(o1);
        ASSERT_TRUE(nc.hasObserver(o1) && nc.countObservers() == 1);

        nc.addObserver(o2);
        ASSERT_TRUE(nc.hasObserver(o2) && nc.countObservers() == 2);

        nc.postNotification(new Poco::Notification);
        ASSERT_TRUE(_handleList["RawPtr"] == 3 && _handleList["AutoPtr"] == 3);

        nc.removeObserver(o1);
        ASSERT_TRUE(!nc.hasObserver(o1)  && nc.countObservers() == 1);

        nc.removeObserver(o2);
        ASSERT_TRUE(!nc.hasObserver(o2) && nc.countObservers() == 0);
    }
}

GTEST_TEST_F(PocoNotificationTest, NotificationQueue)
{
    Poco::NotificationQueue nq;

    // enqueue & dequeue
    {
        nq.enqueueNotification(new TestNotification("first"));
        nq.enqueueNotification(new TestNotification("second"));
        nq.enqueueUrgentNotification(new TestNotification("third"));
        ASSERT_TRUE(!nq.empty() && nq.size() == 3 && !nq.hasIdleThreads());

        Poco::AutoPtr<TestNotification> pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "third");
        ASSERT_TRUE(!nq.empty() && nq.size() == 2 && !nq.hasIdleThreads());

        pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "first");
        ASSERT_TRUE(!nq.empty() && nq.size() == 1 && !nq.hasIdleThreads());

        pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "second");
        ASSERT_TRUE(nq.empty() && nq.size() == 0 && !nq.hasIdleThreads());
    }

    ASSERT_TRUE(Poco::Thread::current() == nullptr);

    // threads
    {
        Poco::FastMutex       mutex;
        std::atomic_bool      stopFlag   = false;
        common::wrap_runnable workerFunc = [this, &nq, &mutex, &stopFlag]()
        {
            Poco::Thread* curThread = Poco::Thread::current();
            ASSERT_TRUE(curThread);

            const std::string threadName = curThread->getName();
            fmt::print("thread({}) started\n", threadName);

            Poco::AutoPtr<TestNotification> pn;
            while (!stopFlag.load() || !nq.empty())
            {
                pn = dynamic_cast<TestNotification*>(nq.waitDequeueNotification(1000));
                if (pn.isNull())
                    continue;

                {
                    Poco::FastMutex::ScopedLock holder(mutex);
                    if (auto iter = _handleList.find(threadName); iter != _handleList.end())
                        iter->second += 1;
                    else
                        _handleList.emplace(threadName, 1);
                }
            }

            fmt::print("thread({}) finished\n", threadName);
        };

        static_assert(std::is_copy_constructible_v<decltype(workerFunc)>);
        static_assert(std::is_move_constructible_v<decltype(workerFunc)>);
        static_assert(std::is_copy_assignable_v<decltype(workerFunc)>);
        static_assert(std::is_move_assignable_v<decltype(workerFunc)>);

        Poco::Thread th1("thread_1");
        Poco::Thread th2("thread_2");
        Poco::Thread th3("thread_3");
        Poco::Thread th4("thread_4");
        Poco::Thread th5("thread_5");

        th1.start(workerFunc);
        th2.start(workerFunc);
        th3.start(workerFunc);
        th4.start(workerFunc);
        th5.start(workerFunc);

        for (int idx = 0; idx != 2000; ++idx)
        {
            nq.enqueueNotification(new TestNotification(fmt::format("{}_{}", "Notification", idx + 1)));
        }

        while (!nq.empty())
        {
            Poco::Thread::sleep(1);
        }

        stopFlag.store(true);
        nq.wakeUpAll();

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();

        ASSERT_TRUE(!th1.isRunning() && !th2.isRunning() && !th3.isRunning() && !th4.isRunning() && !th5.isRunning());
        ASSERT_TRUE(nq.empty() && nq.size() == 0 && !nq.hasIdleThreads() && !_handleList.empty() && TestNotification::count() == 0);

        for (const auto& [id, count] : _handleList)
        {
            ASSERT_TRUE(count > 0);
            fmt::print("thread({}) count({})\n", id, count);
        }
    }
}

GTEST_TEST_F(PocoNotificationTest, PriorityNotificationQueue)
{
    Poco::PriorityNotificationQueue nq;

    // enqueue & dequeue
    {
        nq.enqueueNotification(new TestNotification("first"), 2);
        nq.enqueueNotification(new TestNotification("second"), 3);
        nq.enqueueNotification(new TestNotification("third"), 1);
        ASSERT_TRUE(!nq.empty() && nq.size() == 3 && !nq.hasIdleThreads());

        Poco::AutoPtr<TestNotification> pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "third");
        ASSERT_TRUE(!nq.empty() && nq.size() == 2 && !nq.hasIdleThreads());

        pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "first");
        ASSERT_TRUE(!nq.empty() && nq.size() == 1 && !nq.hasIdleThreads());

        pn = dynamic_cast<TestNotification*>(nq.dequeueNotification());
        ASSERT_TRUE(pn && pn->referenceCount() == 1 && pn->data() == "second");
        ASSERT_TRUE(nq.empty() && nq.size() == 0 && !nq.hasIdleThreads());
    }

    ASSERT_TRUE(Poco::Thread::current() == nullptr);

    // threads
    {
        Poco::FastMutex       mutex;
        std::atomic_bool      stopFlag   = false;
        common::wrap_runnable workerFunc = [this, &nq, &mutex, &stopFlag]()
        {
            Poco::Thread* curThread = Poco::Thread::current();
            ASSERT_TRUE(curThread);

            const std::string threadName = curThread->getName();
            fmt::print("thread({}) started\n", threadName);

            Poco::AutoPtr<TestNotification> pn;
            while (!stopFlag.load() || !nq.empty())
            {
                pn = dynamic_cast<TestNotification*>(nq.waitDequeueNotification(1000));
                if (pn.isNull())
                    continue;

                {
                    Poco::FastMutex::ScopedLock holder(mutex);
                    if (auto iter = _handleList.find(threadName); iter != _handleList.end())
                        iter->second += 1;
                    else
                        _handleList.emplace(threadName, 1);
                }
            }

            fmt::print("thread({}) finished\n", threadName);
        };

        static_assert(std::is_copy_constructible_v<decltype(workerFunc)>);
        static_assert(std::is_move_constructible_v<decltype(workerFunc)>);
        static_assert(std::is_copy_assignable_v<decltype(workerFunc)>);
        static_assert(std::is_move_assignable_v<decltype(workerFunc)>);

        Poco::Thread th1("thread_1");
        Poco::Thread th2("thread_2");
        Poco::Thread th3("thread_3");
        Poco::Thread th4("thread_4");
        Poco::Thread th5("thread_5");

        th1.start(workerFunc);
        th2.start(workerFunc);
        th3.start(workerFunc);
        th4.start(workerFunc);
        th5.start(workerFunc);

        for (int idx = 0; idx != 2000; ++idx)
        {
            nq.enqueueNotification(new TestNotification(fmt::format("{}_{}", "Notification", idx + 1)), idx);
        }

        while (!nq.empty())
        {
            Poco::Thread::sleep(1);
        }

        stopFlag.store(true);
        nq.wakeUpAll();

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();

        ASSERT_TRUE(!th1.isRunning() && !th2.isRunning() && !th3.isRunning() && !th4.isRunning() && !th5.isRunning());
        ASSERT_TRUE(nq.empty() && nq.size() == 0 && !nq.hasIdleThreads() && !_handleList.empty() && TestNotification::count() == 0);

        for (const auto& [id, count] : _handleList)
        {
            ASSERT_TRUE(count > 0);
            fmt::print("thread({}) count({})\n", id, count);
        }
    }
}