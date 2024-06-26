/*
------------------------------------------------
01. join()阻塞演示
02. detach()示例
03. 线程调用成员函数
04. 给线程执行的函数传递引用类型的参数
05. std::thread执行的任务如果抛出异常无法捕获
------------------------------------------------
11. std::promise + std::future 获取线程执行的结果
12. std::packaged_task + std::future 获取线程执行结果
13. 使用lambda获取线程的执行结果
14. std::promise 在指定时间再设置线程任务执行需要的数据
15. std::future std::shared_future常用函数的使用
16. std::promise.set_value_at_thread_exit 线程退出时才将状态设置为就绪
17. std::packaged_task 常用函数的使用
18. std::promise 实现定时器，std::promise<void> 用来实现一次性事件通信
------------------------------------------------
21. std::async 创建策略的区别，获取线程的执行结果
22. std::async 线程池机制，接收返回值
23. 捕获std::async执行的任务抛出的异常
24. 默认以异步方式执行任务
25. 对std::async的返回值（期指）使用wait_for时的坑
------------------------------------------------
31. std::condition_variable 条件变量同步机制 虚假唤醒
32. 解决虚假唤醒

*/

#define TEST32

#ifdef TEST01

#include "../00_Timer/myUtility.hpp"
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

#include "../00_Timer/myUtility.hpp"
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
    void Print(int a)
    {
        std::cout << "Test: " << a << '\n';
    }
};

class Test2
{
public:
    void Exector()
    {
        std::thread(&Test2::Print, this, 2).join();
    }

private:
    void Print(int a)
    {
        std::cout << "Test2: " << a << '\n';
    }
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

#ifdef TEST05

#include <iostream>
#include <thread>

void worker()
{
    std::cout << "worker start\n";
    throw std::runtime_error("error");
    std::cout << "worker end\n";
}

int main()
{
    {
        try
        {
            std::thread t(worker);
            t.join();
        }
        catch (...)
        {
            // 任务抛出异常不会被捕获，直接调用std::terminate终止程序
            std::cout << "catch error\n";
        }
    }
}

#endif // TEST05

//-------------------------------
// std::future 用于访问异步操作的结果，而 std::promise 和 std::packaged_task 在 future 高一层，它们内部都有一个 future，
// promise 包装的是一个值，packaged_task 包装的是一个函数。
// 当需要获取线程中的某个值，可以使用std::promise，当需要获取线程函数返回值，可以使用std::packaged_task。

#ifdef TEST11

#include "../00_Timer/myUtility.hpp"
#include <atomic>
#include <future>  //std::future std::promise
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

        std::cout << "4 + 5 = " << sum1 << '\n' << "6 + 7 = " << sum2 << '\n' << "(4 + 5) / (6 + 7) = " << sum1 / sum2 << '\n';
    }
    return 0;
}
#endif // TEST11

#ifdef TEST12

#include "../00_Timer/myUtility.hpp"
#include <future>  // std::packaged_task, std::future
#include <thread>  // std::thread
#include <utility> // std::move

int Add(int a, int b)
{
    std::cout << "thread id: " << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return a + b;
}

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

#ifdef TEST15

#include "../00_Timer/myUtility.hpp"
#include <chrono>
#include <future>
#include <thread>
#include <vector>

