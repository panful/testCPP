/*
 * 1. std::sort使用lambda
 * 2. constexpr lambda表达式
 * 3. lambda捕获*this
 * 4. 捕获指针，lambda内调用对象方法
 * 5. lambda各种捕获方式的区别 mutable值捕获方式修改捕获的值
 * 6. lambda使用static变量
 * 7. 初始化捕获
 */

#define TEST7

#ifdef TEST1

#include <algorithm>
#include <iostream>
#include <vector>

bool Compare(int a, int b)
{
    std::cout << a << '\t' << b << '\n';
    // return a > b;
    return b > a;
}

int main()
{
    std::vector<int> vec { 1, 3, 5, 4, 2, 6 };
    // std::sort(vec.begin(), vec.end());
    std::sort(vec.begin(), vec.end(), Compare);
}

#endif // TEST1

#ifdef TEST2

// constexpr函数有如下限制：
// 函数体不能包含汇编语句、goto语句、label、try块、静态变量、线程局部存储、没有初始化的普通变量，不能动态分配内存，不能有new delete等，不能虚函数。

#include <string>

int main()
{ // c++17可编译
    constexpr auto lamb = [](int n) { return 1 + n * n; };
    static_assert(lamb(3) == 10, "error");

    constexpr auto lamb2 = [](std::string str) { return "#" + str + "#"; };
    lamb2("s"); // ok
    // static_assert(lamb2("s") == std::string("#s#"), "error"); // 编译不过去
    // static_assert(lamb2("s") == std::string("s"), "error"); // 静态断言失败
}
#endif // TEST2

#ifdef TEST3

#include <iostream>

// 正常情况下，lambda表达式中访问类的对象成员变量需要捕获this，
// 但是这里捕获的是this指针，指向的是对象的引用，正常情况下可能没问题，
// 但是如果多线程情况下，函数的作用域超过了对象的作用域，对象已经被析构了，还访问了成员变量，就会有问题。

// C++17增加了新特性，捕获*this，不持有this指针，而是持有对象的拷贝，这样生命周期就与对象的生命周期不相关啦。

// https://cppinsights.io/

struct MyStruct
{
    int a { 9 };

    int Before17GetA()
    {
        auto f = [this]()
        {
            this->SetA();
            return a;
        };

        return f();
    }

    int After17GetA()
    {
        auto f1 = [*this]()
        {

        // this->SetA(); // error，*this是一个const对象，只能调用常成员函数

        // C++20可以使用return this->a;
        // C++17只能使用return a;

#if __cplusplus > 201703L
            return this->a;
#else
            return a;
#endif
        };

        auto f2 = [*this]() mutable
        {
#if __cplusplus > 201703L
            this->SetA(); // ok
            return this->a;
#else
            SetA();
            return a;
#endif
        };

        f2();
        return f1() == f1() ? 0 : 1;
    }

    void SetA()
    {
        this->a = 88;
    }
};

class MyClass
{
public:
    MyClass()
    {
        s = new MyStruct();
    }

    ~MyClass()
    {
        if (s)
        {
            delete s;
            s = nullptr;
        }
    }

    // 如果捕获*this一定要实现【深拷贝】
    MyClass(const MyClass& obj)
    {
        this->s    = new MyStruct();
        this->s->a = obj.s->a;
    }

    void TestFunc()
    {
        auto lambda1 = [*this]() mutable
        {
#if __cplusplus > 201703L
            this->Func1(); // ok C++20
            this->Func2(); // ok C++20
            std::cout << this->s->After17GetA() << '\n';
#else
            Func2(); // C++17
            Func1(); // C++17
            std::cout << s->After17GetA() << '\n';
#endif
        };

        lambda1();
    }

    // 测试常成员函数和普通函数
    void Func1() const
    {
    }

    void Func2()
    {
    }

private:
    MyStruct* s { nullptr };
};

