/*
1. 标准库hash函数
2. std::function 指向成员函数
3. std::mem_fn 从成员指针创建出函数对象,有点类似function，bind
4. 回调函数
5. 
6.
7. reference_wrapper
8. std::move std::forward
9. 
10.函数返回一个对象，编译器优化
11.
12.std::move std::forward 右值，左值
13.std::invoke std::invoke_r https://zh.cppreference.com/w/cpp/utility/functional/invoke
14.为什么要使用std::invoke
15.使用标准库std::hash对自定义类型求hash值
*/
#define TEST1

#ifdef TEST1
// hash example
#include <iostream>
#include <functional>
#include <string>
#include <map>

int main()
{
    int a = 0;
    int b = 1;
    int c = 2;
    std::string type = "fluid";
    std::string mesh = "Flow";
    std::string physics = "press";

    std::hash<std::string> strHash;
    auto ret1 = strHash(std::to_string(a) + type + mesh + physics);
    auto ret2 = strHash(std::to_string(b) + type + mesh + physics);
    auto ret3 = strHash(std::to_string(c) + type + mesh + physics);
    auto ret4 = strHash(std::to_string(c) + mesh + type + physics);
    auto ret5 = strHash(std::to_string(a) + mesh + type + physics);
    auto ret6 = strHash(std::to_string(c) + type + mesh + physics);

    std::cout << (ret1 == ret2) << std::endl;
    std::cout << (ret1 == ret3) << std::endl;
    std::cout << (ret3 == ret4) << std::endl;
    std::cout << (ret4 == ret5) << std::endl;
    std::cout << (ret3 == ret6) << std::endl;

    char nts1[] = "Test";
    char nts2[] = "Test";
    std::string str1(nts1);
    std::string str2(nts2);

    std::hash<char*> ptr_hash;
    std::hash<std::string> str_hash;

    std::cout << "same hashes:\n" << std::boolalpha;
    std::cout << "nts1 and nts2: " << (ptr_hash(nts1) == ptr_hash(nts2)) << '\n';  // 指针是一个地址
    std::cout << "str1 and str2: " << (str_hash(str1) == str_hash(str2)) << '\n';  // string是一个字符串
    std::cout << "str1 and nts2: " << (str_hash(str1) == ptr_hash(nts2)) << '\n';

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <functional>

class Test {
public:
    int fun(int x, int y)
    {
        return x + y;
    }
};

int main()
{
    Test test;
    // 方式1：fun1的类型可以为auto
    std::function<int(int, int)> fun1 = std::bind(&Test::fun, test, std::placeholders::_1, std::placeholders::_2);
    int result1 = fun1(1, 2);

    // 方式2：fun2的类型必须明确指定，不能为auto
    std::function<int(Test, int, int)> fun2 = &Test::fun;
    int result2 = fun2(test, 1, 2);

    return 0;
}

#endif // TEST2

#ifdef TEST3

//std::mem_fn比 std::bind功能更简单，std::bind功能更复杂，如果使用mem_fn能解决问题就不要用std::bind

#include <functional>
#include <iostream>

class Foo
{
public:
    void no_arg()
    {
        std::cout << "Hello, world.\n";
    }
    void has_arg(int i)
    {
        std::cout << "number: " << i << '\n';
    }
    int data = 7;
};

int main()
{
    Foo f;
    // 参数必须为类成员变量或函数
    auto fn1 = std::mem_fn(&Foo::no_arg);
    fn1(&f);
    auto b1 = std::bind(&Foo::no_arg, f);
    b1();

    auto fn2 = std::mem_fn(&Foo::has_arg);
    fn2(&f, 42);
    auto b2 = std::bind(&Foo::has_arg, f, 3);
    b2();

    auto fn3 = std::mem_fn(&Foo::data);
    std::cout << "data: " << fn3(&f) << '\n';

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <functional>

using FuncType = std::function<void(int)>;

void callBack(int i)
{
    std::cout << "callback  " << i << '\n';
}

class A
{
public:
    void callback(int i) {
        std::cout << "class A callback   " << i << "\n";
    }
};

void test(FuncType f)
{
    f(2);
    std::cout << "test\n";
}

int main()
{
    test(callBack);  // 2

    test(std::bind(callBack, 4));  //4

    A a;
    test(std::bind(&A::callback, a, 7));  // 7

    A aa;
    test(std::bind(&A::callback, aa, std::placeholders::_1));  // 2

    return 0;
}

#endif // TEST4




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

#ifdef TEST8

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <memory>
using namespace std;

struct A
{
    A(int&& n)
    {
        cout << "rvalue overload, n=" << n << endl;
    }
    A(int& n)
    {
        cout << "lvalue overload, n=" << n << endl;
    }
};

class B
{
public:
    template<class T1, class T2, class T3>
    B(T1&& t1, T2&& t2, T3&& t3) :
        a1_(std::forward<T1>(t1)),
        a2_(std::forward<T2>(t2)),
        a3_(std::forward<T3>(t3))
    {

    }
private:
    A a1_, a2_, a3_;
};

template <class T, class U>
std::unique_ptr<T> make_unique1(U&& u)
{
    //return std::unique_ptr<T>(new T(std::forward<U>(u)));
    return std::unique_ptr<T>(new T(std::move(u)));
}

template <class T, class... U>
std::unique_ptr<T> make_unique2(U&&... u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)...));
    //return std::unique_ptr<T>(new T(std::move(u)...));
}

