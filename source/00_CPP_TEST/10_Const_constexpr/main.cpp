/*
* 1. const和constexpr关键字修饰指针时在不同位置的作用
* 2. 常量字符串 string string_view
* 3. const对象不能调用那些成员函数
* 4. 修改const变量的值，常量折叠
* 5. 构造函数使用constexpr https://blog.csdn.net/craftsman1970/article/details/80244873
* 6. constexpr的使用 https://docs.microsoft.com/zh-cn/cpp/cpp/constexpr-cpp?view=msvc-170
*/

#define TEST6

#ifdef TEST1

#include <iostream>

constexpr int x = 110;

int main()
{
    const int* p1 = nullptr;              //p1是一个指向常量的指针，指针本身是变量
    constexpr int* p2 = nullptr;        //p2是一个指向变量的常量指针，指针本身是常量
    constexpr const int* p3 = nullptr;  //p3是一个指向常量的常量指针
    const int* const p4 = nullptr;      //p4是一个指向常量的常量指针

    p1 = &x;    // 正确，x是常量
    //*p1 = 2;  // 错误
    //p2 = &x;  // 错误
    *p2 = 2;    // 正确，需要判断指针
    //p3 = &x;  // 错误
    //*p3 = 2;  // 错误
    //p4 = &x;  // 错误
    //*p4 = 2;  // 错误

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <string>
#include <string_view>

// constexpr只会添加顶层const，不会添加底层const
// 对于顶层const，赋值的时候是可以被去除的，而底层const则不行

int main()
{
    const std::string s1 = "123";
    const std::string_view s2 = "234";
    //constexpr std::string s3 = "234";    //错误
    constexpr std::string_view s4 = "456";

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
    a1.Func1();    // 正确
    //a1.Func2();  // 错误
    //a1.Func3();  // 错误
    //a1.Func4(0); // 错误
    a1.Func5();

    std::cout << "------------\n";

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
        int a = 10;
        const int& b = a;
        std::cout << "a = " << a << "\tb =" << b << std::endl;
        a = 20;
        std::cout << "a = " << a << "\tb =" << b << std::endl;     // b的值被改为了20
        std::cout << "&a = " << &a << "\t&b =" << &b << std::endl; // 地址一样，值一样
    }

    std::cout << "----------------\n";

    {
        const int a = 10;
        int& b = const_cast<int&>(a);
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
        int* b = const_cast<int*>(&a);

        std::cout << "a = " << a << "\tb =" << *b << std::endl;
        *b = 2;
        std::cout << "a = " << a << "\tb =" << *b << std::endl;    // a的值没有改变
        std::cout << "&a = " << &a << "\t&b = " << b << std::endl; // 地址一样，值不一样
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5


#endif // TEST5

#ifdef TEST6

// constexpr.cpp
// Compile with: cl /EHsc /W4 constexpr.cpp
#include <iostream>

using namespace std;

// Pass by value
constexpr float exp(float x, int n)
{
    return n == 0 ? 1 :
        n % 2 == 0 ? exp(x * x, n / 2) :
        exp(x * x, (n - 1) / 2) * x;
}

// Pass by reference
constexpr float exp2(const float& x, const int& n)
{
    return n == 0 ? 1 :
        n % 2 == 0 ? exp2(x * x, n / 2) :
        exp2(x * x, (n - 1) / 2) * x;
}

// Compile-time computation of array length
template<typename T, int N>
constexpr int length(const T(&)[N])
{
    return N;
}

// Recursive constexpr function
constexpr int fac(int n)
{
    return n == 1 ? 1 : n * fac(n - 1);
}

// User-defined type
class Foo
{
public:
    constexpr explicit Foo(int i) : _i(i) {}
    constexpr int GetValue() const
    {
        return _i;
    }
private:
    int _i;
};

int main()
{
    // foo is const:
    constexpr Foo foo(5);
    // foo = Foo(6); //Error!

    // Compile time:
    constexpr float x = exp(5, 3);
    constexpr float y{ exp(2, 5) };
    constexpr int val = foo.GetValue();
    constexpr int f5 = fac(5);
    const int nums[]{ 1, 2, 3, 4 };
    const int nums2[length(nums) * 2]{ 1, 2, 3, 4, 5, 6, 7, 8 };

    // Run time:
    cout << "The value of foo is " << foo.GetValue() << endl;
}
#endif // TEST6

