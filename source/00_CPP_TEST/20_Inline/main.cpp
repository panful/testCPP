
/*
* 1. 虚函数声明为inline
* 2. C++17支持内联变量
*/

#define TEST2

#ifdef TEST1

// 虚函数被声明为inline时，是否真正被inline了
// https://www.cnblogs.com/helloweworld/archive/2013/06/03/3116069.html

class A
{
public:
    inline virtual void Func() {}
};

int main()
{

}
#endif // TEST1

#ifdef TEST2

// C++17前只有内联函数，现在有了内联变量，我们印象中C++类的静态成员变量在头文件中是不能初始化的，但是有了内联变量，就可以达到此目的

// header file
struct A {
    static const int value;
};
inline int const A::value = 10;

// ==========或者========
struct B {
    inline static const int value = 10;
};

int main()
{
    A a;
    B b;
}

#endif // TEST2

