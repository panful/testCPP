/*
* 1. 匿名空间和static
* 2. 函数内部局部静态变量
* 3. 
*/


#define TEST3

#ifdef TEST1

#include "header1.h"
#include <iostream>

//int myStaticInt = 1;  // “myStaticInt” : 重定义；多次初始化
//int anonymousInt = 2; // 可以定义，不使用就不会报错，但是如果使用了就会报错：anonymousInt不明确

void Func1()
{
    static int FuncStaticInt = 4;
    std::cout << "Func1\tFuncStaticInt:\t" << &FuncStaticInt << std::endl;

    int generalInt = 0;
    std::cout << "Func1\tgeneralInt:\t" << &generalInt << std::endl;
}

void Func2()
{
    static int FuncStaticInt = 5; // 存储在静态存储区，整个程序运行期间都会存在，所以地址不一样，只是在别的作用域不能访问，即作用域仍为局部作用域
    std::cout << "Func2\tFuncStaticInt:\t" << &FuncStaticInt << std::endl;

    int generalInt = 0; // 和Func1中的地址一样，因为存储在栈区，函数执行完就会释放该空间
    std::cout << "Func2\tgeneralInt:\t" << &generalInt << std::endl;
}

void Func3()
{
    //std::cout << FuncStaticInt << std::endl;  // error
}

int main()
{
    Func1();
    Func2();

    std::cout << anonymousInt << std::endl;

    //std::cout << namedInt << std::endl;  // error
    std::cout << MyName::namedInt << std::endl;

    std::cout << myStaticInt << std::endl;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

class A
{
public:
    A() { std::cout << "construct\n"; }
    ~A() { std::cout << "destruct\n"; }
};

void Func()
{
    // a是一个局部静态变量，存储期为静态存储区，生存期为整个源程序
    static int a = 0;
    std::cout << a++ << ',';

    // 离开Func()函数的作用域后，并不会析构aa，整个程序结束时才会析构
    static A aa;
}

int main()
{
    // 打印0,1,2,3,4
    Func();
    Func();
    Func();
    Func();
    Func();
}

#endif // TEST2

#ifdef TEST3
int main()
{

}

#endif // TEST3