int main()
{
    // std::future.wait()
    {
        MyUtility::ConsumeTime ct;
        std::future<int> result_future = std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return 9;
        });
        ct.ConsumeTimeByNow();

        result_future.wait();
        ct.ConsumeTimeByNow();

        auto result_number = result_future.get();
        ct.ConsumeTimeByNow();

        std::cout << result_number << '\n';
    }

    std::cout << "------------------------------------------\n";

    // std::future.wait_for()
    // 标准库关于 std::future.wait_for()返回值定义（共享状态可以理解为一个共享的区域）
    // enum class future_status {
    //     ready,   // 共享状态就绪，即std::future中的数据已经可以使用
    //     timeout, // 共享状态在经过指定的等待时间内仍未就绪
    //     deferred // 共享状态持有的函数正在延迟运行，结果将仅在显式请求时计算 具体可以查看std::async(std::launch::deferred,...)
    // };
    {
        MyUtility::ConsumeTime ct;
        std::future<int> result_future = std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return 9;
        });
        ct.ConsumeTimeByNow();

        auto wait_status = result_future.wait_for(std::chrono::seconds(2));
        ct.ConsumeTimeByNow();

        auto result_number = result_future.get();
        ct.ConsumeTimeByNow();

        std::cout << "future status: " << static_cast<int>(wait_status) << "\tresult: " << result_number << '\n';
    }

    std::cout << "------------------------------------------\n";

    // std::future.valid()返回true表示当前的共享状态可用（共享的区域存在），即可以使用get() wait()等函数
    {
        std::cout << std::boolalpha;

        std::promise<void> p;

        std::future<void> f;
        std::cout << f.valid() << '\n';

        // 将std::promise返回值赋值给std::future将变为共享状态，
        // 否则std::future处于非共享状态，调用get() wait()等函数时将会抛出异常
        f = p.get_future();
        std::cout << f.valid() << '\n';

        // 让状态处于就绪状态，即使用wait()不会继续阻塞，可以使用get()获取值，
        // 就好像发送了一个准备就绪的信号
        p.set_value();
        std::cout << f.valid() << '\n';

        // 调用get()后会释放共享状态，即valid()返回false
        f.get();
        std::cout << f.valid() << '\n';
    }

    std::cout << "------------------------------------------\n";

    // std::shared_future
    {
        std::vector<std::shared_future<int>> futures;

        std::promise<int> p1;
        std::promise<int> p2;
        auto f1 = p1.get_future();
        auto f2 = p2.get_future();

        // 将std::future保存到容器需要使用std::future.share()
        // 在std::future上调用share()后，共享状态将变为false
        // 可以理解为：以std::shared_future<T>(std:move(std::future))构造
        futures.emplace_back(f1.share());
        futures.emplace_back(f2.share());

        p1.set_value(1);
        p2.set_value(2);

        std::cout << f1.valid() << '\t' << f2.valid() << '\n'
                  << futures.at(0).valid() << '\t' << futures.at(1).valid() << '\n'
                  << futures.at(0).get() << '\t' << futures.at(1).get() << '\n'
                  << futures.at(0).valid() << '\t' << futures.at(1).valid() << '\n';
    }

    return 0;
}

#endif // TEST15

#ifdef TEST16

#include "../00_Timer/myUtility.hpp"
#include <future>
#include <thread>

int main()
{
    {
        std::promise<int> p;
        std::future<int> f = p.get_future();

        p.set_value(1);
        std::cout << f.get() << '\n';

        // std::promise不能多次使用set_value()否则会崩溃
        try
        {
            p.set_value(2);
            std::cout << f.get() << '\n';
        }
        catch (...)
        {
            std::cout << "wrong of set_value()\n";
        }
    }

    {
        MyUtility::ConsumeTime ct;
        using namespace std::chrono_literals;
        std::promise<int> p;
        std::future<int> f = p.get_future();

        std::thread([&p] {
            std::this_thread::sleep_for(2s);

            // 在当前线程退出的时候再将状态设置为就绪
            p.set_value_at_thread_exit(9);

            // 执行完set_value()就会将状态设置为就绪
            // p.set_value(9);
            std::this_thread::sleep_for(2s);
        }).detach();

        std::cout << "Waiting...\n" << std::flush;

        // set_value_at_thread_exit需要4s
        // set_value只需2秒
        f.wait();
        ct.ConsumeTimeByNow();

        std::cout << "Done!\nResult is: " << f.get() << '\n';
    }
}

#endif // TEST16

#ifdef TEST17

#include "../00_Timer/myUtility.hpp"
#include <future>
#include <thread>

