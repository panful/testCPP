﻿/*
------------------------------------------------
01. join()阻塞演示
02. detach()示例
03. 线程调用成员函数
04. 给线程执行的函数传递引用类型的参数
------------------------------------------------
11. std::promise + std::future 获取线程执行的结果
12. std::packaged_task + std::future 获取线程执行结果
13. 使用lambda获取线程的执行结果
14. std::promise 在指定时间再设置线程任务执行需要的数据
------------------------------------------------
21. std::async 创建策略的区别，获取线程的执行结果
22. std::async 线程池机制，接收返回值
------------------------------------------------
31. std::condition_variable 条件变量同步机制

*/

#define TEST14

#ifdef TEST01

#include "myUtility.hpp"
#include <thread>
#include <vector>

void work()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main()
{
    {
        MyUtility::ConsumeTime ct;

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; i++)
        {
            // 在此处创建线程好线程之后，该线程就会开始异步执行
            threads.emplace_back(std::thread(work));
            // 不能将join()放在此处，因为join会阻塞到线程执行完任务那一刻（线程的任务如果已经执行完不会阻塞）
            // t[i].join();
        }

        // std::this_thread::sleep_for(std::chrono::seconds(6));

        for (auto&& t : threads)
        {
            // 当线程执行的任务还没执行完的时候，此处的join会阻塞住
            // work()函数需要消耗5秒的时间，调用join()与创建线程的时间间隔不小于5秒就不会阻塞
            t.join();
        }
    }

    return 0;
}

#endif // TEST01

#ifdef TEST02

#include "myUtility.hpp"
#include <thread>
#include <vector>

void work()
{
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << i << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    {
        MyUtility::ConsumeTime ct;

        std::thread t(work);

        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 使用detach()时，不会保证线程中的任务一定会执行完
        t.detach();
    }

    return 0;
}

#endif // TEST02

#ifdef TEST03

#include <iostream>
#include <thread>

class Test
{
public:
    void Print(int a) { std::cout << "Test: " << a << '\n'; }
};

class Test2
{
public:
    void Exector()
    {
        std::thread(&Test2::Print, this, 2).join();
    }

private:
    void Print(int a) { std::cout << "Test2: " << a << '\n'; }
};

class Test3
{
public:
    void operator()(int a)
    {
        std::cout << "Test3::operator(): " << a << '\n';
    }
};

int main()
{
    // 在类外部创建线程调用成员函数
    {
        Test test;
        // 第一个参数必须带&，第二个可带可不带（建议带上），第二个参数之后是调用函数的实参
        std::thread(&Test::Print, &test, 3).join();
    }

    // 在类内部创建线程调用成员函数
    {
        Test2 test;
        test.Exector();
    }

    // 调用operator()
    {
        Test3 test;
        // 第一个参数不需要带&
        std::thread(test, 3).join();
    }

    return 0;
}

#endif // TEST03

#ifdef TEST04

#include <iostream>
#include <thread>
#include <utility>

void Func(int n)
{
    std::cout << "Func(int n): " << n << '\n';
}

void Func2(int& n)
{
    std::cout << "Func2(int& n): " << n << '\n';
    n++;
}

void Func3(int&& n)
{
    std::cout << "Func3(int&& n): " << n << '\n';
}
class MyClass
{
public:
    void Func(int n)
    {
        std::cout << "MyClass::Func(int n): " << n << '\n';
    }
    void Func2(int& n)
    {
        std::cout << "MyClass::Func2(int& n): " << n << '\n';
    }
    void Func3(int&& n)
    {
        std::cout << "MyClass::Func3(int&& n): " << n << '\n';
    }
};

int main()
{
    {
        int n = 1;
        std::thread(&Func, n).join();
    }

    // 传递引用必须使用std::ref将参数包裹起来
    {
        int n = 2;
        std::thread(&Func2, std::ref(n)).join();
    }

    {
        int n = 3;
        std::thread(&Func3, n).join();
    }

    {
        MyClass c;
        int n = 2;
        std::thread(&MyClass::Func, &c, 1).join();
        std::thread(&MyClass::Func2, &c, std::ref(n)).join();
        std::thread(&MyClass::Func3, &c, 3).join();
    }

    // lambda
    {
        int a = 2;
        std::thread([a]() { std::cout << "lambda: " << a << '\n'; }).join();
    }

    return 0;
}
#endif // TEST04

//-------------------------------

#ifdef TEST11

#include "myUtility.hpp"
#include <atomic>
#include <future> //std::future std::promise
#include <mutex>
#include <thread>
#include <utility> //std::ref模板传参的时候使用

