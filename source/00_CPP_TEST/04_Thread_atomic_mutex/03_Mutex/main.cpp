/*
* 01 std::mutex 互斥锁
* 02 std::shared_lock 共享锁
* 03 std::timed_mutex 超时锁
* 04 std::recursive_mutex 递归锁
*/



#ifdef TEST1

int main()
{
}

#endif // TEST1

#ifdef TEST2

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

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <list>
#include <iostream>
#include <vector>

#define READ_THREAD_COUNT 8  
#define LOOP_COUNT 5000000  

typedef std::shared_lock<std::shared_mutex> ReadLock;
typedef std::lock_guard<std::shared_mutex> WriteLock;

typedef std::lock_guard<std::mutex> NormalLock;

class shared_mutex_counter {
public:
    shared_mutex_counter() = default;

    unsigned int get() const {
        ReadLock lock(mutex);
        return value;
    }

    void increment() {
        WriteLock lock(mutex);
        value++;
    }

private:
    mutable std::shared_mutex mutex;
    unsigned int value = 0;
};

class mutex_counter {
public:
    mutex_counter() = default;

    unsigned int get() const {
        NormalLock lock(mutex);
        return value;
    }

    void increment() {
        NormalLock lock(mutex);
        value++;
    }

private:
    mutable std::mutex mutex;
    unsigned int value = 0;
};

class timers
{
public:
    timers()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    ~timers()
    {
        m_end = std::chrono::high_resolution_clock::now();
        Consuming();
    }

    void Consuming()
    {
        std::cout << "Time-consuming:" << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_end - m_begin).count() << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point m_begin;
    std::chrono::high_resolution_clock::time_point m_end;
};


void test_shared_mutex()
{
    shared_mutex_counter counter;
    unsigned int temp;

    auto writer = [&counter]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    std::cout << "----- shared mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        timers timer;

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
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    std::cout << "----- mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        timers timer;

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

#endif // TEST2

#ifdef TEST3

// 尝试获取锁的时间过长时就会触发超时, 超时的情况下不让线程继续阻塞

#include <mutex>
#include <iostream>
#include <chrono>

int i = 0;
std::timed_mutex m;

using namespace std::chrono_literals;

void test() {
    for (;;)
    {
        if (!m.try_lock_for(std::chrono::milliseconds(100))) {//超时时间为100ms
            std::cout << "尝试上锁 已超过100ms 仍未成功 触发超时" << std::endl;
            continue;
        }
        i = 10;
        std::cout << "成功上锁" << std::endl;
        std::this_thread::sleep_for(1s);//阻塞一下测试超时锁效果
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

    while (1);
}

#endif // TEST3

#ifdef TEST4

// 同线程同一把锁可以上锁多次, 避免死锁
// lock()一次锁计数器加一, unlock()一次锁计数器减一
// 当锁计数器归零时才会真正解锁

#include <iostream>
#include <mutex>

using namespace std;

recursive_mutex rm;

void test()
{
    rm.lock();
    cout << "Do something" << endl;
    rm.unlock();
}

int main()
{
    for (int i = 0; i < 3; i++)
    {
        rm.lock();
        test();
        rm.unlock();//锁几次就要解锁几次
        this_thread::sleep_for(1ms);
    }
    
}
#endif // TEST4
