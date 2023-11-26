#include "gtest/gtest.h"
#include "util/types.h"
#include "util/wrap_runnable.hpp"
#include "Poco/Event.h"
#include "Poco/Thread.h"
#include "Poco/Semaphore.h"
#include "Poco/RWLock.h"
#include "Poco/ThreadPool.h"
#include "Poco/Condition.h"

GTEST_TEST(PocoThreadingTest, Thread)
{
    Poco::Thread th("shmilyl");
    ASSERT_TRUE(!th.isRunning());
    ASSERT_TRUE(!Poco::Thread::current());

    bool        runFlag{};
    std::string threadName{};
    Poco::Event waitFinish{};

    // start
    {
        common::wrap_runnable call = [&runFlag, &threadName, &waitFinish]()
        {
            Poco::Thread* curThread = Poco::Thread::current();
            ASSERT_TRUE(curThread);

            runFlag    = true;
            threadName = curThread->getName();

            waitFinish.wait();
        };

        th.start(call);
        ASSERT_TRUE(th.isRunning());

        Poco::Thread::sleep(1);
        ASSERT_TRUE(th.isRunning());

        waitFinish.set();
        th.join();
        ASSERT_TRUE(!th.isRunning());
        ASSERT_TRUE(runFlag);
        ASSERT_TRUE(threadName == "shmilyl");
    }

    // startFunc
    {
        runFlag = false;
        threadName.clear();

        th.setName("milan");
        th.startFunc([&runFlag, &threadName, &waitFinish]() {
            Poco::Thread* curThread = Poco::Thread::current();
            ASSERT_TRUE(curThread);

            runFlag = true;
            threadName = curThread->getName();

            waitFinish.wait();
        });
        ASSERT_TRUE(th.isRunning());

        Poco::Thread::sleep(1);
        ASSERT_TRUE(th.isRunning());

        waitFinish.set();
        th.join();
        ASSERT_TRUE(!th.isRunning());
        ASSERT_TRUE(runFlag);
        ASSERT_TRUE(threadName == "milan");
    }
}

GTEST_TEST(PocoThreadingTest, ThreadTrySleep)
{
    Poco::Thread th;
    ASSERT_TRUE(!th.isRunning());

    bool        runFlag{};
    std::string threadName{};

    th.setName("darling");
    th.startFunc([&runFlag, &threadName]() {
        Poco::Thread* curThread = Poco::Thread::current();
        ASSERT_TRUE(curThread);

        runFlag    = true;
        threadName = curThread->getName();

        ASSERT_TRUE(!Poco::Thread::trySleep(1000000));
    });
    ASSERT_TRUE(th.isRunning());

    Poco::Thread::sleep(1);
    ASSERT_TRUE(th.isRunning());

    th.wakeUp();
    th.join();
    ASSERT_TRUE(!th.isRunning());
    ASSERT_TRUE(runFlag);
    ASSERT_TRUE(threadName == "darling");
}

GTEST_TEST(PocoThreadingTest, Semaphore)
{
    Poco::Semaphore sem(0, 3);
    ASSERT_TRUE(!sem.tryWait(1));

    // zero
    {
        sem.set();   // 1
        sem.wait();  // 0

        ASSERT_TRUE(!sem.tryWait(1));
        ASSERT_THROW(sem.wait(1), Poco::TimeoutException);

        sem.set();  // 1
        sem.set();  // 2

        ASSERT_TRUE(sem.tryWait(1)); // 1
        sem.wait();                  // 0

        ASSERT_TRUE(!sem.tryWait(1));
    }

    Poco::Thread th;
    ASSERT_TRUE(!th.isRunning());

    bool        runFlag{};
    std::string threadName{};

    th.setName("honey");
    th.startFunc([&sem, &runFlag, &threadName]() {
        Poco::Thread* curThread = Poco::Thread::current();
        ASSERT_TRUE(curThread);

        sem.wait();

        runFlag    = true;
        threadName = curThread->getName();
    });
    ASSERT_TRUE(th.isRunning());

    Poco::Thread::sleep(1);
    ASSERT_TRUE(th.isRunning());

    sem.set();
    th.join();
    ASSERT_TRUE(!th.isRunning());
    ASSERT_TRUE(runFlag);
    ASSERT_TRUE(threadName == "honey");
}

