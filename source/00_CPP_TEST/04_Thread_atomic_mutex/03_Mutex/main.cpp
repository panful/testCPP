/*
 * 01 std::mutex 互斥锁
 * 02 std::mutex::try_lock()
 * 03 std::recursive_mutex 递归锁
 * 04 std::shared_lock 读写锁
 * 05 std::shared_lock 读写锁
 * 
 * 
 * 07 std::timed_mutex 超时锁
 *
 * 09 自旋锁
 */

#define TEST43

#ifdef TEST1

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex m;

void workerWithOutMutex()
{
    // 不使用互斥锁，下面四行内容打印顺序可能是乱的
    // 例如线程1打印完"111111"，线程2接着打印"111111"而不是线程1接着打印"222222"
    // 所有的线程可能会同时执行这一段代码
    std::cout << "-----------------\n";
    std::cout << "111111\n";
    std::cout << "222222\n";
    std::cout << "333333\n";
}

void workerWithMutex()
{
    // 使用锁，下面四行内容打印的顺序是整齐的
    // 当一个线程占用锁之后，即线程进入m.lock()之后，这个线程就会占用这个锁
    // 其他线程不能进入锁，就被阻塞，直到占用锁的线程退出锁的区域后，其他线程才能依次进入锁
    // lock() ：第一个线程进去后，后面的线程会进入阻塞休眠状态，被放入到阻塞队列中。
    // unlock()：加锁的线程执行完后，会解锁，然后去阻塞队列中唤醒阻塞线程
    // lock()到unlock()之间的代码段只能有一个线程执行
    m.lock();
    std::cout << "-----------------\n";
    std::cout << "111111\n";
    std::cout << "222222\n";
    std::cout << "333333\n";
    m.unlock();
}

