/*
* 1. 函数返回对象，std::move，编译器优化，参数为常引用和引用
* 2. 左值 右值 将亡值
* 3. 模板函数不要使用值传递
* 4. 函数参数为值传递，左值引用传递，右值引用传递，常引用传递  引用塌缩  https://www.qb5200.com/article/295128.html
* 5. 
* 6. 传值传引用 内置类型传值比传引用效率更高 const右值引用  std::ref https://www.cnblogs.com/QG-whz/p/5129173.html
* 7. std::reference_wrapper
* 8.

* 10.函数返回一个对象，编译器优化
*/

#define TEST1

#ifdef TEST1
// g++ demo.cpp -o out -std=c++11 -fno-elide-constructors

#ifdef __GNUC__
#pragma GCC diagnostic push
// 关闭警告:
// parentheses were disambiguated as a function declaration
// 括号作为函数声明已消除歧义
#pragma GCC diagnostic ignored "-Wvexing-parse"
// 关闭警告:
// moving a local object in a return statement prevents copy elision
// 在return语句中移动本地对象可防止复制省略
#pragma GCC diagnostic ignored "-Wpessimizing-move"
#endif // __GNUC__

#include <iostream>
#include <type_traits>
#include <functional>

class Test
{
public:
    Test() { std::cout << " 111 construct\n"; }
    ~Test() { std::cout << " --- construct\n"; }

    Test& operator=(const Test&)noexcept { std::cout << " 222 operator=\n"; return *this; }
    Test& operator=(const Test&&)noexcept { std::cout << " 888 operator=\n"; return *this; }
    Test& operator=(Test&&)noexcept { std::cout << " 333 operator=\n"; return *this; }
    Test& operator=(Test&)noexcept { std::cout << " 999 operator=\n"; return *this; }

    Test(const Test&) { std::cout << " 444 copy construct\n"; }
    Test(const Test&&)noexcept { std::cout << " 555 move construct\n"; }
    Test(Test&&)noexcept { std::cout << " 666 move construct\n"; }
    Test(Test&)noexcept { std::cout << " 777 copy construct\n"; }
};

Test GetTestObj1()
{
    // vs2019: 普通构造 111
    // g++取消优化: 111 666 666
    // g++默认: 111
    return Test();
}

Test GetTestObj2()
{
    // vs2019: 普通构造 + 移动构造  111 + 666
    // g++取消优化: 111 666 666
    // g++默认: 111
    Test t;
    return t;
}

Test GetTestObj3()
{
    // https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-return-ref-ref
    // 不要返回&&
    // vs2019: 普通构造 + 移动构造  111 + 666
    // g++取消优化: 111 666 666
    // g++默认: 111 666
    Test t;
    return std::move(t); // 返回局部变量会有警告
}

Test GetTestObj4()
{
    // vs2019: 普通构造 + 拷贝构造  111 + 777
    // g++取消优化: 111 777 666
    // g++默认: 111 777
    Test t;
    return std::ref(t); // g++ std::ref需要包含functional头文件
}

Test GetTestObj5()
{
    // vs2019: 普通构造 111
    // g++取消优化: 111 666
    // g++默认: 111
    return {};
}