void Add(int x, int y, std::promise<int>& promiseObj)
{
    std::cout << "thread id: " << std::this_thread::get_id() << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(3));
    auto value = x + y;
    // 将一个int类型的值赋值给std::promise<int>
    promiseObj.set_value(value);
}

// std::promise 作用就是用来包装一个值，将数据和 std::future 绑定起来

int main()
{
    std::cout << "main thread id: " << std::this_thread::get_id() << '\n';

    {
        MyUtility::ConsumeTime ct;

        // 一个子线程计算a+b，另一个子线程计算x+y
        // 主线程计算 (a+b)/(x+y)
        int a { 4 }, b { 5 }, x { 6 }, y { 7 };

        // 声明一个std::promise<int> 它可以存储int类型的值
        std::promise<int> promiseObj;
        // 将std::future和std::promise关联
        std::future<int> futureObj = promiseObj.get_future();
        // 创建一个线程，模板传参的时候使用ref，否则传参失败
        std::thread(Add, a, b, std::ref(promiseObj)).detach();

        //---------------------------------------------------------------------------
        // 不能复用上面的future和promise
        std::promise<int> promiseObj2;
        std::future<int> futureObj2 = promiseObj2.get_future();
        std::thread(Add, x, y, ref(promiseObj2)).detach();

        ct.ConsumeTimeByNow();

        // 获取值，此处可能会阻塞住（如果已经获取到函数执行结果就不会阻塞），直到线程执行完获取到值
        auto sum1 = futureObj.get();
        auto sum2 = futureObj2.get();

        std::cout << "4 + 5 = " << sum1 << '\n'
                  << "6 + 7 = " << sum2 << '\n'
                  << "(4 + 5) / (6 + 7) = " << sum1 / sum2 << '\n';
    }
    return 0;
}
#endif // TEST11

#ifdef TEST12

#include "myUtility.hpp"
#include <future>  // std::packaged_task, std::future
#include <thread>  // std::thread
#include <utility> // std::move

int Add(int a, int b)
{
    std::cout << "thread id: " << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return a + b;
}

// std::packaged_task 用来包装一个调用对象，将函数和 std::future 绑定起来，方便异步调用

int main()
{
    std::cout << "main thread id: " << std::this_thread::get_id() << '\n';

    {
        MyUtility::ConsumeTime ct;

        // std::packaged_task的模板参数列表是一个函数类型
        std::packaged_task<decltype(Add)> pt(Add);
        // std::packaged_task<int(int, int)> pt(Add); // 等价于上面的语句

        // 将std::packaged_task包装的调用对象的返回值保存在std::future中
        std::future<int> result = pt.get_future();

        //  std::packaged_task没有拷贝构造函数，所以std::thread第一个参数应该对std::packaged_task使用std::move
        std::thread(std::move(pt), 2, 3).detach();
        ct.ConsumeTimeByNow();

        std::cout << result.get() << '\n';
    }

    return 0;
}

#endif // TEST12

#ifdef TEST13

#include <chrono>
#include <iostream>
#include <thread>

int Add(int a, int b)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
    return a + b;
}

int main()
{
    int result { 0 };

    std::thread t([&] { result = Add(2, 3); });
    t.join(); // 不要使用detach，不然使用result的时候可能仍为0
    std::cout << result << '\n';

    return 0;
}
#endif // TEST13

#ifdef TEST14

#include <future>
#include <iostream>
#include <thread>

void get_task_promise(std::promise<int>& n)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread id: " << std::this_thread::get_id() << '\t' << n.get_future().get() << '\n';
}

void get_task_future(std::future<int>& n)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "thread id: " << std::this_thread::get_id() << '\t' << n.get() << '\n';
}

void set_task_promise(std::promise<int>& n)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int value = 8;
    n.set_value(value);
    std::cout << "thread id: " << std::this_thread::get_id() << '\t' << value << '\n';
}

int main()
{
    {
        std::promise<int> p;
        std::thread t(get_task_promise, std::ref(p));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // 在指定时间给线程设置参数
        p.set_value(5);
        t.join();
    }

    {
        std::promise<int> p;
        auto f = p.get_future();
        std::thread t(get_task_future, std::ref(f));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // 在指定时间给线程设置参数
        p.set_value(4);
        t.join();
    }

    {
        std::promise<int> p;
        std::thread(set_task_promise, std::ref(p)).detach();
        // 获取线程执行结果
        auto get_value = p.get_future().get();
        std::cout << "get value: " << get_value << std::endl;
    }

    return 0;
}
#endif // TEST14

//-------------------------------

#ifdef TEST21

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int Add(int a, int b)
{
    std::cout << "thread id:" << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return a + b;
}

