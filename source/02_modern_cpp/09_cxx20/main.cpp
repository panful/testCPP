/**
 * 101. std::format 的使用
 * 201. std::ranges std::views
 * 301. 协程 coroutine
 * 401. 概念 concepts https://zhuanlan.zhihu.com/p/600617910?utm_id=0
 * 501. std::span
 * 601. std::jthread 无需结合，可以优雅地停止线程 std::osyncstream
 * 602. std::barrier arrive_and_wait()
 * 603. std::barrier arrive_and_drop()
 * 604. std::barrier 构造时设置回调函数
 * 605. std::latch count_down()
 * 606. std::latch arrive_and_wait()
 * 607. std::counting_semaphore std::binary_semaphore
 */

#define TEST607

#ifdef TEST101

#include <format>
#include <iostream>

int main()
{
    {
        // 返回值是 std::string 或 std::wstring
        // {} 表示后面的可变参数
        // {0:} ':'之前的数字表示可变参数的索引
        auto str1 = std::format("test {}\n", "std::format");
        auto str2 = std::format("test {1:} {0:}\n", "world", "hello");
        std::cout << str1 << str2;
    }

    {
        auto i = 1;
        auto f = 2.222f;
        auto d = 3.333;
        auto b = false;

        // 占3位右对齐
        // 小数点后保留5位
        // 科学计数法
        // false、true替代bool类型
        // #显式2、8、16进制的前缀0b 0B 0 0x 0X
        // #b #B #o #O #d #D #x #X 二、八、十、十六进制格式化整数
        auto str = std::format("int: {:>3d}\tfloat: {:.5f}\tdouble: {:e}\tbool: {:s}\tnumber: {:#B}", i, f, d, b, 255);
        std::cout << str << std::endl;
    }

    {
        int i = 1;
        // 格式化指针
        auto str1 = std::format("pointer: {:#X}\n", reinterpret_cast<std::uintptr_t>(&i));
        auto str2 = std::format("pointer: {:#x}\n", unsigned long long(&i));
        std::cout << str1 << str2;
    }

    // 转义字符
    {
        std::cout << std::format("abc {{}} 123\n");
        std::cout << std::format("abc {{\n\txyz\t123\n}}\nabc\n");
    }
}

#endif // TEST101

#ifdef TEST201

#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

// std::ranges std::views https://blog.csdn.net/JohnnyMartin/article/details/118639528

int main()
{
    {
        std::vector vec = { 1, 3, 5, 7, 9, 2, 4, 6, 8, 0 };
        std::ranges::sort(vec);

        for (const auto& elem : vec)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }

    // 使用管道 '|'
    {
        std::vector vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // filter 提取偶数
        // transform 将提取到的偶数乘2
        // 结果res是惰性求值，只有在用到的时候才会实际执行views操作
        auto res = vec | std::views::filter([](auto v) { return v % 2 == 0; }) | std::views::transform([](auto v) { return v * 2; });

        for (const auto& elem : res)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }

    {
        std::vector numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto even           = [](int i) { return 0 == i % 2; };
        auto square         = [](int i) { return i * i; };

        // 从最里边的{}开始计算，先找出偶数，再对所有偶数平方
        // std::ranges::transform_view 是一个类：class
        auto res1 {
            std::ranges::transform_view {std::ranges::filter_view { numbers, even }, square}
        };
        for (const auto& elem : res1)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;

        // std::views::transform 是一个函数对象：struct 重载了 operator()
        auto res2 = std::views::transform(std::views::filter(numbers, even), square);
        for (const auto& elem : res2)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;

        // std::ranges::transform 是一个函数
        // https://zh.cppreference.com/w/cpp/algorithm/ranges/transform
        std::vector<int> res3;
        std::ranges::transform(numbers, std::back_inserter(res3), even);
    }
}

#endif // TEST201

#ifdef TEST301

#include <chrono>
#include <coroutine>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