int main()
{
    MyStruct s1;
    std::cout << s1.Before17GetA() << '\n';
    std::cout << s1.After17GetA() << '\n';

    std::cout << "-----------------\n";

    MyClass c1;
    c1.TestFunc();

    return 0;
}

#endif // TEST3

#ifdef TEST4

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
    void TestNonConst()
    {
        std::cout << "Helper::TestNonConst()\n";
    }

    void TestConst() const
    {
        std::cout << "Helper::TestConst()\n";
    }

    void Test()
    {
        std::cout << &number << '\t' << number << '\n';
    }

private:
    int number { 999 };
};

// 最好不要捕获指针

int main()
{
    std::cout << "----------------------------\n";
    {
        Helper h;
        // 调用TestNonConst()必须使用引用捕获，或者使用mutable关键字
        // 如果不使用mutable，lambda生成的仿函数的operator()是一个const成员函数，所以不能调用捕获对象的非const成员函数
        auto f = [&h]() { h.TestNonConst(); };
        f();
    }

    std::cout << "----------------------------\n";

    // 值捕获会调用拷贝构造函数
    {
        Helper h;
        auto f = [=]() { h.TestConst(); }; // 调用TestConst可以使用值捕获，也可以引用捕获
        f();
        std::cout << "++++++++++++++++++++++++++++\n";
        auto f2 = [h]() { h.TestConst(); };
        f2();
        std::cout << "++++++++++++++++++++++++++++\n";
        auto f3 = [&h]() { h.TestConst(); };
        f3();
    }

    std::cout << "----------------------------\n";
    {
        Helper h;
        auto f = [x = h]() { x.TestConst(); }; // 和直接使用值捕获是一样的
        f();
    }

    std::cout << "----------------------------\n";

    {
        Helper* p = new Helper();
        std::cout << p << '\n';
        auto f = [p]()
        {
            std::cout << p << '\n';
            p->Test(); // 调用Test的时候，Helper的析构函数已经被调用，所以Helper的成员变量也已经被析构
        };
        delete p;
        p = nullptr;
        f();
    }

    std::cout << "----------------------------\n";

    {
        Helper* p = new Helper();
        std::cout << p << '\n';
        // 调用拷贝构造
        auto f = [x = *p]()
        {
            std::cout << &x << '\n';
            x.TestConst(); // 程序没有问题，但是只能调用const成员函数。拷贝一个对象，然后以值的方式捕获这个对象
        };
        delete p;
        p = nullptr;
        f();
    }
    std::cout << "----------------------------\n";
}

#endif // TEST4

#ifdef TEST5

#include <iostream>

