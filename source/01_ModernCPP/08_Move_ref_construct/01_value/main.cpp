/*
 * 1. 不要返回局部对象的右值引用
 * 2. 函数返回值没必要使用std::move和std::forward
 * 3. 模板函数不要使用值传递，最好使用万能引用
 * 4. 函数参数为值传递，左值引用传递，右值引用传递，常引用传递  引用塌缩  https://www.qb5200.com/article/295128.html
 * 5. 判断一个类型是左值还是右值，修改一个类型为左值或右值
 * 6. 函数的参数传值、传引用、传指针
 * 7. std::ref std::cref std::reference_wrapper 给std::bind std::thread等传参
 * 8. 函数返回一个对象，编译器优化 RVO NRVO
 */

// 一文读懂C++右值引用和std::move https://zhuanlan.zhihu.com/p/335994370

#define TEST8

#ifdef TEST1

#include <iostream>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    Helper(const Helper&)
    {
        std::cout << "copy construct\n";
    }

    Helper(Helper&&)
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&)
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }

public:
    void DoSomeThing() const
    {
        std::cout << "Do some thing\n";
    }
};

Helper f1()
{
    Helper h;
    h.DoSomeThing();
    return h;
}

// 返回局部对象的右值引用会有警告，不要返回局部对象的右值引用
// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-return-ref-ref
Helper f2()
{
    Helper h;
    h.DoSomeThing();
    return std::move(h);
}

Helper f3()
{
    return {};
}

Helper f4()
{
    return Helper();
}

int main()
{
    std::cout << "--------------------------------------\n";
    auto&& h1 = f1();
    std::cout << "--------------------------------------\n";
    auto&& h2 = f2();
    std::cout << "--------------------------------------\n";
    auto&& h3 = f3();
    std::cout << "--------------------------------------\n";
    auto&& h4 = f4();
    std::cout << "--------------------------------------\n";

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    Helper(const Helper&)
    {
        std::cout << "copy construct\n";
    }

    Helper(Helper&&)
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&)
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

Helper operator+(Helper&& h1, const Helper& h2)
{
    // 不使用-fno-elide-constructors显式禁用构造函数优化时，两种返回方法是没有区别的
    // 显式禁用构造函数优化时，返回std::move(h1)是右值，直接返回h1是左值
    // 所以不要返回右值引用 https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-return-ref-ref

    // return std::move(h1);
    return h1;
}

template <typename T>
T Func(T&& t)
{
    // 不显式禁用构造函数优化时，这两种返回方式没区别

    // t.DoSomeThing();
    // return std::forward<T>(t);
    return t;
}

