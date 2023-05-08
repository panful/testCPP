/*
 * 1. const和constexpr 【修饰指针】时在不同位置的作用
 * 2. const和constexpr区别，常量字符串
 * 3. const对象不能调用那些成员函数
 * 4. 修改const变量的值，常量折叠
 * 5. const和constexpr的区别
 * 6. 普通函数使用constexpr
 * 7. 构造函数、成员函数使用constexpr
 * 8. std::as_const
 * 9. mutable
 */

#define TEST7

#ifdef TEST1

#include <iostream>

// 一般不会用constexpr修饰指针

constexpr int x = 110;

int main()
{
    const int* p1           = nullptr; // p1是一个指向常量的变量指针，指针本身是变量
    constexpr int* p2       = nullptr; // p2是一个指向变量的常量指针，指针本身是常量，没啥用，和p3效果是一样的
    constexpr const int* p3 = nullptr; // p3是一个指向常量的常量指针
    const int* const p4     = nullptr; // p4是一个指向常量的常量指针
    int* const p5           = nullptr; // p5是一个指向变量的常量指针，指针本身是常量
    // int* constexpr p6       = nullptr; // 错误，不能这样定义

    p1 = &x; // 正确，x是常量
    //*p1 = 2;      // 错误

    // p2 = &x;     // 错误
    *p2 = 2; // 正确，需要判断指针，p2为空会中断

    // p3 = &x;     // 错误
    //*p3 = 2;      // 错误

    // p4 = &x;     // 错误
    //*p4 = 2;      // 错误

    // p5 = &x;     // 错误
    *p5 = 2; // 正确，但是p5为空会中断

    std::cout << *p1 << '\n' << *p2 << '\n' << *p5 << '\n';

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <array>
#include <iostream>
#include <string>
#include <string_view>

// const 表示的是只读 constexpr 表示的常量
// 所有constexpr对象都是const对象，而并非所有的const对象都是constexpr对象
// constexpr对象都具备const属性，并由编译期已知的值完成初始化
// constexpr就好像是告诉编译器尝试在编译期确定值，如果编译期不能确定也没关系

int main()
{
    {
        const std::string s1          = "123";
        const std::string_view s2     = "234";
        constexpr std::string_view s3 = "456";
        const char* s4                = "567";
        // constexpr char* s5            = "678"; // 会有警告：禁止将字符串常量转换为char*("678"的类型是const char*)

        // constexpr std::string s3      = "234"; // 错误

        std::cout << s1 << '\n' << s2 << '\n' << s3 << '\n' << s4 << '\n';
    }

    {
        std::array<int, 3> arr1;

        const size_t size1 = 3;
        std::array<int, size1> arr2;

        constexpr size_t size2 = 3;
        std::array<int, size2> arr3;

        // size_t size3 = 3;
        // std::array<int, size3> arr3; // error
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>

class A
{
public:
    void Func1() const
    {
        std::cout << "func1\n";
    }

    void Func2()
    {
        std::cout << "func2\n";
    }

    const int Func3()
    {
        std::cout << "func3\n";
        return 1;
    }

    void Func4(const int n)
    {
        std::cout << "func4\n";
    }

    const int Func5() const
    {
        std::cout << "func5\n";
        return 1;
    }
};

int main()
{
    // const对象只能调用函数声明后面带const的成员函数

    const auto a1 = A();
    a1.Func1(); // 正确
    // a1.Func2();  // 错误
    // a1.Func3();  // 错误
    // a1.Func4(0); // 错误
    a1.Func5();

    std::cout << "----------------------\n";

    auto a2 = A();
    a2.Func1();
    a2.Func2();
    a2.Func3();
    a2.Func4(0);
    a2.Func5();
}

#endif // TEST3

#ifdef TEST4

#include <iostream>

int main()
{
    {
        int a        = 10;
        const int& b = a;
        std::cout << "a = " << a << "\tb =" << b << std::endl;
        a = 20;
        std::cout << "a = " << a << "\tb =" << b << std::endl;     // b的值被改为了20
        std::cout << "&a = " << &a << "\t&b =" << &b << std::endl; // 地址一样，值一样
    }

    std::cout << "----------------\n";

    {
        const int a = 10;
        int& b      = const_cast<int&>(a);
        std::cout << "a = " << a << "\tb =" << b << std::endl;
        b = 20;
        std::cout << "a = " << a << "\tb =" << b << std::endl;     // a的值没有改变
        std::cout << "&a = " << &a << "\t&b =" << &b << std::endl; // 地址一样，值不一样
    }

    std::cout << "-------------------\n";

    {
        // 常量折叠 http://www.cppblog.com/xmli/archive/2010/11/23/134425.html
        // 预编译阶段，编译器会进行优化，将const的值全部替换掉，但在实际运行阶段，内存里的值被改变了
        const int a = 1;
        int* b      = const_cast<int*>(&a);

        std::cout << "a = " << a << "\tb =" << *b << std::endl;
        *b = 2;
        std::cout << "a = " << a << "\tb =" << *b << std::endl;    // a的值没有改变
        std::cout << "&a = " << &a << "\t&b = " << b << std::endl; // 地址一样，值不一样
    }

    return 0;
}

#endif // TEST4

#ifdef TEST6

// 构造函数使用constexpr https://blog.csdn.net/craftsman1970/article/details/80244873
// constexpr的使用 https://docs.microsoft.com/zh-cn/cpp/cpp/constexpr-cpp?view=msvc-170

#include <iostream>

int test1()
{
    return 8;
}

// c++11 constexpr函数不能返回void
// c++14 可以返回void
constexpr void test2()
{
    return;
}

const int test3()
{
    return 3;
}

// constexpr函数只能调用constexpr修饰的函数
constexpr int test4()
{
    // test1();     // error，因为test1没有被constxepr修饰
    test2(); // ok
    // test3();     // error，因为test3没有被constexpr修饰，只被const修饰
    // test3() + 2; // error

    return 1;
}

// 只要调用的是constexpr函数或者使用字面值常量（编译期确定）都可以编译通过
constexpr int test5(int x)
{
    // return x;            // ok

    // return 1 + 2 + x;    // ok

    // int a = x * x;
    // a = 3;
    // return a + x;        // ok

    // int ret = 1 + 2 + x;
    // return ret;          // ok

    return test4() + x + 2;
}

// 值传递
constexpr float func1(float x, int n)
{
    return x * n;
}

// 引用传递
constexpr float func2(const float& x, const int& n)
{
    return x * n;
}

// 编译期计算数组长度
template <typename T, int N>
constexpr int func3(const T (&)[N])
{
    return N;
}

// 递归
constexpr int func4(int n)
{
    return n == 1 ? 1 : n * func4(n - 1);
}

int main()
{
    {
        std::cout << test1() << '\n';
        std::cout << test5(test1()) << '\n';
        auto n = test1();
        std::cout << test5(n) << '\n';

        int x    = 4;
        auto ret = test5(x);
        std::cout << ret << '\n';
    }

    std::cout << "---------------------\n";

    {
        int x              = 3;
        auto ret1          = test5(x); // auto被推导为int，而不是constexpr int
        constexpr int ret2 = test5(1); // ok
        // constexpr int ret3 = test5(x); //error 如果返回值被constexpr修饰的变量接收，则参数必须为常量

        const int y        = 4;
        constexpr int ret4 = test5(y); // ok
    }

    return 0;
}
#endif // TEST6

#ifdef TEST7

#include <iostream>

class Test
{
public:
#if __cplusplus < 202002L
    // c++20以下不支持构造函数内修改成员变量，只能在初始化列表修改
    // 且成员变量如果不用初始化列表初始化，必须在声明的时候初始化
    constexpr explicit Test() noexcept : m_int(0)
    {
        int x = 666;
    }

    constexpr explicit Test(int n) noexcept : m_int(n)
    {
        int x = 666;
    }
#else
    // 只要构造函数内部不调用非constexpr函数或者不能在编译期确定值的函数就可以使用constexpr
    constexpr explicit Test() noexcept
    {
        int x = 666;
    }

    constexpr explicit Test(int n) noexcept
    {
        m_int = n;
        int x = 666;
    }
#endif
public:
    constexpr void SetValue(int n) noexcept
    {
        m_int = n;
    }

    constexpr int GetValue() const noexcept
    {
        return m_int;
    }

private:
    int m_int { 0 };
};

int main()
{
    {
        Test t;
        auto v1 = t.GetValue();
        t.SetValue(2);
        auto v2 = t.GetValue();

        std::cout << v1 << '\t' << v2 << '\n';
    }

    {
        Test t(1);
        auto v1 = t.GetValue();
        t.SetValue(2);
        auto v2 = t.GetValue();
        std::cout << v1 << '\t' << v2 << '\n';
    }

    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <utility> // std::as_const

int main()
{
    std::cout << std::boolalpha;

    {
        std::string str1             = "str";
        const std::string str2       = "str";
        auto constStr1               = std::as_const(str1);
        auto& constStr2              = std::as_const(str1);
        const std::string constStr3  = std::as_const(str1);
        const std::string& constStr4 = std::as_const(str1);

        std::cout << std::is_const_v<decltype(str1)> << '\n';
        std::cout << std::is_const_v<decltype(str2)> << '\n';
        std::cout << std::is_const_v<decltype(constStr1)> << '\n';
        std::cout << std::is_const_v<decltype(constStr2)> << '\n';
        std::cout << std::is_const_v<decltype(constStr3)> << '\n';
        std::cout << std::is_const_v<decltype(constStr4)> << '\n';
    }
    std::cout << "----------------------\n";
    {
        int A1             = 2;
        const int A2       = 2;
        auto constA1       = std::as_const(A1);
        auto& constA2      = std::as_const(A1);
        const int constA3  = std::as_const(A1);
        const int& constA4 = std::as_const(A1);

        std::cout << std::is_const_v<decltype(A1)> << '\n';
        std::cout << std::is_const_v<decltype(A2)> << '\n';
        std::cout << std::is_const_v<decltype(constA1)> << '\n';
        std::cout << std::is_const_v<decltype(constA2)> << '\n';
        std::cout << std::is_const_v<decltype(constA3)> << '\n';
        std::cout << std::is_const_v<decltype(constA4)> << '\n';
    }
    std::cout << "----------------------\n";
    {
        const char* name_const { "Tom" };
        std::cout << std::format("type of name_const is: {}\n", typeid(name_const).name());
        std::cout << std::is_const_v<decltype(name_const)> << '\n';

        auto name_remove_const = const_cast<char*>(name_const);
        std::cout << std::format("type of name_remove_const is: {}\n", typeid(name_remove_const).name());
        std::cout << std::is_const_v<decltype(name_remove_const)> << '\n';

        auto name_add_const = const_cast<const char*>(name_remove_const);
        std::cout << std::format("type of name_add_const is: {}\n", typeid(name_add_const).name());
        std::cout << std::is_const_v<decltype(name_add_const)> << '\n';

        std::string str { "Hello world" };
        const std::string& str_const_ref = std::as_const(str);
        std::cout << std::format("type of str_const_ref is : {}\n", typeid(str_const_ref).name());
        std::cout << std::is_const_v<decltype(str_const_ref)> << '\n';
    }
}

#endif // TEST8

#ifdef TEST9

#include <iostream>

class Test
{
public:
    int GetValue() const
    {
        if (!m_init)
        {
            // do some thing
            m_init = true;
        }

        return m_value;
    }

    void SetValue(int n)
    {
        m_value = n;
        m_init  = true;
    }

private:
    int m_value { 0 };
    // 因为m_init在const修饰的成员函数内被修改，所以m_init需要被mutable修饰
    mutable bool m_init { false };
};

int main()
{
    Test t;
    std::cout << t.GetValue() << '\n';
    t.SetValue(666);
    std::cout << t.GetValue() << '\n';

    return 0;
}

#endif // TEST9