int main()
{
    constexpr size_t threadsNum { 10 };

    // 使用互斥锁
    {
        std::vector<std::thread> threads;

        for (size_t i = 0; i < threadsNum; i++)
        {
            threads.emplace_back(std::thread(workerWithMutex));
        }

        for (auto&& t : threads)
        {
            t.join();
        }
    }

    std::cout << "++++++++++++++++++++++++++++++++++++++++++\n";

    // 不使用互斥锁
    {
        std::vector<std::thread> threads;

        for (size_t i = 0; i < threadsNum; i++)
        {
            threads.emplace_back(std::thread(workerWithOutMutex));
        }

        for (auto&& t : threads)
        {
            t.join();
        }
    }

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::chrono::milliseconds interval(100);
std::mutex mutex;

// 两个线程都能修改 'job_shared' mutex 将保护此变量
int job_shared = 0;

// 只有一个线程能修改 'job_exclusive' 不需要保护
int job_exclusive = 0;

// 此线程能修改 'job_shared' 和 'job_exclusive'
void job_1()
{
    // 令 'job_2' 持锁
    std::this_thread::sleep_for(interval);

    while (true)
    {
        // 尝试锁定 mutex 以修改 'job_shared'
        // try_lock()获取锁成功时，返回true
        // 使用了try_lock()之后不能再使用lock()，否则抛出异常
        // 允许此函数虚假地失败而返回 false ，即使互斥当前未为任何其他线程所锁定。
        if (mutex.try_lock())
        {
            std::cout << "job shared (" << job_shared << ")\n";
            mutex.unlock();
            return;
        }
        else
        {
            // 不能获取锁以修改 'job_shared' 但有其他工作可做
            ++job_exclusive;
            std::cout << "job exclusive (" << job_exclusive << ")\n";
            std::this_thread::sleep_for(interval);
        }
    }
}

// 此线程只能修改 'job_shared'
void job_2()
{
    mutex.lock();
    std::this_thread::sleep_for(5 * interval);
    ++job_shared;
    mutex.unlock();
}

int main()
{
    std::thread thread_1(job_1);
    std::thread thread_2(job_2);

    thread_1.join();
    thread_2.join();
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <mutex>
#include <thread>

// 同线程同一把锁可以上锁多次, 避免死锁
// lock()一次锁计数器加一, unlock()一次锁计数器减一
// 当锁计数器归零时才会真正解锁
class X
{
    std::recursive_mutex rm;
    std::string shared;

public:
    void fun1()
    {
        std::lock_guard<std::recursive_mutex> lk(rm);
        shared = "fun1";
        std::cout << "in fun1, shared variable is now " << shared << '\n';
    }
    void fun2()
    {
        std::lock_guard<std::recursive_mutex> lk(rm);
        shared = "fun2";
        std::cout << "in fun2, shared variable is now " << shared << '\n';
        fun1(); // 递归锁在此处变得有用
        // 注意此处打印的shared是fun1
        std::cout << "back in fun2, shared variable is " << shared << '\n';
    };
};

int main()
{
    X x;
    std::thread t1(&X::fun1, &x);
    std::thread t2(&X::fun2, &x);
    t1.join();
    t2.join();
}

#endif // TEST3

#ifdef TEST4

#include "../00_Timer/myUtility.hpp"
#include <iostream>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
class Test
{
public:
    size_t Get() const
    {
        std::shared_lock<std::shared_mutex> lk(m_mutex);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        return m_number;
    }
    void Set(size_t n)
    {
        std::unique_lock<std::shared_mutex> lk(m_mutex);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        m_number = n;
    }

private:
    mutable std::shared_mutex m_mutex;
    size_t m_number { 0 };
};

int main()
{
    Test t;
    constexpr size_t numOfThreads { 10000 };

    // 读比写花费的时间少得多

    {
        std::cout << "--- read ---\n";

        MyUtility::ConsumeTime<std::chrono::milliseconds> ct;

        std::list<std::thread> threads;
        for (size_t i = 0; i < numOfThreads; i++)
        {
            threads.emplace_back(std::thread(&Test::Get, &t));
        }
        for (auto&& t : threads)
        {
            t.join();
        }
    }

    {
        std::cout << "--- write ---\n";
        MyUtility::ConsumeTime<std::chrono::milliseconds> ct;

        std::list<std::thread> threads;
        for (size_t i = 0; i < numOfThreads; i++)
        {
            threads.emplace_back(std::thread(&Test::Set, &t, i));
        }
        for (auto&& t : threads)
        {
            t.join();
        }
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

/*
一个或多个读线程同时读取共享资源，且只有一个写线程来修改这个资源，这种情况下才能从shared_mutex获取性能优势
shared_lock是read lock。搭配std::shared_mutex使用，被锁后仍允许其他线程执行同样被shared_lock的代码。
lock_guard和unique_lock是write lock。被锁后不允许其他线程执行被shared_lock或unique_lock的代码。
https://www.jianshu.com/p/86abf93a05e7/
*/

/*
当有一个全局变量被多个线程高频调用读写时需要锁功能
1.当变量没有写入需求时可以支持多个线程同时读取(所有读取线程公用一个共享锁)
2.当有一个线程有写入需求时, 需要先等所有读取锁释放锁资源之后锁上互斥锁.此时不允许读取了, 需要等带写入的互斥锁解锁后才能读取
*/

/*
shared_lock是read lock。搭配std::shared_mutex使用，被锁后仍允许其他线程执行同样被shared_lock的代码。
lock_guard和unique_lock是write lock。被锁后不允许其他线程执行被shared_lock或unique_lock的代码。
*/

#include "../01_Thread/myUtility.hpp"
#include <iostream>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

#define READ_THREAD_COUNT 8
#define LOOP_COUNT 5000000

using ReadLock = std::shared_lock<std::shared_mutex>;
using WriteLock = std::lock_guard<std::shared_mutex>;
using NormalLock = std::lock_guard<std::mutex>;

// 使用读写锁
class shared_mutex_counter
{
public:
    shared_mutex_counter() = default;

    unsigned int get() const
    {
        ReadLock lock(mutex);
        return value;
    }

    void increment()
    {
        WriteLock lock(mutex);
        value++;
    }

private:
    mutable std::shared_mutex mutex;
    unsigned int value = 0;
};

// 使用普通锁
class mutex_counter
{
public:
    mutex_counter() = default;

    unsigned int get() const
    {
        NormalLock lock(mutex);
        return value;
    }

    void increment()
    {
        NormalLock lock(mutex);
        value++;
    }

private:
    mutable std::mutex mutex;
    unsigned int value = 0;
};

void test_shared_mutex()
{
    shared_mutex_counter counter;
    unsigned int temp;

    auto writer = [&counter]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++)
        {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++)
        {
            temp = counter.get();
        }
    };

    std::cout << "----- shared mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        MyUtility::ConsumeTime<std::chrono::milliseconds> ct;

        for (int i = 0; i < READ_THREAD_COUNT; i++)
        {
            threadlist.push_back(std::make_shared<std::thread>(reader));
        }
        std::shared_ptr<std::thread> pw = std::make_shared<std::thread>(writer);

        for (auto& it : threadlist)
        {
            it->join();
        }
        pw->join();
    }
    // 因为shared_lock允许多个线程同时进行【读操作】，所以temp的值不一定是LOOP_COUNT
    std::cout << "count:" << counter.get() << ", temp:" << temp << std::endl;
}

void test_mutex()
{
    mutex_counter counter;
    unsigned int temp;

    auto writer = [&counter]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++)
        {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++)
        {
            temp = counter.get();
        }
    };

    std::cout << "----- mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        MyUtility::ConsumeTime<std::chrono::milliseconds> ct;

        for (int i = 0; i < READ_THREAD_COUNT; i++)
        {
            threadlist.push_back(std::make_shared<std::thread>(reader));
        }

        std::shared_ptr<std::thread> pw = std::make_shared<std::thread>(writer);

        for (auto& it : threadlist)
        {
            it->join();
        }
        pw->join();
    }
    std::cout << "count:" << counter.get() << ", temp:" << temp << std::endl;
}

int main()
{
    test_shared_mutex();
    test_mutex();
    return 0;
}

#endif // TEST5

#ifdef TEST7

// 尝试获取锁的时间过长时就会触发超时, 超时的情况下不让线程继续阻塞

#include <chrono>
#include <iostream>
#include <mutex>

int i = 0;
std::timed_mutex m;

using namespace std::chrono_literals;

void test()
{
    for (;;)
    {
        if (!m.try_lock_for(std::chrono::milliseconds(100)))
        { // 超时时间为100ms
            std::cout << "try lock, but timeout\n";
            continue;
        }
        i = 10;
        std::cout << "lock success\n";
        std::this_thread::sleep_for(1s); // 阻塞一下测试超时锁效果
        m.unlock();
        std::this_thread::sleep_for(1ms);
    }
}
int main()
{
    for (int i = 0; i < 3; i++)
    {
        std::thread t1(test);
        t1.detach();
    }

    while (1)
        ;
}

#endif // TEST7