int main()
{
    std::cout << "1--------------------------------------\n";

    Helper h1;
    Helper h2;

    std::cout << "2--------------------------------------\n";

    auto&& h3 = std::move(h1) + h2;

    std::cout << "3--------------------------------------\n";

    auto&& h4 = Helper() + h2;

    std::cout << "4--------------------------------------\n";

    auto&& h5 = Func(std::move(h2));

    std::cout << "5--------------------------------------\n";

    auto&& h6 = Func(h1);

    std::cout << "6--------------------------------------\n";

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>

// 正确析构
template <typename T>
void Func1(T&& t)
{
    std::cout << "111\n";
}

// 正确析构
template <typename T>
void Func2(T& t)
{
    std::cout << "222\n";
}

// 如果T没有拷贝构造函数就会崩溃，因为值传递会有一次参数拷贝，参数拷贝时需要有拷贝构造函数
// 函数执行完，会调用一次t的析构函数，main函数结束的时候也会调用析构，如果参数对应的类没有拷贝构造函数
// 就只new了一次，但是析构两次调用两次delete就会崩溃
template <typename T>
void Func3(T t)
{
    std::cout << "333\n";
}

class A
{
public:
    A()
    {
        m_aa = new int[10]();
        std::cout << "construct\n";
    }

    A(const A& a)
    {
        m_aa = new int[10]();
        for (size_t i = 0; i < 10; ++i)
        {
            m_aa[i] = a.m_aa[i];
        }
        std::cout << "copy construct\n";
    }

    ~A()
    {
        if (m_aa)
        {

            delete m_aa;
            m_aa = nullptr;
            std::cout << "destruct\n";
        }
    }

private:
    int* m_aa { nullptr };
};

int main()
{
    A a;
    Func1(a);
    Func2(a);
    // 执行函数Func3函数体之前会调用拷贝构造函数，将a拷贝到实参
    Func3(a);
    // 执行完Func3之后，就会调用一次析构函数，将拷贝的参数析构

    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <iostream>

// Test1相比于其他三个要多一次构造（拷贝构造）

// 值传递，会有拷贝消耗，对传入的参数不能修改（内部修改的只是拷贝后的值，外部的变量没有改变）
template <class T>
T* Test1(T arg)
{
    return new T(arg); // new 出来的没有delete就会少一次析构，new调用的拷贝构造函数
}

// 左值引用，不能传常参数等右值
template <class T>
T* Test2(T& arg)
{
    return new T(arg);
}

// 常引用，左值右值都可以传，但是不能对传入的参数进行修改
template <class T>
T* Test3(const T& arg)
{
    return new T(arg);
}

// 万能引用，可以解决参数完美转发的问题 关于万能引用请看00_13_TEST12
// 引用塌缩
template <class T>
T* Test4(T&& arg)
{
    return new T(std::forward<T&&>(arg));
}

class A
{
public:
    A()
    {
        std::cout << "construct\n";
    }

    A(const A& a)
    {
        std::cout << "copy construct\n";
    }

    A(const A&& a) noexcept
    {
        std::cout << "move construct\n";
    }

    ~A()
    {
        std::cout << "destruct\n";
    }
};

int main()
{
    std::cout << "0-------------------\n";
    {
        A a;
        auto ret = Test1(a);
        // delete ret; // 此处如果不delete就会少一次析构，后面的例子也一样
        // ret = nullptr;
    }
    std::cout << "1-------------------\n";
    {
        A a;
        auto ret = Test2(a);
    }
    std::cout << "2-------------------\n";
    {
        A a;
        auto ret = Test3(a);
    }
    std::cout << "3-------------------\n";
    {
        A a;
        // auto ret1 = Test4(a); // 参数类型错误
        auto ret2 = Test4(std::move(a));
    }
    std::cout << "4-------------------\n";
    {
        auto ret = Test4(A());
    }
    std::cout << "5-------------------\n";

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <type_traits>

int main()
{
    using MyInt  = int;                                // int
    using lMyInt = std::add_lvalue_reference_t<MyInt>; // int&
    using rMyInt = std::add_rvalue_reference_t<MyInt>; // int&&

    MyInt n1  = 1;
    lMyInt n2 = n1;
    rMyInt n3 = 1;

    std::cout << std::boolalpha;

    std::cout << std::is_lvalue_reference_v<MyInt> << '\n'
              << std::is_lvalue_reference_v<lMyInt> << '\n'
              << std::is_lvalue_reference_v<rMyInt> << '\n';

    std::cout << "---------------------\n";

    std::cout << std::is_rvalue_reference_v<MyInt> << '\n'
              << std::is_rvalue_reference_v<lMyInt> << '\n'
              << std::is_rvalue_reference_v<rMyInt> << '\n';

    return 0;
}

#endif // TEST5

#ifdef TEST6

// 什么时候用值传递，什么时候用引用传递：https://cplusplus.com/articles/z6vU7k9E/
// 关于传值与传引用的讨论：https://www.cnblogs.com/QG-whz/p/5129173.html

// 基本类型值传递效率最高，int float double 指针等
// 例如以下1的方式传参效率最高，其他三个汇编都会有两次mov（寻址操作），引用基于指针
// 1.int 2.int& 3.const int& 4.int&&
// 在 x86-64 上，对于只有一个指针成员且没有自定义复制构造函数的类，传值是可以通过寄存器进行的，就像传递普通int和指针那样

#include <iostream>

int x = 1;

void f1(int n)
{
    x = n + 1;
}

void f2(int& n)
{
    x = n + 1;
}

void f3(int&& n)
{
    x = n + 1;
}

void f4(const int& n)
{
    x = n + 1;
}

int main()
{
    int n = 0;
    f1(n);
    f2(n);
    f3(0);
    f4(n);
}

#endif // TEST6

#ifdef TEST7

// https://www.cnblogs.com/jerry-fuyi/p/12747850.html
// std::ref std::cref返回值就是std::reference_wrapper
// std::reference_wrapper就是把模板参数中的类型，对应的指针包裹了一层
// 绑定到需要通过引用传递参数的函数时非常有用：std::bind std::thread等

#include <functional>
#include <iostream>
#include <type_traits>

void Func(int& n)
{
    ++n;
}

void Func2(const int& n)
{
}

int main()
{
    {
        // std::bind会将参数n拷贝，然后传给Func
        // 所以此处n的值并不会改变
        int n  = 0;
        auto f = std::bind(Func, n);
        f(n);
        std::cout << "++0: " << n << '\n';
    }

    {
        // 使用std::ref将n的引用传给std::bind
        int n  = 0;
        auto f = std::bind(Func, std::ref(n));
        f(n);
        std::cout << "++0: " << n << '\n';
    }

    {
        int n   = 0;
        auto rw = std::reference_wrapper<int>(n);
        auto f  = std::bind(Func, rw);
        f(n);
        std::cout << "++0: " << n << '\n';
    }

    {
        // 常引用
        int n   = 0;
        auto rw = std::cref(n); // std::reference_wrapper<const int>
        auto f  = std::bind(Func2, rw);
        f(n);
    }

    // std::reference_wrapper 还可以包裹函数指针等其他任何类型
    {
        std::reference_wrapper<decltype(Func)> rw1 = std::ref(Func);
        std::reference_wrapper<void(int&)> rw2     = std::ref(Func);

        int n = 0;
        rw1(n);
        rw2(n);
    }

    return 0;
}
#endif // TEST7

#ifdef TEST8

#include <iostream>

class A
{
public:
    constexpr A() noexcept
    {
        std::cout << "construct\n";
    }

    ~A() noexcept
    {
        std::cout << "destruct\n";
    }

    A(const A&) noexcept
    {
        std::cout << "copy construct\n";
    }

    A(A&&) noexcept
    {
        std::cout << "move construct\n";
    }

    A& operator=(const A&) noexcept
    {
        std::cout << "copy assign\n";
        return *this;
    }

    A& operator=(A&&) noexcept
    {
        std::cout << "move assign\n";
        return *this;
    }

    int m { 1 };
    int n { 2 };
    int k { 3 };
};

A get()
{
    A aa;
    aa.m = 7;
    aa.n = 8;
    aa.k = 9;
    return aa;
}

// https://mp.weixin.qq.com/s/qzf97S7jld7GJle2yJPmLw
// https://mp.weixin.qq.com/s/rpnElZyBNVsicMR7j4SQvQ
// GDB: https://mp.weixin.qq.com/s/XxPIfrQ3E0GR88UsmQNggg

int main()
{
    { 
        // 整个过程只有一次构造一次析构，用GDB可以看到get()中的aa和main()中的a是同一个地址
        // 在main函数中构造a，然后将这个a传给get()函数，所以get()函数内部不会构造aa，只需要操作main函数中创建的a
        A a = get();
    }
    std::cout << "-------------------------------\n";
    {

        A a;       // 构造
        a = get(); // 移动赋值
    }
}

#endif // TEST8
