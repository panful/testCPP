
// boost function_traits type_traits
// https://blog.csdn.net/luoqie123/article/details/52058354

/*
* 1. type_traits常用模板，类型判断
* 2. decltype的使用
* 3.
* 4. 模板可变参数打印 std::index_sequence  00_13_TEST16
* 5. function_traits 获取函数返回值类型，函数参数类型及个数
* 6. 实现编译期判断一个类是不是另一个类的子类
* 7. std::iterator_traits，STL源码剖析，解引用，即获取指针指向值的类型
*/

#define TEST2

#ifdef TEST1

#include <array>
#include <iostream>
#include <type_traits>

class A {};

int main()
{
    std::array<int, 3> arr;
    std::cout << std::boolalpha;
    std::cout << std::is_array<A>::value << '\n';
    std::cout << std::is_array<A[]>::value << '\n';
    std::cout << std::is_array<A[3]>::value << '\n';
    std::cout << std::is_array<float>::value << '\n';
    std::cout << std::is_array<int>::value << '\n';
    std::cout << std::is_array<int[]>::value << '\n';
    std::cout << std::is_array<int[3]>::value << '\n';
    std::cout << std::is_array<std::array<int, 3>>::value << '\n';
    std::cout << std::is_array<decltype(arr)>::value;
}
#endif // TEST1

#ifdef TEST2

// https://blog.csdn.net/u014609638/article/details/106987131
// 不光变量有类型，函数有类型，表达式也有类型

#include <iostream>
#include <vector>

int TestFunc(int, int&, int*) { std::cout << "The function TestFunc is called\n"; return 0; }
class TestClass {};

