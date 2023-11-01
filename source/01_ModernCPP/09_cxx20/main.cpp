/**
 * 101. std::format 的使用
 * 201. std::ranges std::views
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

#include <iostream>
#include <ranges>
#include <vector>

int main()
{
    {
        std::vector vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // filter 提取偶数
        // transform 将提取到的偶数乘2
        // 结果res是惰性求值，只有在用到的时候才会实际执行views操作
        auto res = vec | std::views::filter([](auto v) { return v % 2 == 0; }) | std::views::transform([](auto v) { return v * 2; });

        for (auto elem : res)
        {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
}

#endif // TEST201