int main()
{
    // 值捕获
    {
        int i  = 1;
        auto f = [i] { return i; }; // 等价于 auto f = [i](){return i;};  ()可写可不写
        i      = 10;
        int j  = f();
        std::cout << j << std::endl; // 1，值捕获时，定义lambda之后修改捕获的变量，不会影响lambda的执行结果
    }

    // 引用捕获
    {
        int i  = 1;
        auto f = [&i] { return i; };
        i      = 10;
        int j  = f();
        i      = 20;
        std::cout << j << std::endl; // 10 ，引用捕获时，定义lambda之后修改捕获的变量，lambda使用的是调用时捕获的变量对应的值
    }

    // 隐式值捕获
    {
        int i  = 1;
        int j  = 2;
        auto f = [=] { return i + j; };
        i      = 5;
        int m  = f();
        std::cout << m << std::endl; // 3
    }

    // 隐式引用捕获
    {
        int i  = 1;
        int j  = 2;
        auto f = [&] { return i + j; };
        i      = 3;
        int m  = f();
        std::cout << m << std::endl; // 5
    }

    // 隐式、显式混合捕获1
    {
        int i = 1;
        int j = 2;
        // i为值捕获，j为引用捕获
        auto f = [=, &j] { return i + j; };
        i      = 3;
        j      = 4;
        int m  = f();
        std::cout << m << std::endl; // 5
    }

    // 隐式、显式混合捕获2
    {
        int i = 1;
        int j = 2;
        // i为引用捕获，j为值捕获
        auto f = [&, j]() -> int { return i + j; }; //->int是C++返回值类型后置语法
        i      = 3;
        j      = 4;
        int m  = f();
        std::cout << m << std::endl; // 5
    }

    // 可变lambda（在lambda内修改捕获的值）
    {
        int i  = 10;
        auto f = [&i] /*()mutable*/ { return ++i; }; // 引用捕获mutable可写可不写
        i      = 5;
        int j  = f();
        std::cout << j << std::endl; // 6
    }

    // 可变lambda
    {
        int i  = 10;
        auto f = [i]() mutable { return ++i; }; // 值捕获必须有mutable，不然报错，此处不能省略()
        i      = 5;
        int j  = f();
        std::cout << j << std::endl; // 6
    }

    // lambda不能修改捕获的const变量
    {
        // const int i = 10;
        // auto f = [&i]() mutable {return ++i; };
        // auto f = [i]() mutable {return ++i; };
        // int j = f();
        // std::cout << j << std::endl;
    }

    // 捕获列表和参数列表
    {
        // 参数列表是需要在调用该lambda的时候传入参数，捕获列表是用来声明那些变量可以在lambda内部使用
        int i = 1;
        auto f = [i](int x) { return -(i + x); }; // 参数列表：调用时需要传入一个int值，捕获列表：变量i可以在lambda内部使用
        auto j = f(3);
        std::cout << j << std::endl;              // -(1+3) = -4
    }

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <iostream>

int n         = 0;
static int sn = 1;

int main()
{
    // gcc11 C++20 lambda可以捕获静态变量，但是会有警告
    // msvc c++20 lambda不能捕获静态变量
    {
        auto f = [n]() { std::cout << n << '\n'; };
        f();
    }

    std::cout << "----------------------------------------\n";

    {
        auto f = [sn]() { std::cout << sn << '\n'; };
        f();
    }

    std::cout << "----------------------------------------\n";

    // 会修改sn的值
    {
        std::cout << sn << '\n';
        auto f = [sn]()
        {
            ++sn;
            std::cout << sn << '\n';
        };
        std::cout << sn << '\n';
        f();
        std::cout << sn << '\n';
    }

    std::cout << "----------------------------------------\n";

    // 静态变量是不需要捕获的，直接可以在lambda内部使用
    // 相当于引用捕获
    {
        std::cout << n << '\t' << sn << '\n';
        auto f = []()
        {
            n++;
            sn++;
            std::cout << "lambda: " << n << '\t' << sn << '\n';
        };
        std::cout << n << '\t' << sn << '\n';
        f();
        std::cout << n << '\t' << sn << '\n';
    }
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <vector>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    Helper(const Helper&);
    Helper& operator=(const Helper&) = delete;

    Helper(Helper&&)
    {
        std::cout << "move construct\n";
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
    void TestNonConst()
    {
        std::cout << "Helper::TestNonConst()\n";
    }

    void TestConst() const
    {
        std::cout << "Helper::TestConst()\n";
    }
};

int main()
{
    {
        Helper h;

        // 调用移动构造函数。不能直接值捕获，因为拷贝构造函数是delete的
        // 捕获列表中=左边的x其实就是lambda生成的仿函数的成员变量的名称
        // =右边的表达式是成员变量的源，所以=两边如果是变量名是可以相同的
        auto f = [x = std::move(h)]() mutable
        {
            x.TestConst();
            x.TestNonConst();
        };
        f();
    }

    std::cout << "----------------------------------------------------------\n";

    {
        std::vector<int> vec { 1, 2, 3 };
        std::cout << vec.size() << '\n';

        // 调用移动构造，所以lambda作用域外边的vec会被清空
        auto f = [x = std::move(vec)]() { std::cout << x.size() << '\n'; };
        std::cout << vec.size() << '\n';
        f();
        std::cout << vec.size() << '\n';
    }
}

#endif // TEST7