int main()
{
    // var
    {
        // decltype(x); 当x是变量的时候，返回x的类型
        int a = 1, & b = a;
        decltype(a) x;     // int x;
        decltype(b) y = a; // int& y;所以y必须使用左值初始化
        //decltype(999) y;   // error 
    }

    // expr
    {
        // decltype(x); 当x是表达式的时候，返回的是表达式x结果的类型。表达式的结果是左值，返回的是引用，右值返回的是类型
        int a = 1, * p = &a, & r = a;
        decltype(a + 1) x;  // int x;
        decltype(p) y;      // int* y;
        decltype(*p) z = a; // int& z; // p指向的对象是一个左值
        decltype((a)) m = a;// int& m; // 加了括号该变量就成为一个表达式，该表达式返回的是一个左值（因为该表达式的结果可以作为赋值语句的左侧的值，注意区分其他表达式返回的值为右值）
        decltype(a) n;      // int n;  // 注意带括号和不带括号的区别

        decltype((p)) u = p;// int* &u;// (p)是左值，所以u是一个引用，因此u是指向int类型的指针的引用
    }

    // func
    {
        // 声明一个函数类型
        using FuncType = int(int, int&, int*);

        // 声明一个FuncType类型的函数指针，并使用函数TestFunc初始化
        FuncType* func = TestFunc; // 等价于 int(*func)(int,int&,int*); func = TestFunc;

        decltype(func) x;      // x是一个函数指针，和func,z类型一样
        decltype(TestFunc) y;  // y是一个类型为int(int,int&,int*)的函数
        decltype(TestFunc)* z; // z是一个函数指针，和func,x类型一样, int(*z)(int,int&,int*);

        int a = 1, & b = a, * c = &a;
        decltype(TestFunc(a, b, c)) w;   // int w;  TestFunc函数返回值为int型的右值，此处不会调用函数TestFunc
        decltype((TestFunc(a, b, c))) v; // int v;  表达式(TestFunc(a,b,c))函数返回值为int型的右值，此处也不会调用TestFunc
    }

    // class object
    {
        // decltype(x); x不能是类型
        // decltype(int); // error
        // decltype(TestClass) x; // error 

        TestClass t;
        decltype(t) x;       // TestClass x;
        decltype((t)) y = x; // TestClass &y;
    }

    // auto
    {
        // Effective Modern C++ 条款3
        std::vector<int> vec{ 1,2,3,4,5 };
        decltype(vec[0]) v0 = vec[0];   // v0类型为 int&
        decltype(auto) v1 = vec[1];     // v1类型为 int&
        auto v2 = vec[2];               // v2类型为 int
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <vector>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

// 打印可变参数
template<typename ...Args>
void printer(Args&&... args)
{
    (std::cout << ... << args) << '\n';
}

// 给指定std::vector添加任意数量元素
template<typename T, typename... Args>
void push_back_vec(std::vector<T>& v, Args&&... args)
{
    static_assert((std::is_constructible_v<T, Args&&> && ...));
    (v.push_back(std::forward<Args>(args)), ...);
}

// 编译期交换整数的字节序
// compile-time endianness swap based on http://stackoverflow.com/a/36937049 
template<class T, std::size_t... N>
constexpr T bswap_impl(T i, std::index_sequence<N...>)
{
    return (((i >> N * CHAR_BIT & std::uint8_t(-1)) << (sizeof(T) - 1 - N) * CHAR_BIT) | ...);
}

// std::make_unsigned_t<T> 给T添加unsigned限定符，即 unsigned int 等价于 std::make_unsigned_t<int>
template<class T, class U = std::make_unsigned_t<T>>
constexpr U bswap(T i)
{
    return bswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
}

int main()
{
    {
        printer(1, 2, 3, "abc");
    }

    {
        std::vector<int> v;
        push_back_vec(v, 6, 2, 45, 12);
        push_back_vec(v, 1, 2, 9);
        for (int i : v) std::cout << i << ' ';
    }

    {
        static_assert(bswap<std::uint16_t>(0x1234u) == 0x3412u);
        //static_assert(bswap<std::uint16_t>(0x1234u) == 0x1234u); // 静态断言失败
        static_assert(bswap<std::uint64_t>(0x0123456789abcdefULL) == 0xefcdab8967452301ULL);
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

// https://www.cnblogs.com/yaoyu126/p/12427845.html

/*
 * function_traits用来获取所有函数语义类型的信息，
 *      (函数类型、返回类型、参数个数和参数的具体类型)。
 *
 * 函数语义类型包括
 *  普通函数
 *  函数指针
 *  function/lambda
 *  成员函数
 *  函数对象
 *
 * 实现function_traits的关键技术
 *  要通过模板特化和可变参数模板来获取函数类型和返回类型。
 *  先定义一个基本的function_traits的模板类：
 *     template<typename T>
 *     struct function_traits;
 *  再通过特化，将返回类型和可变参数模板作为模板参数，就可以获取函数类型、函数返回值和参数的个数了。
 *
 * 如:
 *  int func(int a, string b);
 * ## 获取函数类型
 *      function_traits<decltype(func)>::function_type; // int __cdecl(int, string)
 * # 获取函数返回值
 *      function_traits<decltype(func)>::return_type;   // int
 * # 获取函数的参数个数
 *      function_traits<decltype(func)>::arity;         // 2
 * # 获取函数第一个入参类型
 *      function_traits<decltype(func)>::args<0>::type; // int
 * # 获取函数第二个入参类型
 *      function_traits<decltype(func)>::args<1>::type; // string
 */

#ifndef __FUNCTION_TRAITS_H__
#define __FUNCTION_TRAITS_H__

#include <functional>

 // 原型
template<typename T>
struct function_traits;

// 普通函数
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{
    enum { arity = sizeof...(Args) };
    using return_type = ReturnType;
    using function_type = ReturnType(Args...);
    using stl_function_type = std::function<function_type>;
    using pointer = ReturnType(*)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };

    using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
    using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
};

// 函数指针
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)> : function_traits<ReturnType(Args...)> {};

// std::function
template<typename ReturnType, typename... Args>
struct function_traits<std::function<ReturnType(Args...)>> : function_traits<ReturnType(Args...)> {};

// 成员函数
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};\

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

// 函数对象
template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {};

template<typename Function>
typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
{
    return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
}

template<typename Function>
typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
{
    return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
}

template<typename Function>
typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
{
    return static_cast<typename function_traits<Function>::pointer>(lambda);
}

#endif

// ========== test ===========
#include <iostream>
#include <string>

using namespace std;

template<typename T>
void printType()
{
    cout << typeid(T).name() << endl;
}

float(*cast_func)(int, int, int, int);
float free_function(const string& a, int b)
{
    return (float)a.size() / b;
}
struct AA
{
    int f(int a, int b)volatile { return a + b; }
    int operator()(int)const { return 0; }
};

void test_function_traits()
{
    std::function<int(int)> f = [](int a) {return a; };
    printType<function_traits<std::function<int(int)>>::function_type>();
    printType<function_traits<std::function<int(int)>>::args<0>::type>();
    printType<function_traits<decltype(f)>::function_type>();
    printType<function_traits<decltype(free_function)>::function_type>();
    printType<function_traits<decltype(cast_func)>::function_type>();
    printType<function_traits<AA>::function_type>();
    using T = decltype(&AA::f);
    printType<T>();
    printType<function_traits<decltype(&AA::f)>::function_type>();
    static_assert(std::is_same<function_traits<decltype(f)>::return_type, int>::value, "");
}

int func(int a, char b)
{
    return 0;
}

int main()
{
    {
        // 获取函数类型
        function_traits<decltype(func)>::function_type;     // int __cdecl(int, char)
        // 获取函数返回值  
        function_traits<decltype(func)>::return_type;       // int
        // 获取函数的参数个数    
        function_traits<decltype(func)>::arity;             // 2
        // 获取函数第一个入参类型
        function_traits<decltype(func)>::args<0>::type;     // int
        // 获取函数第二个入参类型
        function_traits<decltype(func)>::args<1>::type;     // char

        //function_traits<decltype(func)>::args<2>::type;   //静态断言失败，func函数只有两个参数

        cout << typeid(function_traits<decltype(func)>::function_type).name() << endl;
        cout << typeid(function_traits<decltype(func)>::return_type).name() << endl;
        cout << (int)function_traits<decltype(func)>::arity << endl;
    }

    std::cout << "----------------------------\n";

    {
        test_function_traits();
    }
    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <iostream>

struct true_type {
    static constexpr bool value = true;
};

struct false_type {
    static constexpr bool value = false;
};

template <typename B, typename D>
true_type test_is_base(B*){return true_type();}

// 函数 test_is_base可以不用实现
template <typename B, typename D>
false_type test_is_base(void*);

// 注意struct的默认继承方式是public
// static_cast编译期转换
// decltype(x);返回的是一个类型，所以可以继承，可以看TEST2关于delctype的使用
// is_base_of就是继承一个true_type或者false_type的结构体
// test_is_base<B, D>(static_cast<D*>(nullptr)就是将D*类型的nullptr传给函数test_is_base
template <typename B, typename D>
struct is_base_of : decltype(test_is_base<B, D>(static_cast<D*>(nullptr))) {};

class A {};
class B :public A {};

int main()
{
    std::cout << std::boolalpha;
    std::cout << is_base_of<A, B>::value << '\n'; // true  因为B继承自A
    std::cout << is_base_of<B, A>::value << '\n'; // false 因为A不是继承自B
}

#endif // TEST6

#ifdef TEST7

//STL源码剖析 iterator_traits https://blog.csdn.net/buxiao1983/article/details/44043955

#include <iostream>
#include <vector>

//第一种方式
template <class Iter>
struct My_iter_trait//简化版的iterator_traits，只定义了value_type；为了区别于STL中的iterator::traits，名字也改为My_iter_trait
{
    typedef typename Iter::value_type value_type;
};

//第二种方式，偏特化
template <class Type>
struct My_iter_trait<Type*>
{
    // value_type就是指针指向值的类型
    typedef Type value_type;
};

//使用标准库iterator_traits
template <typename T>
struct MyStruct
{
    // [1]和[2]有同样的效果
    //typedef typename T::value_type VT;                       // [0] 如果T是指针类型，这句代码无法获取指针指向的数据类型，报错
    typedef typename My_iter_trait<T>::value_type VT;        // [1] 
    //typedef typename std::iterator_traits<T>::value_type VT; // [2] 使用iterator_traits可以获取指针指向的数据类型
};

// [1] 使用自定义 interator_trait解引用
template <class Iter>
void iter_print1(const Iter& a, const Iter& b)
{
    typedef typename My_iter_trait<Iter>::value_type T; //此处是关键，请思考一下：如果a，b是指针的话，除了用My_iter_trait的第二种方式，还有什么办法可以获得a或b所指向的数据的类型？
    T key;  //值
    Iter p; //指针
    for (p = a; p != b; p++)
    {
        key = *p;
        std::cout << key << ',';
    }
    std::cout << std::endl;
}
// [2] 通过使用*解引用，取得指针指向的实际值
template<typename Iter>
void iter_print2(const Iter& a, const Iter& b)
{
    Iter p = a;
    while (p != b)
    {
        std::cout << *p++ << ',';
    }
    std::cout << '\n';
}

// [3] 使用std::interator_trait解引用
template <class Iter>
void iter_print3(const Iter& a, const Iter& b)
{
    typedef typename std::iterator_traits<Iter>::value_type T;
    T key;  //值
    Iter p; //指针
    for (p = a; p != b; p++)
    {
        key = *p;
        std::cout << key << ',';
    }
    std::cout << std::endl;
}

int main()
{
    int a[10] = { 1,2,3,4,5,6,7,8,9,10 };
    std::vector<std::string> b{ "A","B","C","D","E","F","G" };
    int* p = a;

    iter_print1<int*>(a, a + 10);
    //iter_print1(a, a + 10); // 第一个参数是数组，第二个是指针，所以报错
    iter_print1(p, p + 10);   // 第一个参数和第二个都是指针，可以自动推导
    iter_print1<std::vector<std::string>::iterator>(b.begin(), b.end());
    iter_print1(b.begin(), b.end()); // c++17可以不指定模板列表，会自动推导

    std::cout << "---------------\n";

    iter_print2(p, p + 10);
    iter_print2(b.begin(), b.end());

    std::cout << "---------------\n";

    iter_print3(p, p + 10);
    iter_print3(b.begin(), b.end());

    std::cout << "---------------\n";

    MyStruct<int*>::VT s = 0;  //int
    //MyStruct<int>::VT s = 0;    //错误，int本身就是值类型，所以不存在获取指向值的类型

    return 0;
}

#endif // TEST7