struct Result
{
    struct promise_type
    {
        Result get_return_object()
        {
            return {};
        }

        std::suspend_never initial_suspend()
        {
            return {};
        }

        std::suspend_never final_suspend() noexcept
        {
            return {};
        }

        void return_void()
        {
        }

        void unhandled_exception()
        {
        }
    };
};

std::coroutine_handle<> coroutine_handle;

struct AWaitableObject
{
    AWaitableObject()
    {
    }

    bool await_ready() const
    {
        return false;
    }

    int await_resume()
    {
        return 0;
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
        coroutine_handle = handle;
    }
};

Result CoroutineFunction()
{
    std::cout << "start coroutine\n";
    int ret = co_await AWaitableObject();
    std::cout << "finish coroutine\n";
}

int main()
{
    std::cout << "start \n";
    auto coro = CoroutineFunction();
    std::cout << "coroutine co_await\n";
    coroutine_handle.resume();

    return 0;
}

#endif // TEST301

#ifdef TEST401

#include <iostream>

// 定义一个Integral概念，表示T类型必须是整型（符合std::is_integral_v<T>条件）
template <typename T>
concept Integral = std::is_integral_v<T>;

// 函数的参数和返回值都应该满足Integral概念
Integral auto Func(Integral auto a, Integral auto b)
{
    return a + b;
}

int main()
{
    std::cout << Func(10, 5) << std::endl;
}

#endif // TEST401

#ifdef TEST501

#include <algorithm>
#include <array>
#include <iostream>
#include <span>
#include <string>
#include <vector>

/**
 * std::span 是某段连续数据的视图
 * 不拥有数据，不分配和销毁数据
 * 拷贝非常快，推荐值传递
 * 运行期、编译期都可以确定长度
 */

