/*
 * 
 * 2. exchange()的使用
 */

#define TEST2

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