/*
 * 1. enum枚举的按位与或非 00_22_TEST4
 * 2. unsigned int 和 unsigned char[4] 互相转换 https://www.cnblogs.com/mmmmmmmmm/p/14586206.html
 * 3. 位运算符 & | ！ >> <<
 * 4. functional头文件中的位操作函数，逻辑操作函数
 * 5. 重载 bool (也属于类型转换)
 * 6. 重载类型转换
 */

#define TEST6

#ifdef TEST1

int main()
{
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

int main()
{
    // unsigned int 分解为 unsigned char[4]
    unsigned int x = 0x04030201;
    auto a         = x & 0xFF;       // [0-7]
    auto b         = x >> 8 & 0xFF;  // [8-15]
    auto c         = x >> 16 & 0xFF; // [16-23]
    auto d         = x >> 24 & 0xFF; // [24-31]

    // 将unsigned char[4]合并为unsigned int
    unsigned int y = 0;
    y |= a;
    y |= b << 8;
    y |= c << 16;
    y |= d << 24;

    std::cout << a << '\t' << b << '\t' << c << '\t' << d << '\n';
    std::cout << x << '\n' << y << '\n';
}
#endif // TEST2

#ifdef TEST3

#include <iostream>

int main()
{
    auto c0 = 0x01020304 >> 8; // c0 = 0x010203 类型为int

    unsigned int c1  = 0x01020304 >> 16; // c1 = 0x0102
    unsigned char c2 = 0x01020304 >> 16; // c2 = 0x02   取uint的最低8位

    uint32_t ui1 = 3;
    uint32_t ui2 = 5;
    auto ret1    = ui1 | ui2; // ret1 = 7  按位或（二进制）

    ui1 |= ui2; // ui1 = ui1|ui2;

    int test = 0;
}

#endif // TEST3

#ifdef TEST4

#include <functional>
#include <iostream>

int main()
{
    std::bit_or<int> a;
    auto ret1 = a(2, 5); // 等价于 auto ret1 = 2|5

    std::logical_and<bool> b;
    auto ret2 = b(false, true); // 等价于 auto ret2 = false && true;

    auto ret3 = std::not_fn(ret2); // 等价于 auto ret3 = !ret2;

    std::plus<int> c;
    auto ret4 = c(2, 4); // 等价于 auto ret4 = 2 + 4;

    // std::equal_to d;  //可以自动识别类型
    std::equal_to<int> d;
    auto ret5 = d(2, 3); // 等价于 auto ret5 = (2 == 3);

    std::ranges::equal_to e; // C++ 20
    auto ret6 = e(2, 3.3);   //    auto ret6 = (2 == 3.3);
    auto ret7 = e(2, 'b');   //    auto ret7 = (2 == 'b');

    return 0;
}
#endif // TEST4

#ifdef TEST5

#include <iostream>

struct Test
{
    constexpr explicit operator bool() const noexcept
    {
        return true;
    }
};

int main()
{
    Test t;

    // 如果Test没有重载bool，则不能对Test对象做判断(if)操作
    if (t)
    {
        std::cout << "true\n";
    }

    // 如果使用explicit，则Test对象转换为bool类型必须如下显式转换，不能 bool b = t
    bool b = static_cast<bool>(t);
}

#endif // TEST5

#ifdef TEST6

#include <iostream>

template <typename T>
struct Test
{
    constexpr operator T*() const noexcept
    {
        return static_cast<T*>(this->Object);
    }

    T* Object;
};

int main()
{
    Test<int> t;
    int* p = t; // 将 t 隐式转换为 int* 类型，避免隐式转换可以使用 explicit
}

#endif // TEST6