int main()
{
    // 构造 std::span
    {
        std::array<int, 5> arr { 1, 2, 3, 4, 5 };
        std::vector<int> vec { 1, 2, 3, 4, 5 };
        std::string str { "12345" };
        int arr_cstyle[] { 1, 2, 3, 4, 5 };
        int* ptr = new int[5]();

        std::span<int, 5> test_span0 { arr }; // 模板的第二个参数不能大于实际的数据长度
        std::span<int> test_span1 { ptr, 3 };
        std::span<int> test_span2 { arr_cstyle };
        std::span<char> test_span3 { str };
        std::span<int> test_span4 { vec };
        std::span<int, 5> test_span5 { vec }; // std::vector不能在构造时指定长度，模板中的长度必须和std::vector的长度一致，或者模板只要一个参数
        std::span<int> test_span6 { vec.begin(), 3 }; // 使用迭代器初始化，相当于指针

        // std::span<int, 3> test_span00 { arr };  // 编译错误，模板实参不能小于实际数据长度，除非在构造时指定的长度不大于模板实参
    }

    std::cout << "-------------------------------\n";

    // span 不拥有数据
    {
        std::span<int> test_span;
        std::cout << "size: " << test_span.size() << std::endl;
        {
            int* int_ptr = new int[5]();
            int_ptr[0]   = 9;
            test_span    = { int_ptr, 5 };

            std::cout << "size: " << test_span.size() << std::endl;
            std::cout << "front:" << test_span.front() << std::endl;

            delete[] int_ptr;
            int_ptr = nullptr;
        }
        std::cout << "size: " << test_span.size() << std::endl;
        std::cout << "front:" << test_span.front() << std::endl;
    }

    std::cout << "-------------------------------\n";

    // 修改std::span相当于修改源数据
    {
        std::vector vec { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        std::span span1(vec);
        std::span span2 { span1.subspan(1, span1.size() - 2) };

        std::transform(span2.begin(), span2.end(), span2.begin(), [](int i) { return i * i; });

        for (auto elem : vec)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
        for (auto elem : span1)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
        for (auto elem : span2)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "-------------------------------\n";

    // 修改修改源数据相当于std::span
    {
        std::vector vec { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        std::span span1(vec);

        std::transform(vec.begin(), vec.end(), vec.begin(), [](int i) { return i * i; });

        for (auto elem : vec)
        {
            std::cout << elem << ' ';
        }

        std::cout << std::endl;
        for (auto elem : span1)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
}

#endif // TEST501

#ifdef TEST601

#include <iostream>
#include <syncstream>
#include <thread>

void task(bool sync)
{
    if (sync)
    {
        // 确保输出流在多线程环境中同步，免除数据竞争，而且将不以任何方式穿插或截断。
        std::osyncstream { std::cout } << "first line\n"
                                       << "second line\n"
                                       << "third line\n";
    }
    else
    {
        std::cout << "first line\n"
                  << "second line\n"
                  << "third line\n";
    }
}

int main()
{
    // std::jthread 无需手动合并
    {
        std::jthread t1(task, true);
        std::jthread t2(task, true);
        std::jthread t3(task, true);
    }

    std::cout << "--------------------------------------\n";

    {
        std::jthread t1(task, false);
        std::jthread t2(task, false);
        std::jthread t3(task, false);
    }

    std::cout << "--------------------------------------\n";

    {
        std::jthread t([](std::stop_token stop_token) {
            while (!stop_token.stop_requested())
            {
                std::cout << "Working...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            std::cout << "Exiting...\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));
        t.request_stop(); // 请求停止线程
    }

    std::cout << "--------------------------------------\n";

    {
        std::jthread t([](std::stop_token stop_token) {
            while (!stop_token.stop_requested())
            {
                std::cout << "Working...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            std::cout << "Thread stopped.\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto stop_source = t.get_stop_source();
        stop_source.request_stop(); // 请求线程停止
    }

    std::cout << "--------------------------------------\n";

    {
        std::jthread t([](std::stop_token stop_token) {
            while (!stop_token.stop_requested())
            {
                std::cout << "Working...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            std::cout << "Thread stopped.\n";
        });

        // stop_requested()
        // 作用：返回一个布尔值，表示是否收到停止请求。
        // 用法：线程可以通过循环检查 stop_requested() 来决定是否停止运行。

        // stop_possible()
        // 作用：返回一个布尔值，表示是否可以对该 std::stop_token 发起停止请求。
        // 说明：如果没有与 std::stop_token 关联的 std::stop_source，该函数返回 false。

        auto stop1 = t.get_stop_token();
        std::osyncstream { std::cout } << "possible: " << stop1.stop_possible() << "\trequest: " << stop1.stop_requested() << '\n';

        std::this_thread::sleep_for(std::chrono::seconds(2));
        t.request_stop(); // 请求线程停止

        auto stop2 = t.get_stop_token();
        std::osyncstream { std::cout } << "possible: " << stop2.stop_possible() << "\trequest: " << stop2.stop_requested() << '\n';
    }
}

#endif // TEST601

#ifdef TEST602

#include <barrier>
#include <iostream>
#include <thread>

std::barrier b(3); // 创建一个屏障，需要3个线程到达

void task(const char* threadName)
{
    std::cout << threadName << " is waiting\n";
    // b.arrive_and_wait(); // 减少计数并等待（阻塞）
    b.wait(b.arrive()); // 和 b.arrive_and_wait() 等价
    std::cout << threadName << " is processing\n";
}

int main()
{
    {
        std::jthread t1(task, "Thread 1");
        std::jthread t2(task, "Thread 2");
        std::jthread t3(task, "Thread 3");
    }

    std::cout << "-----------------------\n";

    // std::barrier 是周期性的，当等待的线程达到设置的数量时就会重置，可以被再次使用
    {
        std::jthread t1(task, "Thread 1");
        std::jthread t2(task, "Thread 2");
        std::jthread t3(task, "Thread 3");
    }

    return 0;
}

#endif // TEST602

#ifdef TEST603

#include <barrier>
#include <iostream>
#include <thread>

std::barrier b(3); // 创建一个屏障，需要3个线程到达

void task(int id)
{
    std::cout << id << " is waiting\n";

    if (id == 0)
    {
        // 减少当前计数1，并减少重置计数1，不会等待（阻塞）
        b.arrive_and_drop();
    }
    else
    {
        b.arrive_and_wait();
    }

    std::cout << id << " is processing\n";
}

int main()
{
    {
        std::jthread t1(task, 0);
        std::jthread t2(task, 1);
        std::jthread t3(task, 2);
    }

    std::cout << "-----------------------\n";

    // 使用 arrive_and_drop() 之后，只需要两个线程就能满足等待数量
    {
        std::jthread t1(task, 3);
        std::jthread t2(task, 4);
    }
}

#endif // TEST603

#ifdef TEST604

#include <barrier>
#include <iostream>
#include <thread>

// std::barrier 的回调函数必须是无参数的函数，且不能抛出异常
// 回调函数在每次所有线程到达（期望次数减为0）后会被调用
std::barrier b(3, [n = 0]() mutable noexcept { std::cout << "callback " << n++ << std::endl; });

void task(const char* str)
{
    std::cout << "before " << str << std::endl;
    b.arrive_and_wait();
    std::cout << "after " << str << std::endl;
}

int main()
{
    std::jthread t1(task, "t1");
    std::jthread t2(task, "t2");
    std::jthread t3(task, "t3");
}

#endif // TEST604

#ifdef TEST605

#include <chrono>
#include <iostream>
#include <latch>
#include <thread>

// std::latch 只能减少计数，无法增加计数，也不会重置计数
// 是一个比 std::barrier 更简单的线程屏障，只能单次使用
std::latch work_start { 3 };

void work()
{
    std::cout << "wait other thread\n";
    work_start.wait(); // 等待计数为 0
    std::cout << "exectue work\n";
}

int main()
{
    using namespace std::chrono_literals;

    std::jthread thread { work };
    std::this_thread::sleep_for(3s);

    std::cout << "sleep done\n";

    work_start.count_down();  // 默认值是 1 减少计数 1
    work_start.count_down(2); // 传递参数 2 减少计数 2

    return 0;
}

#endif // TEST605

#ifdef TEST606

#include <iostream>
#include <latch>
#include <thread>

std::latch work_start { 3 };

void task(int n)
{
    std::cout << n << " wait other thread\n";
    work_start.arrive_and_wait(); // 等价于 count_down(); wait(); 减少计数并等待（阻塞）
    std::cout << n << " execute work\n";

    // try_wait() 返回期望计数是否等于0
    std::cout << "not need wait: " << work_start.try_wait() << std::endl;
}

int main()
{
    std::jthread t1(task, 0);
    std::jthread t2(task, 1);
    std::jthread t3(task, 2);
}

#endif // TEST606

#ifdef TEST607

#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

std::counting_semaphore<1> sema(0); // 创建一个初始计数值为0的信号量

// std::binary_semaphore 的定义如下
// using binary_semaphore = counting_semaphore<1>;

void worker()
{
    std::cout << "before acquire\n";

    // 尝试获取一个资源，如果计数值为0则阻塞当前线程直到计数值变为正，如果计数值大于0，则减少计数值并继续执行
    sema.acquire();

    // 非阻塞地尝试获取一个资源，如果计数值大于0则减少计数值并返回true，如果计数值为0，则立即返回false
    if (sema.try_acquire())
    {
    }

    std::cout << "after acquire\n";
}

int main()
{
    std::jthread t(worker); // 创建一个新线程

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "before release\n";
    sema.release(); // 增加计数值，最多增加到 std::counting_semaphore<MAX> 的 MAX 值
    std::cout << "after release\n";

    return 0;
}

#endif // TEST607
