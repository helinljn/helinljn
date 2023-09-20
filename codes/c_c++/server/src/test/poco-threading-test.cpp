#include "gtest/gtest.h"
#include "util/poco.h"
#include "util/WrapCallable.hpp"
#include "Poco/Event.h"
#include "Poco/Thread.h"
#include "Poco/Semaphore.h"

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
        Poco::WrapCallable call = [&runFlag, &threadName, &waitFinish]()
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