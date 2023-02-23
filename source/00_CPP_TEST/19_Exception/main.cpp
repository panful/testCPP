/*
* 1. 定义一个抛出异常的宏
* 2. noexcept
*/

//
// vs中EHa/EHs/EHsc的区别 https://blog.csdn.net/dan452819043/article/details/115340141
// /EHa能让catch(…)捕获结构化异常，能保证结构化异常或者C++异常发生的时候局部对象的析构函数被调用，而且假设extern "C"函数可能会抛出C++异常。
// /EHs无法让catch(…)捕获结构化异常，而且只能保证C++异常抛出的时候局部对象的析构函数被调用，而且假设extern "C"函数可能会抛出C++异常。
// /EHsc无法让catch(…)捕获结构化异常，而且只能保证C++异常抛出的时候局部对象的析构函数被调用，而且假设extern "C"函数不会抛出C++异常。
//
// CMake中通过 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}/EHsc")设置编译选项
//
#define TEST2

#ifdef TEST1

#include <iostream>
#include <exception>
#include <string>
#include <type_traits>
#include <functional>
#include <string>

#define ThrowError(_error_) throw(_error_.Information())

class Error
{
public:
    Error(const std::string& _file, const std::string& _line, const std::string& _method, const std::string& _message):
        file(_file),
        line(_line),
        method(_method),
        message(_message)
    {}

    std::string Information()const
    {
        return 
            "----------------------------\n"
            "File\t\t: " + file + '\n' +
            "Line\t\t: " + line + '\n' +
            "Method\t\t: " + method + '\n' +
            "Message\t\t: " + message + '\n' +
            "----------------------------\n";
    }
private:
    std::string file;
    std::string line;
    std::string method;
    std::string message;
};

int main()
{
    {
        try
        {
            ThrowError(Error("file", "1234", "method", "An error"));
        }
        catch (std::string s)
        {
            std::cout << "catch : \n" << s << std::endl;
        }
    }

    {
        try
        {
            ThrowError(Error(__FILE__, std::to_string(__LINE__), __FUNCTION__, "throw an error"));
        }
        catch (std::string s)
        {
            std::cout << "catch : \n" << s << std::endl;
        }
    }

    return 0;
}

#endif // TEST1


#ifdef TEST2

// 关闭函数使用了noexpect内部还抛出异常触发的警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"

#include <iostream>

void func1()
{
    std::cout << "func1\n";
}

void func2()
{
    std::cout << "func2\n";
    throw("func2");
}

// noexcept修饰的函数，如果内部抛出异常，程序直接调用abort()终止运行，异常不会被捕获
// 如果函数声明为noexcept，但是函数内部还抛出了异常编译器会有警告（使用#pragma可以关闭该警告）
void func3() noexcept
{
    std::cout << "func3\n";
    throw("func3");
}

// 参数使用true和不使用任何参数效果一样
void func4() noexcept(true)
{
    std::cout << "func4\n";
    throw("func4");
}

// 参数为false表示异常可发生也可不发生
void func5() noexcept(false)
{
    std::cout << "func5\n";
    throw("func5");
}

// 如果父类的【纯虚函数】是被标记了noexcept，那么子类必须需要标记为对应的noexcept，
// 如果父类未标记，那么子类可标记或不标记
class Parent
{
public:
    virtual void Func1() const = 0;
    virtual void Func2() noexcept = 0;
};
class Child :public Parent
{
public:
    void Func1() noexcept {};
    void Func2() noexcept override {};
};

typedef void (*FuncPointer1)();
typedef void (*FuncPointer2)() noexcept;

// c++标准库中，其对operator new或者operator new[] 的标记便是可能会抛出异常，
// 因为可能内存不够用(即使申请的内存小于总内存，可能因为内存碎片化，已经给不出申请的内存大小了，会抛出异常)。
// 类中的析构函数是不应该抛出异常的，因此c++对于operator delete或者operator delete[] 都是标记为不应该抛出异常的。

int main()
{
    // 函数指针及该指针所指向的函数必须有一致的异常说明。
    // 简单来说，如果某个函数指针指定了不抛出异常的声明，那么其只能指向不抛出异常的函数。
    // 如果该函数指针未做声明，或者声明了可能抛出异常，那该函数指针可以指向任意函数。
    {
        FuncPointer1 f1 = func1;
        FuncPointer1 f3 = func3;
        //FuncPointer2 f2 = func2; // error FuncPointer2是noexcept，而func2可以抛出异常
        FuncPointer2 f4 = func3;
    }

    try
    {
        func1();
    }
    catch (...)
    {
        std::cout << "catch an exception\n";
    }

    std::cout << "---------------------\n";
    try
    {
        func2();
    }
    catch (...)
    {
        std::cout << "catch an exception\n";
    }

    std::cout << "---------------------\n";
    try
    {
        func3();
    }
    catch (...)
    {
        std::cout << "catch an exception\n";
    }

    std::cout << "---------------------\n";
    try
    {
        //func4();
    }
    catch (...)
    {
        std::cout << "catch an exception\n";
    }

    std::cout << "---------------------\n";
    try
    {
        func5();
    }
    catch (...)
    {
        std::cout << "catch an exception\n";
    }

    return 0;
}

#pragma GCC diagnostic pop

#endif // TEST2