int main()
{
    std::cout << std::boolalpha;
    std::cout << "thread id: " << std::this_thread::get_id() << '\n';
    auto worker = [](int n) {
        std::cout << "thread id: " << std::this_thread::get_id() << '\n';
        return 2 * n;
    };

    // valid用于检查std::packaged_task对象是否拥有合法函数
    // 只有当valid返回ture时，才能使用operator()等执行任务的函数
    {
        std::packaged_task<int(int)> pt;
        std::cout << "valid: " << pt.valid() << '\n';
    }

    std::cout << "-------------------\n";

    {
        std::packaged_task<int(int)> pt(worker);
        std::cout << "valid: " << pt.valid() << '\n';
        std::future<int> f = pt.get_future();
        std::thread(std::move(pt), 2).join();
        std::cout << f.get() << '\n';
    }

    std::cout << "-------------------\n";

    // operator()用来在当前线程执行任务
    {
        std::packaged_task<int(int)> pt(worker);
        std::future<int> f = pt.get_future();
        pt(3);
        std::cout << f.get() << '\n';
    }

    std::cout << "-------------------\n";

    // reset()重置状态，抛弃先前执行的结果。构造共享状态。
    // 等价于 std::packaged_task pt = std::packaged_task(std::move(f)) ，其中 f 是存储的任务。
    {
        std::packaged_task<int(int)> pt(worker);
        std::future<int> f = pt.get_future();

        pt(3);
        std::cout << f.get() << '\n';

        std::cout << "valid: " << pt.valid() << '\n';
        pt.reset();
        std::cout << "valid: " << pt.valid() << '\n';

        // 如果没有重置状态不能多次执行任务
        pt(4);
        std::cout << pt.get_future().get() << '\n';
    }

    std::cout << "-------------------\n";

    // std::packaged_task.make_ready_at_thread_exit()立即执行任务，但是线程退出时才将状态设置为就绪
    // MSVC一直抛出异常，不知道啥原因
    // https://zh.cppreference.com/w/cpp/thread/packaged_task

    return 0;
}

#endif // TEST17

#ifdef TEST18

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void f1()
{
    std::cout << std::this_thread::get_id() << "\tthis is f1()\n";
}

void f2(std::future<void> futureObj, std::function<void()> f)
{
    // 定时器，每隔1s执行一次f()
    while (futureObj.wait_for(std::chrono::seconds(1)) == std::future_status::timeout)
    {
        f();
    }
}

int main()
{
    std::cout << std::this_thread::get_id() << '\n';

    std::promise<void> promiseObj;
    std::future<void> futureObj = promiseObj.get_future();

    std::thread t(f2, std::move(futureObj), f1);

    // 不阻塞主线程，可以做一些其他事情
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 给线程t发送信号退出while循环，即停止定时器
    promiseObj.set_value();
    t.join();

    return 0;
}
#endif // TEST18

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
// 如果不明确指定创建策略，以上两个都不是 async的默认策略，而是未定义，
// 它是一个基于任务的程序设计，内部有一个【调度器(线程池)】，会根据实际情况决定采用哪种策略。 async(std::launch::async |
// std::launch::deferred, func, args...);

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

#include "../00_Timer/myUtility.hpp"
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

        auto f1 = std::async(
            std::launch::async, MySleep, 5); // 返回值被接收，所以不会析构，这句代码也就不会阻塞，返回值的作用域结束时才会析构f1，那个时候才会阻塞
        ut.ConsumeTimeByNow();

        auto f2 = std::async(std::launch::async, MySleep, 5); // f1,f2,f3并行析构，这三句代码析构共总只需要5s
        ut.ConsumeTimeByNow();

        auto f3 = std::async(std::launch::async, MySleep, 5);
        ut.ConsumeTimeByNow();
    }

    return 0;
}

#endif // TEST22

#ifdef TEST23

#include <future>
#include <iostream>
#include <thread>

void worker()
{
    std::cout << "thread id: " << std::this_thread::get_id() << "\nworker start\n";
    throw std::runtime_error("error");
    std::cout << "worker end\n";
}

int main()
{
    {
        auto fut = std::async(worker);
        // auto fut = std::async(std::launch::async, worker);
        // auto fut = std::async(std::launch::deferred, worker);
        try
        {
            fut.get();
            // 如果任务抛出异常，get()之后的代码不会执行
            std::cout << "end get()\n";
        }
        catch (...)
        {
            // 任务如果抛出异常，在调用get()的线程中可以捕获到该异常
            std::cout << "thread id: " << std::this_thread::get_id() << '\n';
            std::cout << "catch error\n";
        }
    }
}

