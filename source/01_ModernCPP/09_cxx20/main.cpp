/**
 * 101. std::format 的使用
 * 201. std::ranges std::views
 * 301. 协程 coroutine
 * 401. 概念 concepts https://zhuanlan.zhihu.com/p/600617910?utm_id=0
 * 501. std::span
 */

#define TEST201

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
}

#endif // TEST101

#ifdef TEST201

#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

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
        auto res1 { std::ranges::transform_view { std::ranges::filter_view { numbers, even }, square } };
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
