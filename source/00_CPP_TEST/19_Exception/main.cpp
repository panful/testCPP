/*
* 1. 定义一个抛出异常的宏
* 2. noexcept
*/

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

// noexcept修饰的函数，如果内部抛出异常，程序直接调用abort终止运行，异常不会被捕获
void func3() noexcept
{
    std::cout << "func3\n";
    throw("func3");
}

int main()
{
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

    return 0;
}

#endif // 
