/*
* 1.重载bool(),可以对指定类型进行if(Obj)操作
* 2.模板类重载流操作符
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
    //friend std::ostream& operator<<(std::ostream& out, A& a);

    // 3 error
    //std::ostream& operator<< (std::ostream& out) { out << "test" << std::endl; return out; }
    // 4 error
    //std::ostream& operator<< (std::ostream& out,A& a) { out << "test" << std::endl; return out; }
};

// 2
//std::ostream& operator<< (std::ostream& out, A& a)
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
