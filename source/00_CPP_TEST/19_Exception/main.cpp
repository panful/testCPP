/*
* 1. 定义一个抛出异常的宏
*/

#define TEST1

#ifdef TEST1

#include <iostream>
#include <exception>
#include <string>

#define ThrowError(Error) throw(Error.message)

class Error
{
public:
    Error(const std::string& _file, const std::string& _fun, const std::string& _method, const std::string& _message)
        :file(_file),
        fun(_fun),
        method(_method),
        message(_message)
    {}
private:
    std::string file;
    std::string fun;
    std::string method;
    std::string message;
};

int main()
{
    {
        try
        {
            ThrowError(Error("file", "fun", "method", "An error"));
        }
        catch (std::string s)
        {
            std::cout << s << std::endl;
        }
    }

    {
        try
        {
            ThrowError(Error(__FILE__, __LINE__, __FUNCTION__, "An error"));
        }
        catch (std::string s)
        {
            std::cout << s << std::endl;
        }
    }
    return 0;
}

#endif // TEST1
