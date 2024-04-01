/*
 * 1. 重载bool(),可以对指定类型进行if(Obj)操作
 * 2. 模板类重载流操作符
 * 3. 成员函数的引用饰词
 * 4. final修饰类、虚成员函数
 */

#define TEST4

#ifdef TEST1

#include <iostream>

class A
{
public:
    explicit operator bool() const noexcept
    {
        return false;
    }
};

int main()
{
    A a;
    if (!a)
    {
        std::cout << "class A::operator bool() return false\n";
    }
}
#endif // TEST1

#ifdef TEST2

#include <iostream>

class A
{
public:
    // 1
    friend std::ostream& operator<<(std::ostream& out, A& a)
    {
        out << "AAA";
        return out;
    }

    // 2
    // friend std::ostream& operator<<(std::ostream& out, A& a);

    // 3 error
    // std::ostream& operator<< (std::ostream& out) { out << "test" << std::endl; return out; }
    // 4 error
    // std::ostream& operator<< (std::ostream& out,A& a) { out << "test" << std::endl; return out; }
};

// 2
// std::ostream& operator<< (std::ostream& out, A& a)
//{
//    out << "test";
//    return out;
//}

int main()
{
    A a;
    std::cout << a;
}
#endif // TEST2

#ifdef TEST3

#include <iostream>
class Test
{
public:
    // 仅在*this是左值时调用
    void Func() &
    {
        std::cout << "void Test::Func()&\n";
    }
    // 仅在*this是右值时调用
    void Func() &&
    {
        std::cout << "void Test::Func()&&\n";
    }

    // *this为左值或右值都可以调用
    void Func2()
    {
        std::cout << "void Test::Func2()\n";
    }
};

// 返回Test型别的右值
Test MakeTest()
{
    return Test {};
}

int main()
{
    Test t;
    t.Func();
    t.Func2();

    MakeTest().Func();
    MakeTest().Func2();

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>

class Base
{
public:
    virtual void Func1()
    {
        std::cout << "Base::Func1()\n";
    }

    virtual void Func2() final
    {
        std::cout << "Base::Func1()\n";
    }
    // error final只能修饰虚函数
    // void Func3() final
    // {
    //     std::cout << "Base::Func3()\n";
    // }
};

class Derived : public Base
{
public:
    void Func1() override
    {
        std::cout << "Derived::Func1()\n";
    }
    // error 基类中的Func2被final修饰，子类不能重写
    // void Func2()
    // {
    //     std::cout << "Derived::Func2()\n";
    // }
    // error
    // void Func2() override
    // {
    //     std::cout << "Derived::Func2()\n";
    // }
};

int main()
{
    Base* p = new Derived();
    p->Func1();
    p->Func2();

    return 0;
}

#endif // TEST4
