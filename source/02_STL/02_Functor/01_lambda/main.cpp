/*
* 1. std::sort使用lambda
* 2. constexpr lambda表达式
* 3. lambda捕获*this
* 5. lambda各种捕获方式的区别 mutable值捕获方式修改捕获的值
*/

#define TEST3

#ifdef TEST1

#include <vector>
#include <algorithm>
#include <iostream>

bool Compare(int a, int b)
{
    std::cout << a << '\t' << b << '\n';
    //return a > b;
    return b > a;
}

int main()
{
    std::vector<int> vec{ 1,3,5,4,2,6 };
    //std::sort(vec.begin(), vec.end());
    std::sort(vec.begin(), vec.end(), Compare);
}

#endif // TEST1

#ifdef TEST2

// constexpr函数有如下限制：
// 函数体不能包含汇编语句、goto语句、label、try块、静态变量、线程局部存储、没有初始化的普通变量，不能动态分配内存，不能有new delete等，不能虚函数。

#include <string>

int main() { // c++17可编译
    constexpr auto lamb = [](int n) { return 1 + n * n; };
    static_assert(lamb(3) == 10, "error");

    constexpr auto lamb2 = [](std::string str) {return "#" + str + "#"; };
    lamb2("s"); // ok
    //static_assert(lamb2("s") == std::string("#s#"), "error"); // 编译不过去
    //static_assert(lamb2("s") == std::string("s"), "error"); // 静态断言失败
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
    int a{ 9 };

    int Before17GetA() {
        auto f = [this]() {
            this->SetA();
            return a;
        };

        return f();
    }

    int After17GetA() {
        auto f1 = [*this]() {

            //this->SetA(); // error，*this是一个const对象，只能调用常成员函数

            // C++20可以使用return this->a;
            // C++17只能使用return a;

#if __cplusplus > 201703L
            return this->a;
#else
            return a;
#endif
        };

        auto f2 = [*this]() mutable{
#if __cplusplus > 201703L
            this->SetA(); // ok
            return this->a;
#else
            SetA();
            return a;
#endif

        };

        return f1() == f1() ? 0 : 1;
    }

    void SetA() {
        this->a = 88;
    }
};

class MyClass
{
public:
    MyClass() { 
        s = new MyStruct(); 
    }
    ~MyClass() {
        if (s)
        {
            delete s;
            s = nullptr;
        }
    }
    // 如果捕获*this一定要实现【深拷贝】
    MyClass(const MyClass& obj) {
        this->s = new MyStruct();
        this->s->a = obj.s->a;
    }

    void TestFunc()
    {
        auto lambda1 = [*this]()mutable
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
    void Func1() const{}
    void Func2() {}

private:
    MyStruct* s{ nullptr };
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



#ifdef TEST5

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    {//test1 值捕获
        int i = 1;
        auto f = [i] {return i; }; //等价于 auto f = [i](){return i;};  ()可写可不写
        i = 10;
        int j = f();
        cout << j << endl;  // 1
    }

    {//test2 引用捕获
        int i = 1;
        auto f = [&i] {return i; };
        i = 10;
        int j = f();
        cout << j << endl;  // 10
    }

    {//test3 隐式值捕获
        int i = 1;
        int j = 2;
        auto f = [=] {return i + j; };
        i = 3;
        int m = f();
        cout << m << endl;  //3
    }

    {//test4 隐式引用捕获
        int i = 1;
        int j = 2;
        auto f = [&] {return i + j; };
        i = 3;
        int m = f();
        cout << m << endl;  //5
    }

    {//test5 隐式、显式混合捕获1
        int i = 1;
        int j = 2;
        //i为值捕获，j为引用捕获
        auto f = [=, &j] {return i + j; };
        i = 3;
        j = 4;
        int m = f();
        cout << m << endl;  //5
    }

    {//test5 隐式、显式混合捕获2 
        int i = 1;
        int j = 2;
        //i为引用捕获，j为值捕获
        auto f = [&, j]()->int {return i + j; };  //->int是C++返回值类型后置语法
        i = 3;
        j = 4;
        int m = f();
        cout << m << endl;  //5
    }

    {//test6 可变lambda 1
        int i = 10;
        auto f = [&i]/*()mutable*/ {return ++i; }; // 引用捕获mutable可写可不写
        i = 5;
        int j = f();
        cout << j << endl;  //6
    }

    {//test6 可变lambda 2
        int i = 10;
        auto f = [i]()mutable {return ++i; };  // 值捕获必须有mutable，不然报错，此处不能省略()
        i = 5;
        int j = f();
        cout << j << endl;  //6
    }

    {//值捕获，引用捕获均编译错误，因为i为const
        //const int i = 10;
        //auto f = [&i]() mutable {return ++i; };
        //auto f = [i]() mutable {return ++i; };
        //int j = f();
        //cout << j << endl;
    }

    {// test7 捕获列表和参数列表
     // 参数列表是需要在调用该lambda的时候传入参数，捕获列表是用来声明那些变量可以在lambda内部使用
        int i = 1;
        auto f = [i](int x) {return -(i + x); }; //参数列表：调用时需要传入一个int值，捕获列表：变量i可以在lambda内部使用
        auto j = f(3);
        cout << j << endl;  // -(1+3) = -4
    }

    {//test8 lambda的返回类型
        vector<int> ivec{ -12,2,-22,3,0 };
        //改变ivec里的值，负数变成整数                                                
        //此lambda不写返回类型没有问题.                                               
        transform(ivec.begin(), ivec.end(), ivec.begin(), [](int i) {return i < 0 ? -i : i; });

        //此lambda不写返回类型也没有问题.  
        vector<int> ret;
        ret.resize(ivec.size());
        transform(ivec.begin(), ivec.end(), ret.begin(), [](int i) {if (i < 0) return -i; else return i; });

        for (const auto& s : ret)
        {
            cout << s << " ";
        }
        cout << endl;
    }

    return 0;
}


#endif // TEST5