int main()
{
    {
        auto ret = GetTestObj1();
    }
    std::cout << "=================\n";

    {
        auto ret = GetTestObj2();
    }
    std::cout << "=================\n";

    {
        auto ret = GetTestObj3();
    }
    std::cout << "=================\n";

    {
        auto ret = GetTestObj4();
    }
    std::cout << "=================\n";

    {
        auto ret = GetTestObj5();
    }
    std::cout << "*****************************\n";

    {
        Test t1, t2, t3;
        t1 = Test(); // 333 右值引用赋值
        t2 = t3;     // 999 左值引用赋值
    }
    std::cout << "=================\n";

    {
        const Test t1;
        Test t2;
        //t1 = t2; // error
        t2 = t1;   // 222 常引用赋值（左值）
    }
    std::cout << "=================\n";

    {
        const Test t1;
        Test t2(t1); // 444 常引用拷贝构造函数
    }
    std::cout << "=================\n";

    {
        Test t2(const Test()); // 不会输出任何内容
    }
    std::cout << "=================\n";

    {
        const Test t1;
        Test t2(std::move(t1));  // // 555 移动构造函数 会报警告：不要对常量使用std::move
    }
    std::cout << "=================\n";

    return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

#endif // TEST1


#ifdef TEST3

#include <iostream>

// 正确析构
template<typename T>
void Func1(T&& t)
{
    std::cout << "111\n";
}

// 正确析构
template<typename T>
void Func2(T& t)
{
    std::cout << "222\n";
}

// 如果T没有拷贝构造函数就会崩溃，因为值传递会有一次参数拷贝，参数拷贝时需要有拷贝构造函数
// 函数执行完，会调用一次t的析构函数，main函数结束的时候也会调用析构，如果参数对应的类没有拷贝构造函数
// 就只new了一次，但是析构两次调用两次delete就会崩溃
template<typename T>
void Func3(T t)
{
    std::cout << "333\n";
}

// 正确析构
template<typename T>
void Func4(const T& t)
{
    std::cout << "444\n";
}

class A
{
public:
    A() {
        m_aa = new int[10]();
        std::cout << "construct\n";
    }
    A(const A& a) {
        m_aa = new int[10]();
        for (size_t i = 0; i < 10; ++i)
        {
            m_aa[i] = a.m_aa[i];
        }
        std::cout << "copy construct\n";
    }
    ~A() {
        if (m_aa)
        {

            delete m_aa;
            m_aa = nullptr;
            std::cout << "destruct\n";
        }
    }
private:
    int* m_aa{ nullptr };
};

int main()
{
    A a;
    Func1(a);
    Func2(a);
    // 执行函数Func3函数体之前会调用拷贝构造函数
    Func3(a);
    // 执行完Func3之后，就会调用一次析构函数
    Func4(a);

    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <iostream>

// Test1相比于其他三个要多一次构造（拷贝构造）

// 值传递，会有拷贝消耗，对传入的参数不能修改（内部修改的只是拷贝后的值，外部的变量没有改变）
template<class T>
T* Test1(T arg)
{
    return new T(arg); // new 出来的没有delete就会少一次析构，new调用的拷贝构造函数
}

// 左值引用，不能传常参数等右值
template<class T>
T* Test2(T& arg)
{
    return new T(arg);
}

// 常引用，左值右值都可以传，但是不能对传入的参数进行修改
template<class T>
T* Test3(const T& arg)
{
    return new T(arg);
}

// 万能引用，可以解决参数完美转发的问题 关于万能引用请看00_13_TEST12
// 引用塌缩
template<class T>
T* Test4(T&& arg)
{
    return new T(std::forward<T&&>(arg));
}

class A
{
public:
    A() {
        std::cout << "construct\n";
    }
    A(const A& a) {
        std::cout << "copy construct\n";
    }
    A(const A&& a) noexcept {
        std::cout << "move construct\n";
    }
    ~A() {
        std::cout << "destruct\n";
    }
};

int main()
{
    std::cout << "0-------------------\n";
    {
        A a;
        auto ret = Test1(a);
        //delete ret; // 此处如果不delete就会少一次析构，后面的例子也一样
        //ret = nullptr;
    }
    std::cout << "1-------------------\n";
    {
        A a;
        auto ret = Test2(a);
    }
    std::cout << "2-------------------\n";
    {
        A a;
        auto ret = Test3(a);
    }
    std::cout << "3-------------------\n";
    {
        A a;
        //auto ret1 = Test4(a); // 参数类型错误
        auto ret2 = Test4(std::move(a));
    }
    std::cout << "4-------------------\n";
    {
        auto ret = Test4(A());
    }
    std::cout << "5-------------------\n";

    return 0;
}

#endif // TEST4



#ifdef TEST6

// 什么时候用值传递，什么时候用引用传递
// https://cplusplus.com/articles/z6vU7k9E/

#include <iostream>

// [1]
//void Func(int x)
//{
//    x++;
//    std::cout << "int\n";
//}

// [2]
//void Func(const int x)
//{
//    //x++;
//    std::cout << "const int\n";
//}

// [3]
void Func(int& x)
{
    x++;
    std::cout << "int&\n";
}

// [4]
void Func(const int& x)
{
    //x++;
    std::cout << "const int&\n";
}

// [5]
void Func(int&& x)
{
    x++;
    std::cout << "int&&\n";
}

// [6]
void Func(const int&& x)
{
    //x++;
    std::cout << "const int&&\n";
}

int main()
{
    int v1 = 1;
    const int v2 = 1;

    Func(v1);
    Func(1);
    Func(v2);
    Func((const int&&)v1);
}

#endif // TEST6

#ifdef TEST7

//https://www.cnblogs.com/jerry-fuyi/p/12747850.html
//reference_wrapper 和原生指针的区别？？？
//reference_wrapper 绑定到需要通过引用传递参数的函数时非常有用
#include <iostream>
#include <functional>
using namespace std;

void func(int a, int b)
{
    cout << "a = " << a << ",";
    cout << "b = " << b << endl;
}

void func1(int i)
{
    cout << "i = " << i << endl;
}

int main()
{
    // 包裹函数指针
    std::reference_wrapper<void(int, int)> f0 = func;
    f0(5, 7);

    std::function<void(int, int)> f4 = func;
    f4(3, 4);

    //std::ref返回一个reference_wrapper包裹的对象
    auto f1 = std::ref(func);
    f1(8, 9);

    //std::add_rvalue_reference

     // 和bind结合使用
    int i = 10, j = 3;
    auto f2 = std::bind(func1, i);
    auto f3 = std::bind(func1, std::ref(i));
    std::reference_wrapper<int> r_val = j;
    auto f5 = std::bind(func1, r_val);
    i = 30; j = 40;

    f2();
    f3();
    f5();


    return 0;
}
#endif // TEST7


#ifdef TEST9

void fun(int& x) {
    //
}

int main() {
    //fun(10);
    return 0;
}

#endif // TEST9


#ifdef TEST10
// g++ demo.cpp -o demo.exe -std=c++0x -fno-elide-constructors

#include <iostream>

template <typename T>
class MyVector
{
public:
    MyVector() :data(nullptr) { std::cout << "default\n"; } //无参构造
    explicit MyVector(T t) :data(new T(t)), size(1) { std::cout << "only one\n"; } //有参构造
    MyVector(MyVector<T>& t) {
        std::cout << "copy\n";
        if (t.size <= 0) {
            size = 0;
            data = nullptr;
        }
        else if (t.size == 1) {
            data = new T(*t.data);
            size = 1;
        }
        else {
            data = new T[t.size]();
            data = t.data;
            size = t.size;
        }
    }

    explicit MyVector(MyVector<T>&& t) { // 移动构造
        std::cout << "move\n";
        if (t.size <= 0) {
            size = 0;
            data = nullptr;
        }
        else if (t.size == 1) {
            data = t.data;
            size = 1;

            delete t.data;
            t.data = nullptr;
        }
        else {
            data = t.data;
            size = t.size;

            delete[] t.data;
            t.data = nullptr;
        }
    }

    MyVector(std::initializer_list<T> init_list) { // 初始化列表构造
        std::cout << "init list\n";
        data = new T[init_list.size()]();
        size = init_list.size();
        size_t index = 0;
        for (auto elem : init_list)
        {
            data[index++] = elem;
            if (index >= size)
                break;
        }
    }

    const MyVector<T>& operator=(const MyVector<T>& t) { //拷贝赋值
        std::cout << "assign\n";
        if (t.size <= 0) {
            size = 0;
            data = nullptr;
        }
        else if (t.size == 1) {
            data = new T(*t.data);
            size = 1;
        }
        else {
            data = new T[t.size]();
            data = t.data;
            size = t.size;
        }
        return *this;
    }

    const MyVector<T>& operator=(MyVector<T>&& t) noexcept { // 移动赋值
        std::cout << "move assign\n";
        if (t.size <= 0) {
            size = 0;
            data = nullptr;
        }
        else if (t.size == 1) {
            data = t.data;
            size = 1;

            delete t.data;
            t.data = nullptr;
        }
        else {
            data = t.data;
            size = t.size;

            delete[] t.data;
            t.data = nullptr;
        }
        return *this;
    }

    ~MyVector() {
        std::cout << "destruct\n";
        if (data)
        {
            if (size > 1)
                delete[] data;
            else
                delete data;

            data = nullptr;
        }
    }

private:
    T* data{ nullptr };
    size_t size{ 0 };
};

MyVector<int> getVec1()
{
    return {};  //default
}

MyVector<int> getVec2()
{
    return MyVector<int>();
    //default   MyVector<int>()
    //move      return
    //destruct  析构MyVector<int>()产生的匿名对象
}

MyVector<int> getVec3()
{
    return std::move(MyVector<int>());
    //destruct
    //move
    //destruct
}

int main()
{
    {
        MyVector<int> vec1; // 无参构造
        MyVector<int> vec2{ 1,2,3,4 }; //初始化列表构造
        MyVector<int> vec3(2); //有参构造
        MyVector<int> vec4 = MyVector<int>(); //无参构造 移动构造 析构
        MyVector<int> vec5(vec4); //拷贝构造
        MyVector<int> vec6 = std::move(vec5);
        vec1 = std::move(vec6);
        std::cout << "11111111\n";
    }
    std::cout << "========\n";
    {
        auto ret1 = getVec1();     //move destruct 将get返回的对象移动到ret1，然后析构get返回的匿名对象
        std::cout << "22222222\n";
        auto ret2 = getVec2();  //move destruct
        std::cout << "33333333\n";
        auto ret3 = getVec3();  //move destruct
        std::cout << "44444444\n";
    }

    return 0;
}

#endif // TEST10