// 第一个参数是创建策略：
// std::launch::async 表示任务执行在另一线程；
// std::launch::deferred 表示延迟执行任务，调用 get 或者 wait 时才会执行，不会创建线程，惰性执行在【当前线程】。
// 如果不明确指定创建策略，以上两个都不是 async 的默认策略，而是未定义，它是一个基于任务的程序设计，内部有一个【调度器(线程池)】，会根据实际情况决定采用哪种策略。
// async(std::launch::async | std::launch::deferred, func, args...);

int main()
{
    std::cout << "main thread id: " << std::this_thread::get_id() << '\n';

    {
        // 显式指定异步执行任务
        std::future<int> result = std::async(std::launch::async, Add, 2, 3);
        // 调用get()会阻塞，直到线程执行完任务
        std::cout << "std::launch::async\t" << result.get() << '\n';
    }

    {
        // 指定任务延迟执行，任务会在当前线程（主线程）执行
        std::future<int> result = std::async(std::launch::deferred, Add, 3, 4);
        std::cout << "std::launch::deferred\t" << result.get() << '\n';
    }

    {
        // 不指定策略，会在内部的线程池中使用一个线程执行任务
        std::future<int> result = std::async(Add, 6, 7);
        std::cout << "unspecified\t\t" << result.get() << '\n';
    }

    return 0;
}

#endif // TEST21

#ifdef TEST22

#include "myUtility.hpp"
#include <future>
#include <thread>

void MySleep(int s)
{
    std::cout << "-- thread id: " << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(s));
}

// 若从 std::async 获得的 std::future 未被移动或绑定到引用，则在完整表达式结尾std::future的析构函数将阻塞直至异步计算完成，
// 实际上相当于同步操作，所以使用std::async最好将返回值保存起来
// 下面的代码，前三个std::async打印的线程id是同一个，恰好证明了std::async内部有一个线程池（不一定是线程池，但是类似线程池）

int main()
{
    {
        MyUtility::ConsumeTime ut;

        std::async(std::launch::async, MySleep, 5); // 返回值没有被接收，临时对象被析构，阻塞 5s
        ut.ConsumeTimeByNow();

        std::async(std::launch::async, MySleep, 5); // 再阻塞 5s
        ut.ConsumeTimeByNow();

        auto f1 = std::async(std::launch::async, MySleep, 5); // 返回值被接收，所以不会析构，这句代码也就不会阻塞，返回值的作用域结束时才会析构f1，那个时候才会阻塞
        ut.ConsumeTimeByNow();

        auto f2 = std::async(std::launch::async, MySleep, 5); // f1,f2,f3并行析构，这三句代码析构共总只需要5s
        ut.ConsumeTimeByNow();

        auto f3 = std::async(std::launch::async, MySleep, 5);
        ut.ConsumeTimeByNow();
    }

    return 0;
}

#endif // TEST22

//-------------------------------

#ifdef TEST31

#include <iostream>
#include <list>
#include <mutex>
#include <thread>

class Test
{
public:
    Test() = default;
    ~Test() = default;

    void Eexcutor()
    {
        std::thread(&Test::PrintElement, this).detach();
    }

    void AddElement(int e)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "add element: " << e << '\n';
        m_list.emplace_back(e);
        m_cv.notify_all();
    }

private:
    void PrintElement()
    {
#if (1)
        while (true)
        {
            // 只有当std::condition_variable收到通知时才会打印
            std::cout << "--- while ---\n";

            std::unique_lock<std::mutex> lock(m_mutex);
            // 当lambda表达式返回true时会一直阻塞，阻塞并不会占用cpu
            // 当调用std::condition_variable::notify_all或notify_one时，会执行一次lambda，
            // 如果返回false停止阻塞执行后面代码，返回true则继续阻塞
            m_cv.wait(lock, [this]() { std::cout << "wait\n"; return !m_list.empty(); });

            auto element = m_list.front();
            m_list.pop_front();
            std::cout << "pop front: " << element << '\n';
        }
#elif
        while (true)
        {
            // 程序结束之前会一直打印，即一直占用cpu
            std::cout << "--- while ---\n";

            if (!m_list.empty())
            {
                auto element = m_list.front();
                m_list.pop_front();
                std::cout << element << '\n';
            }
        }
#endif
    }

private:
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::list<int> m_list;
};

int main()
{
    Test t;
    t.Eexcutor();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.AddElement(1);
    t.AddElement(2);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.AddElement(3);
    t.AddElement(4);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.AddElement(5);
    t.AddElement(6);

    // 保证将插入的元素全部pop完成
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}

#endif // TEST31
