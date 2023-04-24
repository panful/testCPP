/*
1. 模板函数在cpp文件实现
2. 变量模板
3. 模板的别名，特化模板的别名（模板别名和别名模板啥关系？？？什么联系？？？）
4. 模板特化
5. 类模板，类成员函数的模板（类一个，函数一个，共两个模板）
6. bool类型的值作为模板参数（即true或fasle），std::true_type,std::false_type
7. 默认模板参数
8. extern template，代码暂时编译不过去
9. 模板显示实例化
10.通过模板自定义一个栈
11.
12.变参模板 std::integer_sequence  01_07  https://blog.csdn.net/baidu_41388533/article/details/109716432
*/



// 预处理 -> 编译 -> 汇编 -> 链接
//

#define TEST2

#ifdef TEST1

#include "temp.h"
#include <iostream>

// 编译器并不是把函数模板处理成能够处理任意类型的函数；编译器从函数模板通过具体类型产生不同的函数；

// 编译器会对函数模板进行两次编译：
// 1. 在声明的地方对模板代码本身进行编译，
// 2. 在调用的地方对参数替换后的代码进行编译。

//这是因为函数模板要被实例化后才能成为真正的函数，在使用函数模板的源文件中包含函数模板的头文件，
// 如果该头文件中只有声明，没有定义，那编译器无法实例化该模板，最终导致链接错误。
// 模板可以重载返回值, 函数重载不行，如果我们试图通过在头文件中定义函数模板, 
// 在cpp文件中实现函数模板, 那么我们必须在在实现的那个cpp文件中手动实例化, 
// 也就是使用你需要使用的参数替换模板, 从而使得编译器为你编译生成相应参数的模板函数.