#endif // TEST23

#ifdef TEST24

#include <future>
#include <iostream>
#include <thread>

template <typename T, typename... Args>
typename std::future<std::result_of_t<T(Args...)>> reallyAsyc(T&& t, Args&&... args)
{
    return std::async(std::launch::async, std::forward<T>(t), std::forward<Args>(args)...);
}

void worker(int a, int b)
{
    std::cout << "thread id:" << std::this_thread::get_id() << "\targs: " << a << ", " << b << '\n';
}

int main()
{
    std::cout << "thread id:" << std::this_thread::get_id() << '\n';
    auto f1 = reallyAsyc(worker, 1, 2);
    auto f2 = reallyAsyc(worker, 3, 4);
    auto f3 = reallyAsyc(worker, 5, 6);
}

#endif // TEST24

#ifdef TEST25

#include <future>
#include <iostream>
#include <thread>

void worker()
{
    // 睡眠1s
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
    std::cout << "start\n";
    // auto fut = std::async(worker);
    auto fut = std::async(std::launch::deferred, worker);

    while (fut.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready)
    {
        std::cout << "--- while ---\n";
    }

    // while之后的代码可能永远都不会执行到，因为std::async的默认执行方式可以是延迟的std::deferred也可以是std::async
    // 当执行方式是std::deferred时，fut.wait_for将永远返回std::deferred，因此会死循环
    std::cout << "end\n";
}

#endif // TEST25

//-------------------------------

#ifdef TEST31

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

struct Test
{
    std::mutex mutex {};
    std::condition_variable cv {};

    void worker()
    {
        while (true)
        {
            std::unique_lock lk(mutex);

            // 此处 wait_for 阻塞时并不会占用 CPU
            if (std::cv_status::timeout == cv.wait_for(lk, std::chrono::seconds(1)))
            {
                std::cout << "time out\n";
                continue;
            }

            // 被其他线程唤醒（notify）或发生虚假唤醒，都返回 std::cv_status::no_timeout
            std::cout << "do something\n";
        }
    }

    void notify()
    {
        cv.notify_all();
    }
};

// 虚假唤醒：
// 1. 由notify_all唤醒之后却得不到需要的数据，notify_all会唤醒所有线程，但是只能有一个线程使用数据
// 2. 有的系统会出于某种原因唤醒正在阻塞队列的线程，这时候消费者线程也是得不到需要的数据的(因为不是由生产者线程唤醒)。
// 解决虚假唤醒的方法：提前while判断队列是否为空，为空则继续等待，建议使用wait的lambda判断

// 对于第二种情况
// Windows(MSVC) 会发生虚假唤醒（没有调用 notify 但是每过一段时间就会返回 std::cv_status::no_timeout
// Windows(MinGW) 会发生
// Linux 可能不会发生这种虚假唤醒，实测：打印300多次 time out，也只打印了一次 do something

int main()
{
    Test test {};

    std::thread t(&Test::worker, &test);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    test.notify();

    t.join();
}

#endif // TEST31

#ifdef TEST32

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

struct Test
{
    std::mutex mutex {};
    std::condition_variable cv {};
    std::atomic_bool flag { false };

    void worker()
    {
        while (true)
        {
            std::unique_lock lk(mutex);

            // 当lambda表达式返回false时会一直阻塞
            // 当调用std::condition_variable::notify_all或notify_one时，会执行一次lambda，
            // 如果lambda返回true则停止阻塞执行后面代码，返回false则继续阻塞
            if (!cv.wait_for(lk, std::chrono::seconds(1), [this]() { return static_cast<bool>(this->flag); }))
            {
                std::cout << "time out\n";
                continue;
            }

            // 带谓词的 wait_for 相当于：超时后再执行谓词，并返回谓词的结果

            // 被其他线程唤醒（notify）或发生虚假唤醒，都返回 std::cv_status::no_timeout
            std::cout << "do something\n";
            flag = false;
        }
    }

    void notify()
    {
        flag = true;
        cv.notify_all();
    }
};

int main()
{
    Test test {};

    std::thread t(&Test::worker, &test);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    test.notify();

    t.join();
}

#endif // TEST32
