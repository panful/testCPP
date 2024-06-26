/*
 * 1. 不要返回局部对象的右值引用
 * 2. 函数返回值没必要使用std::move和std::forward
 * 3. 模板函数不要使用值传递，最好使用万能引用
 * 4. 引用折叠引用塌缩00_13_04_TEST12
 * 5. 判断一个类型是左值还是右值，修改一个类型为左值或右值
 * 6. 函数的参数传值、传引用、传指针
 * 7. std::ref std::cref std::reference_wrapper 给std::bind std::thread等传参
 * 8. 函数返回一个对象，编译器优化 RVO NRVO
 * 9. 形参是万能引用，使用标签分派
 */

// 一文读懂C++右值引用和std::move https://zhuanlan.zhihu.com/p/335994370

#define TEST6

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

int main()
{
    // 当编译器在引用折叠的语境下生成引用的引用时，结果会变成单个引用。
    // 如果原始的引用中有任一引用为左值引用，则结果为左值引用。否则，结果为右值引用
    {
        auto&& n  = 0; // int&&
        auto&& n2 = n; // int&
        auto&& n3 = 3; // int&&
    }
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
#include <vector>

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

    Helper(Helper&&) noexcept
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&) noexcept
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

void f5(const Helper& h)
{
    Helper tmp = h;
}

void f6(Helper&& h)
{
    Helper tmp = h;
}

void f7(Helper&& h)
{
    Helper tmp = std::move(h);
}

void f8(Helper&& h)
{
    f5(h);
}

void f9(Helper h)
{
    Helper tmp = h;
}

class Test
{
    std::vector<int> _data;

public:
    void Set(const std::vector<int>& data)
    {
        _data = data;
    }

    void Set(std::vector<int>&& data)
    {
        _data = std::move(data);
    }
};

int main()
{
    {
        int n = 0;
        f1(n);
        f2(n);
        f3(0);
        f4(n);
    }

    std::cout << "1-------------\n";
    {
        f5(Helper());
    }

    std::cout << "2-------------\n";
    {
        f6(Helper());
    }

    std::cout << "3-------------\n";
    {
        f7(Helper());
    }

    std::cout << "4-------------\n";
    {
        Helper h {};
        f5(std::move(h));
    }

    std::cout << "5-------------\n";
    {
        Helper h {};
        f5(h);
    }

    std::cout << "6-------------\n";
    {
        Helper h {};
        f8(std::move(h));
    }

    std::cout << "7-------------\n";
    {
        Helper h {};
        f9(h);
    }
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
// GDB使用: https://mp.weixin.qq.com/s/XxPIfrQ3E0GR88UsmQNggg

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

#ifdef TEST9

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <type_traits>

class Test
{
public:
    template <typename T>
    void AddName(T&& t)
    {
        AddNameImpl(std::forward<T>(t), std::is_integral<std::remove_reference_t<T>>());
    }

private:
    template <typename T>
    void AddNameImpl(T&& n, std::true_type)
    {
        std::cout << "int true\n";
        AddName(NameFromIdx(std::forward<T>(n)));

        // 不建议使用这种
        // m_names.emplace(NameFromIdx(std::forward<T>(n)));
    }

    template <typename T>
    void AddNameImpl(T&& t, std::false_type)
    {
        std::cout << "int false\n";
        m_names.emplace(std::forward<T>(t));
    }

    std::string NameFromIdx(int i)
    {
        return 0 == m_namesWithIdx.count(i) ? std::string() : m_namesWithIdx.at(i);
    }

private:
    std::set<std::string> m_names;
    std::map<int, std::string> m_namesWithIdx;
};

int main()
{
    Test t;
    t.AddName(1);
    t.AddName("abc");
    t.AddName(std::string("abc"));

    std::cout << "-------------------------------------\n";

    short n = 1;
    t.AddName(n);

    const long long n2 = 2;
    t.AddName(n2);
}

#endif // TEST9