GTEST_TEST(PocoThreadingTest, RWLock)
{
    Poco::RWLock lock;
    volatile int counter = 0;

    // lock
    {
        counter = 0;
        common::wrap_runnable call = [&lock, &counter]()
        {
            int  lastCount = 0;
            bool success   = true;
            for (int idx = 0; idx != 1000; ++idx)
            {
                lock.readLock();
                lastCount = counter;
                for (int k = 0; k != 100; ++k)
                {
                    if (lastCount != counter)
                        success = false;
                    Poco::Thread::yield();
                }
                lock.unlock();

                lock.writeLock();
                for (int k = 0; k != 100; ++k)
                {
                    --counter;
                    Poco::Thread::yield();
                }

                for (int k = 0; k != 100; ++k)
                {
                    ++counter;
                    Poco::Thread::yield();
                }

                ++counter;
                if (counter <= lastCount)
                    success = false;
                lock.unlock();
            }

            ASSERT_TRUE(success);
        };

        Poco::Thread th1;
        Poco::Thread th2;
        Poco::Thread th3;
        Poco::Thread th4;
        Poco::Thread th5;

        th1.start(call);
        th2.start(call);
        th3.start(call);
        th4.start(call);
        th5.start(call);

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();

        ASSERT_TRUE(counter == 5000);
    }

    // tryLock
    {
        counter = 0;
        common::wrap_runnable call = [&lock, &counter]()
        {
            int  lastCount = 0;
            bool success   = true;
            for (int idx = 0; idx != 1000; ++idx)
            {
                while (!lock.tryReadLock())
                    Poco::Thread::yield();
                lastCount = counter;
                for (int k = 0; k != 100; ++k)
                {
                    if (lastCount != counter)
                        success = false;
                    Poco::Thread::yield();
                }
                lock.unlock();

                while (!lock.tryWriteLock())
                    Poco::Thread::yield();
                for (int k = 0; k != 100; ++k)
                {
                    --counter;
                    Poco::Thread::yield();
                }

                for (int k = 0; k != 100; ++k)
                {
                    ++counter;
                    Poco::Thread::yield();
                }

                ++counter;
                if (counter <= lastCount)
                    success = false;
                lock.unlock();
            }

            ASSERT_TRUE(success);
        };

        Poco::Thread th1;
        Poco::Thread th2;
        Poco::Thread th3;
        Poco::Thread th4;
        Poco::Thread th5;

        th1.start(call);
        th2.start(call);
        th3.start(call);
        th4.start(call);
        th5.start(call);

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();

        ASSERT_TRUE(counter == 5000);
    }
}

GTEST_TEST(PocoThreadingTest, ThreadPool)
{
    Poco::ThreadPool pool("TestThreadPool", 2, 3, 3);
    ASSERT_TRUE(pool.allocated() == 2);
    ASSERT_TRUE(pool.used() == 0);
    ASSERT_TRUE(pool.capacity() == 3);
    ASSERT_TRUE(pool.available() == 3);

    pool.addCapacity(1);
    ASSERT_TRUE(pool.allocated() == 2);
    ASSERT_TRUE(pool.used() == 0);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 4);

    Poco::Event     ready{Poco::Event::EVENT_MANUALRESET};
    Poco::FastMutex mutex{};
    volatile int    counter{};

    common::wrap_runnable call = [&ready, &mutex, &counter]()
    {
        Poco::Thread* curThread = Poco::Thread::current();
        ASSERT_TRUE(curThread);

        ready.wait();
        for (int idx = 0; idx != 10000; ++idx)
        {
            Poco::FastMutex::ScopedLock holder(mutex);
            ++counter;
        }
    };

    pool.start(call);
    ASSERT_TRUE(pool.allocated() == 2);
    ASSERT_TRUE(pool.used() == 1);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 3);

    pool.start(call);
    ASSERT_TRUE(pool.allocated() == 2);
    ASSERT_TRUE(pool.used() == 2);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 2);

    pool.start(call);
    ASSERT_TRUE(pool.allocated() == 3);
    ASSERT_TRUE(pool.used() == 3);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 1);

    pool.start(call);
    ASSERT_TRUE(pool.allocated() == 4);
    ASSERT_TRUE(pool.used() == 4);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 0);

    ASSERT_THROW(pool.start(call), Poco::NoThreadAvailableException);

    ready.set();
    pool.joinAll();
    ASSERT_TRUE(counter == 40000);
    ASSERT_TRUE(pool.allocated() == 4);
    ASSERT_TRUE(pool.used() == 0);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 4);

    pool.stopAll();
    ASSERT_TRUE(pool.allocated() == 0);
    ASSERT_TRUE(pool.used() == 0);
    ASSERT_TRUE(pool.capacity() == 4);
    ASSERT_TRUE(pool.available() == 4);
}

GTEST_TEST(PocoThreadingTest, Condition)
{
    Poco::FastMutex mutex{};
    Poco::Condition cond{};
    volatile int    counter{};

    common::wrap_runnable call = [&mutex, &cond, &counter]()
    {
        Poco::Thread* curThread = Poco::Thread::current();
        ASSERT_TRUE(curThread);

        {
            Poco::FastMutex::ScopedLock holder(mutex);
            cond.wait(mutex);

            ++counter;
        }
    };

    ASSERT_TRUE(counter == 0);

    Poco::Thread th1;
    Poco::Thread th2;
    Poco::Thread th3;
    Poco::Thread th4;
    Poco::Thread th5;

    th1.start(call);
    th2.start(call);
    th3.start(call);
    th4.start(call);
    th5.start(call);

    Poco::Thread::sleep(1);
    ASSERT_TRUE(counter == 0);

    cond.broadcast();

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();

    ASSERT_TRUE(counter == 5);
}