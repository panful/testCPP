/*
* 1.重载bool(),可以对指定类型进行if(Obj)操作
*/

#define TEST1

#ifdef TEST1

#include <iostream>

class A
{
public:
    explicit operator bool()const noexcept 
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