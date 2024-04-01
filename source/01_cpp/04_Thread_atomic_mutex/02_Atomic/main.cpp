/*
 * 1. 多线程累加使用锁和原子操作对比
 * 2. exchange()的使用
 * 3.
 */

#define TEST1

#ifdef TEST1

#include "../00_Timer/myUtility.hpp"
#include <array>
#include <atomic>
#include <mutex>
#include <thread>

static constexpr size_t numOfThreads { 10 };
static constexpr size_t numOfAccumulation { 10000000 };

using CT = MyUtility::ConsumeTime<std::chrono::milliseconds>;

class NoLock
{
public:
    NoLock()
    {
        for (size_t i = 0; i < numOfThreads; i++)
        {
            m_threads[i] = std::thread(&NoLock::Accumulation, this);
        }
    }

    ~NoLock()
    {
        for (auto&& t : m_threads)
        {
            t.join();
        }

        std::cout << "-----------------------------\nNoLock: " << m_result << '\n';
    }

private:
    void Accumulation()
    {
        for (size_t i = 0; i < numOfAccumulation; i++)
        {
            m_result += 1;
        }
    }

private:
    std::array<std::thread, numOfThreads> m_threads;
    int m_result { 0 };
    CT m_ct;
};

class Atomic
{
public:
    Atomic()
    {
        for (size_t i = 0; i < numOfThreads; i++)
        {
            m_threads[i] = std::thread(&Atomic::Accumulation, this);
        }
    }

    ~Atomic()
    {
        for (auto&& t : m_threads)
        {
            t.join();
        }

        std::cout << "-----------------------------\nAtomic: " << m_result << '\n';
    }

private:
    void Accumulation()
    {
        for (size_t i = 0; i < numOfAccumulation; i++)
        {
            m_result += 1;
        }
    }

private:
    std::array<std::thread, numOfThreads> m_threads;
    std::atomic<int> m_result { 0 };
    CT m_ct;
};

class Mutex
{
public:
    Mutex()
    {
        for (size_t i = 0; i < numOfThreads; i++)
        {
            m_threads[i] = std::thread(&Mutex::Accumulation, this);
        }
    }

    ~Mutex()
    {
        for (auto&& t : m_threads)
        {
            t.join();
        }

        std::cout << "-----------------------------\nMutex: " << m_result << '\n';
    }

private:
    void Accumulation()
    {
        for (size_t i = 0; i < numOfAccumulation; i++)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_result += 1;
        }
    }

private:
    std::array<std::thread, numOfThreads> m_threads;
    std::mutex m_mutex;
    int m_result { 0 };
    CT m_ct;
};

// 使用10个线程从0累加numOfAccumulation次
// NoLock对共享数据不做任何保护，耗时最短，但是结果错误
// Atomic对共享数据使用原子操作，耗时较少，结果正确
// Mutex对共享数据加锁保护，耗时最多，结果正确

int main()
{
    {
        NoLock n;
    }
    {
        Atomic a;
    }
    {
        Mutex m;
    }

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::atomic<bool> ready(false);
std::atomic<bool> winner(false);

void count1m(int n)
{
    // wait for ready
    while (!ready)
    {
    }

    // count to 1M
    for (int i = 0; i < 1000000; i++)
    {
    }

    // 只有第一个执行exchange的线程会返false,并输出下面的语句,其余线程都返回true无法进入if
    if (!winner.exchange(true))
    {
        std::cout << "thread #" << n << " won!\n";
    }
}

int main()
{
    std::atomic_int num;
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++)
    {
        threads.push_back(std::thread(count1m, i + 1)); // 创建10个线程
    }

    ready.store(true); // 开始

    for (auto& th : threads)
    {
        th.join();
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::atomic<int> cnt = { 0 };

void f()
{
    for (int n = 0; n < 1000; ++n)
    {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}

int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(f);
    }
    for (auto& t : v)
    {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n';
}

#endif // TEST3

#ifdef TEST17

#include <atomic>
#include <iostream>

int main()
{
    std::atomic_flag aa;
    std::atomic_int k;

    std::cout << k.load() << '\n';

#if __cplusplus >= 202002L
    std::cout << aa.test() << '\n';
#endif

    k.store(1);
    aa.test_and_set();

    std::cout << k.load() << '\n';

#if __cplusplus >= 202002L
    std::cout << aa.test() << '\n';
#endif

    return 0;
}

#endif // TEST17