int main()
{
    auto p1 = make_unique1<A>(2);

    int i = 10;
    auto p2 = make_unique1<A>(i);

    int j = 100;
    auto p3 = make_unique2<B>(i, 2, j);
    return 0;
}

#endif // TEST8



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


#ifdef TEST12

#include <iostream>

void fun(int&& n)
{
    std::cout << "右值\n";
}

void fun(int& n)
{
    std::cout << "左值\n";
}

int main()
{
    int ln1 = 9;
    int ln2 = 6;
    int ln3 = 3;
    int ln4 = 1;
    int&& rn1 = 8;
    int&& rn2 = 7;
    int&& rn3 = 6;
    int&& rn4 = 5;

    fun(std::move(rn1));         //右值
    fun(std::forward<int>(rn2)); //右值
    fun(std::forward<int&>(rn3));//左值
    std::cout << "===\n";
    fun(std::move(ln1));          //右值
    fun(std::forward<int&>(ln2)); //左值
    fun(std::forward<int>(ln3));  //右值
    std::cout << "***\n";
    fun(2);   // 右值
    fun(ln4); // 左值
    fun(rn4); // 左值

    return 0;
}

#endif // TEST12

#ifdef TEST13

#include <functional>
#include <iostream>
#include <type_traits>

struct Foo {
    Foo(int num) : num_(num) {}
    void print_add(int i) const { std::cout << num_ + i << '\n'; }
    int num_;
};

void print_num(int i)
{
    std::cout << i << '\n';
}

struct PrintNum {
    void operator()(int i) const
    {
        std::cout << i << '\n';
    }
};

int main()
{
    // 调用自由函数
    std::invoke(print_num, -9);

    // 调用 lambda
    std::invoke([]() { print_num(42); });

    // 调用成员函数
    const Foo foo(314159);
    std::invoke(&Foo::print_add, foo, 1);

    // 调用（访问）数据成员
    std::cout << "num_: " << std::invoke(&Foo::num_, foo) << '\n';

    // 调用函数对象
    std::invoke(PrintNum(), 18);

    // c++23
#if defined(__cpp_lib_invoke_r)
    auto add = [](int x, int y) { return x + y; };
    auto ret = std::invoke_r<float>(add, 11, 22);
    static_assert(std::is_same<decltype(ret), float>());
    std::cout << ret << '\n';
    std::invoke_r<void>(print_num, 44);
#endif

}

#endif // TEST13

#ifdef TEST14
#include <iostream>

#define WARP_CALL(fun, ...) fun(__VA_ARGS__)

template <typename Fun, typename...Args>
auto warp_call1(Fun f, Args... args)->decltype(f(args...))
{
    return f(args...); //注意此处args后面的...不要忘记
}

template <typename Fun, typename...Args>
auto warp_call2(Fun&& f, Args&&...args)
{
    // 只是给f和args多了一步完美转发，注意...的位置
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template<typename Fun, typename...Args>
decltype(auto) warp_call3(Fun&& f, Args&&... args)noexcept
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template<typename Fun, typename...Args>
constexpr auto warp_call4(Fun&& f, Args&&... args) noexcept
->decltype(std::forward<Fun>(f)(std::forward<Args>(args)...))
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template<typename Fun, typename...Args>
std::invoke_result_t<Fun, Args...> warp_call5(Fun&& f, Args... args)noexcept
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

int fun(int x, int y)
{
    return x + y;
}

int main()
{
    auto ret1 = WARP_CALL(fun, 2, 2);
    std::cout << "x + y = " << ret1 << std::endl;

    auto ret2 = warp_call1(fun, 2, 4);
    std::cout << "x + y = " << ret2 << std::endl;

    auto ret3 = warp_call2(fun, 1, 4);
    std::cout << "x + y = " << ret3 << std::endl;

    auto ret4 = warp_call3(fun, 4, 4);
    std::cout << "x + y = " << ret4 << std::endl;

    //std::invoke就相当于warp_call4
    auto ret5 = warp_call4(fun, 3, 4);
    std::cout << "x + y = " << ret5 << std::endl;

    auto ret6 = warp_call5(fun, 3, 2);
    std::cout << "x + y = " << ret6 << std::endl;

    return 0;
}
#endif // TEST14

#ifdef TEST15

#include <functional>

class A
{
    friend struct std::hash<A>;
};

namespace std
{
    template<>
    struct hash<A>
    {
        size_t operator()(A const& a) const noexcept { return 1; /*返回哈希值*/ }
    };
}

int main()
{
    A a;
    std::hash<A> s;
    auto hashCode = s(a);
}
#endif // TEST15