int main()
{
    // AddFunc在cpp文件中实现，模板找不到，报错：无法解析的外部符号
    auto ret1 = AddFunc(1, 2);
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

template<class T>
constexpr T pi = T(3.1415926535897932385L);

int main() 
{
    std::cout << pi<int> << std::endl;    // 3
    std::cout << pi<double> << std::endl; // 3.14159
    return 0;
}
#endif // TEST2

#ifdef TEST3

#include <iostream>
// 1
template <typename T>
struct MyStruct
{
    using type = T;
};

template <>
struct MyStruct<int>
{
    using type = char;
};

template <typename T>
using TT1 = MyStruct<T>;

// 2
template <typename T>
struct MyStruct2
{
    using type = T;

    template <typename U>
    using rebind = MyStruct2<U>; // 注意此处的实例化
};

template <typename T>
using TT2 = MyStruct2<T>;

// 3
template<typename T = int>
struct A
{
    A(T = T{}) { std::cout << typeid(T).name() << '\n'; }
};

template <typename T>
using AA = A<T>;

int main()
{
    // 1
    MyStruct<int>::type i = 'c';     //char
    MyStruct<float>::type f = 0.0f;   //float

    TT1<int>::type ii = 'c';  //char
    TT1<float>::type c = 1.0f; //float

    // 2
    TT2<int>::type iii = 9;  //int 

    MyStruct2<int>::rebind<char>::type t1;  //char
    MyStruct2<int>::rebind<char>::rebind<float>::type t2;  //float，最后一个生效
    MyStruct2<int>::type t3;  //int

    // 3
    A a1;  //int
    A<float> a2; //float

    using AAA = A<>;
    //using AAA = A; //错误

    AAA a3;  //int
    //AAA<float> a1; //错误，AAA的模板参数只能是默认的int

    AA<float> a4;  //float

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <vector>

template <typename T>
struct MyStruct
{
    // 模板参数为非指针类型实例化使用该版本
    T i;
};

template <typename T>
struct MyStruct<T*>  //特化
{
    // 模板实例化的参数为指针就会调用该模板
    int i;
};

template <typename T>
void test(T* p) { std::cout << "ptr" << '\n'; }

template <typename T>
void test(const T* p) { std::cout << "const" << '\n'; }

int main()
{
    MyStruct<char> s0;  //char
    s0.i = 'c';

    MyStruct<char*> s1;
    auto x1 = s1.i = 0;  //int

    MyStruct<std::vector<int>::iterator*> s2;
    auto x2 = s2.i = 0;  //int

    using charPtr = char*;
    MyStruct<charPtr> s3;
    auto x3 = s3.i = 0;  //int

    int x = 0;
    const int* p = nullptr;

    test<int>(&x); //模板参数为int，函数参数为int*
    test(p);

    return 0;
}


#endif // TEST4

#ifdef TEST5

#include <iostream>

template <typename T0>
class Test
{
public:
    template <typename T1>
    void display() {
        std::cout << "type0:" << typeid(T0).name() << "\ntype1:" << typeid(T1).name() << '\n';
    }
};

int main()
{
    Test<float> a;
    a.display<int>();

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <type_traits>

template <typename T>
void fun_bool(T& val, std::true_type)
{
    std::cout << "val:" << val << "\ttrue\n";
}

template <typename T>
void fun_bool(T& val, std::false_type)
{
    std::cout << "val:" << val << "\tfalse\n";
}

template <typename T>
void fun(const T& val)
{
    //如果T是指针类型调用true，否则调用false
    fun_bool(val, std::is_pointer<T>());
}

template <bool b>
void test()
{
    if (b)
        std::cout << "true\n";
    else
        std::cout << "false\n";
}

int main()
{
    int i = 1;
    fun(&i); //true
    fun(2);  //false

    test<true>();   //test只能用bool类型的值实例化
    //test<int>();  //错误
    //test();       //错误

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>

template <typename T = int>
class MyClass
{
public:
    void display() { std::cout << typeid(T).name() << '\n'; }
};

template <typename T = float>
void test() { std::cout << typeid(T).name() << '\n'; }

int main()
{
    MyClass c1;  //使用默认的类型int初始化模板参数
    c1.display();

    MyClass<char> c2;
    c2.display();

    test();  //使用默认的参数
    test<char>();

    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <iostream>

template <typename T>
class Test
{
public:
    void fun(T val) { std::cout << val << '\n'; }
};

int main()
{
    extern template class Test;

    Test<int> t2;
    t2.fun(2);

    int i = 0;
    return 0;
}

#endif // TEST8

#ifdef TEST9

#include <iostream>

template <typename T>
class MyClass
{
public:
    //MyClass() = default;
    T value;
};

template <typename T>
void test(T val1, T val2)
{
    std::cout << val1 + val2 << '\n';
}

template void test<float>(float val1, float val2);  //函数的显示实例化

//template class MyClass<int>;                      //类的显示实例化
template<> class MyClass<int>
{
public:
    int value;
};

int main()
{
    test(1, 2);//使用隐式实例化
    test(1.0f, 2.2f);

    MyClass a;
    //std::cout << typeid(a.value).name() << '\n';

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <iostream>
#include <vector>
template <typename T>
class MyStack
{
private:
    std::vector<T> elems;
public:
    void push(T const&);
    void pop();
    T top()const;
};

template <typename T>
void MyStack<T>::push(T const& e)
{
    elems.emplace_back(e);
}

template <typename T>
void MyStack<T>::pop()
{
    if (!elems.empty())
        elems.pop_back();
    else
        throw std::out_of_range("out_of_range");
}
template <typename T>
T MyStack<T>::top()const
{
    if (elems.empty())
        throw std::out_of_range("out_of_range");
    return elems.back();
}

int main()
{
    try
    {
        MyStack<int> s1;
        s1.push(1);
        s1.push(2);
        s1.push(3);
        std::cout << s1.top() << std::endl;  //3
        s1.pop();
        std::cout << s1.top() << std::endl;  //2

        MyStack<std::string> s2;
        s2.push("string1");
        s2.push("string2");
        s2.push("string3");
        std::cout << s2.top() << std::endl;  //string3
        s2.pop();
        std::cout << s2.top() << std::endl;  //string2
        s2.pop();
        std::cout << s2.top() << std::endl;  //string1
        s2.pop();
        s2.pop();  //此时栈顶没有元素，抛出异常:out_of_range

    }
    catch (std::exception const& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
#endif // TEST10


