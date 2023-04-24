/*
* 1. const和constexpr关键字修饰指针时在不同位置的作用
* 2. 常量字符串 string string_view
* 3. const对象不能调用那些成员函数
* 4. 修改const变量的值，常量折叠
* 5. const和constexpr的区别
* 6. constexpr修饰函数，构造函数
* 7. C++四种类型转换  00_07_TEST8 dynamic_cast
* 8. std::an_const
*/

#define TEST7

#ifdef TEST1

#include <iostream>

constexpr int x = 110;

int main()
{
    const int* p1 = nullptr;              //p1是一个指向常量的指针，指针本身是变量
    constexpr int* p2 = nullptr;        //p2是一个指向变量的常量指针，指针本身是常量
    constexpr const int* p3 = nullptr;  //p3是一个指向常量的常量指针
    const int* const p4 = nullptr;      //p4是一个指向常量的常量指针

    p1 = &x;    // 正确，x是常量
    //*p1 = 2;  // 错误
    //p2 = &x;  // 错误
    *p2 = 2;    // 正确，需要判断指针
    //p3 = &x;  // 错误
    //*p3 = 2;  // 错误
    //p4 = &x;  // 错误
    //*p4 = 2;  // 错误

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <string>
#include <string_view>

// constexpr只会添加顶层const，不会添加底层const
// 对于顶层const，赋值的时候是可以被去除的，而底层const则不行

int main()
{
    const std::string s1 = "123";
    const std::string_view s2 = "234";
    //constexpr std::string s3 = "234";    //错误
    constexpr std::string_view s4 = "456";

}

#endif // TEST2

#ifdef TEST3

#include <iostream>

class A
{
public:
    void Func1() const
    {
        std::cout << "func1\n";
    }
    void Func2()
    {
        std::cout << "func2\n";
    }
    const int Func3()
    {
        std::cout << "func3\n";
        return 1;
    }
    void Func4(const int n)
    {
        std::cout << "func4\n";
    }
    const int Func5() const
    {
        std::cout << "func5\n";
        return 1;
    }
};

int main()
{
    // const对象只能调用函数声明后面带const的成员函数

    const auto a1 = A();
    a1.Func1();    // 正确
    //a1.Func2();  // 错误
    //a1.Func3();  // 错误
    //a1.Func4(0); // 错误
    a1.Func5();

    std::cout << "----------------------\n";

    auto a2 = A();
    a2.Func1();
    a2.Func2();
    a2.Func3();
    a2.Func4(0);
    a2.Func5();
}

#endif // TEST3

#ifdef TEST4

#include <iostream>

int main()
{
    {
        int a = 10;
        const int& b = a;
        std::cout << "a = " << a << "\tb =" << b << std::endl;
        a = 20;
        std::cout << "a = " << a << "\tb =" << b << std::endl;     // b的值被改为了20
        std::cout << "&a = " << &a << "\t&b =" << &b << std::endl; // 地址一样，值一样
    }

    std::cout << "----------------\n";

    {
        const int a = 10;
        int& b = const_cast<int&>(a);
        std::cout << "a = " << a << "\tb =" << b << std::endl;
        b = 20;
        std::cout << "a = " << a << "\tb =" << b << std::endl;     // a的值没有改变
        std::cout << "&a = " << &a << "\t&b =" << &b << std::endl; // 地址一样，值不一样
    }

    std::cout << "-------------------\n";

    {
        // 常量折叠 http://www.cppblog.com/xmli/archive/2010/11/23/134425.html
        // 预编译阶段，编译器会进行优化，将const的值全部替换掉，但在实际运行阶段，内存里的值被改变了
        const int a = 1;
        int* b = const_cast<int*>(&a);

        std::cout << "a = " << a << "\tb =" << *b << std::endl;
        *b = 2;
        std::cout << "a = " << a << "\tb =" << *b << std::endl;    // a的值没有改变
        std::cout << "&a = " << &a << "\t&b = " << b << std::endl; // 地址一样，值不一样
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

// const 表示的是只读 constexpr 表示的常量
void func(const int x) {}  // ok
//void func(constexpr int y) {} // error

constexpr int func2() { return 2; }

int main()
{
    {
        constexpr int num1 = 1 + 2 + 3;
        char arr1[num1] = { 0 };
        //num1++; // error

        const int num2 = 1 + 2 + 3;
        char arr2[num2] = { 0 };
        //num2++; // error

        int num3 = 1 + 2 + 3;
        //char arr3[num3] = { 0 }; // error

        char arr4[func2()] = { 0 };
    }
}
#endif // TEST5

#ifdef TEST6

// 构造函数使用constexpr https://blog.csdn.net/craftsman1970/article/details/80244873
// constexpr的使用 https://docs.microsoft.com/zh-cn/cpp/cpp/constexpr-cpp?view=msvc-170

#include <iostream>

using namespace std;

int test1()
{
    return 8;
}

constexpr void test2()
{
    // ok
}

const int test3()
{
    return 3;
}

constexpr int test4()
{
    //test1(); // error，因为test1没有被constxepr修饰
    test2(); // ok
    //test3(); //error，因为test3没有被constexpr修饰，只被const修饰

    return 1;
}

constexpr int test5(int x)
{
    //return x;  // ok

    //return 1 + 2 + x;  // ok

    //int a = x * x;
    //a = 3;
    //return a + x;  // ok

    //int ret = 1 + 2 + x;
    //return ret;  // ok

    //return test3() + x + 2; // error

    //return test1(); // error

    return test4() + x + 2;
}

// 值传递
constexpr float func1(float x, int n)
{
    return x * n;
}

// 引用传递
constexpr float func2(const float& x, const int& n)
{
    return x * n;
}

// 编译期计算数组长度
template<typename T, int N>
constexpr int func3(const T(&)[N])
{
    return N;
}

// 递归
constexpr int func4(int n)
{
    return n == 1 ? 1 : n * func4(n - 1);
}

// 自定义类型
class Foo
{
public:
    constexpr explicit Foo(int i) : _i(i) {} // c++17:ok c++20:ok
    //constexpr explicit Foo(int i) { int x = 4; _i = i + x; } // c++17:error c++20:ok
    constexpr int GetValue() const
    {
        return _i;
    }
private:
    int _i;
};

int main()
{
    {
        std::cout << test1() << '\n';
        std::cout << test5(test1()) << '\n';
        auto n = test1();
        std::cout << test5(n) << '\n';

        int x = 4;
        auto ret = test5(x);
        std::cout << ret << '\n';
    }

    std::cout << "---------------------\n";

    {
        int x = 3;
        auto ret1 = test5(x);// auto被推导为int，而不是constexpr int
        constexpr int ret2 = test5(1); // ok
        //constexpr int ret3 = test5(x); //error 如果返回值被constexpr修饰的变量接收，则参数必须为常量

        const int y = 4;
        constexpr int ret4 = test5(y); // ok
    }

    std::cout << "-------------------\n";

    {
        // foo is const:
        constexpr Foo foo1(5);
        // f1 = Foo(6); //Error!
        auto foo2 = Foo(2);
        Foo foo3(3);

        // Compile time:
        constexpr float x = func1(5, 3);
        constexpr float y{ func2(2, 5) };
        constexpr int val = foo1.GetValue();
        constexpr int f5 = func4(5);
        const int nums[]{ 1, 2, 3, 4 };
        const int nums2[func3(nums) * 2]{ 1, 2, 3, 4, 5, 6, 7, 8 };

        // Run time:
        cout << "The value of foo is " << foo1.GetValue() << endl;
    }

}
#endif // TEST6

#ifdef TEST7

#include <iostream>

class A {};
class B :public A {};

void testFunc1() {}
int testFunc2(int) { return 1; }

#define PRINT_TYPE(x) std::cout<<#x<<'\t'<<typeid(x).name()<<'\n';

int main()
{
    // static_cast
    {
        int n = 0, * pn = &n;
        //auto r1 = static_cast<float*>(pn);    // error
        auto r2 = static_cast<float>(n);      // ok r2类型为float
        auto r3 = static_cast<int*>(nullptr); // ok r3类型为int*，且r3 = nullptr
        //auto r4 = static_cast<float*>(pn);    // error 不能转换空指针以外的指针类型

        A* pA = new A();
        B* pB = new B();
        auto r5 = static_cast<B*>(pA);  // ok 父类转子类，不安全，不建议这样做
        auto r6 = static_cast<A*>(pB);  // ok 子类转父类，安全
        auto r7 = static_cast<void*>(nullptr); // ok
    }

    // const_cast
    {
        const int cn = 0, * pcn = &cn, & rcn = cn;
        int n = 0, * pn = &n, & rn = n;
        //auto r1 = const_cast<int>(cn);      // error <>中的类型只能是指针或引用

        auto r2 = const_cast<int*>(pcn);       // ok 删除const限定
        auto& r3 = const_cast<int&>(rcn);      // ok r3类型为 int&
        auto r4 = const_cast<const int*>(pn);  // ok 添加const限定
        auto& r5 = const_cast<const int&>(n);  // ok r5类型为 const int&

        // typeid无法判断非指针数据为const https://bbs.csdn.net/topics/190122047

        PRINT_TYPE(cn);
        PRINT_TYPE(pcn);
        PRINT_TYPE(rcn);
        PRINT_TYPE(n);
        PRINT_TYPE(pn);
        PRINT_TYPE(rn);

        PRINT_TYPE(r2);
        PRINT_TYPE(r3);
        PRINT_TYPE(r4);
        PRINT_TYPE(r5);
    }

    std::cout << "---------------\n";

    {
        const int a = 10;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine  这句代码是一个未定义行为(UB)

        // 指针p和指针q都是指向a变量的，指向地址相同，但是值却不一样
        // 常量折叠 可以看00_10_TEST4
        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    std::cout << "---------------\n";

    {
        int c = 11;
        const int a = c;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine

        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    std::cout << "---------------\n";

    {
        const int c = 11;
        const int a = c;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine

        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    // reinterpret_cast
    {
        int n = 666, * p = &n, & r = n;
        auto r1 = reinterpret_cast<float*>(p); // ok r1的类型为float*
        auto ret1 = *r1; // ret1是一个非法值
        auto r2 = reinterpret_cast<float&>(r); // ok r2的类型为float不是float&
        //auto r3 = reinterpret_cast<float>(n);  // error
        //auto r4 = reinterpret_cast<float>(r);  // error
        auto r5 = reinterpret_cast<float*>(&r);  // ok
        // auto ret2 = *r5; // error 运行时报错
        auto r6 = reinterpret_cast<float&>(n);  // ok

        // 函数指针转换
        typedef void (*FUNC)();
        auto r7 = reinterpret_cast<FUNC>(testFunc1);
        auto r8 = reinterpret_cast<FUNC>(testFunc2);
        r7(); // ok
        r8(); // ok

        //auto r9 = reinterpret_cast<int*>(nullptr); // error，空指针转换应该用static_cast
        float* pf = nullptr;
        auto r10 = reinterpret_cast<int*>(pf); //ok

        // 一定要谨慎使用reinterpret_cast 
        // 例如{auto r5 = reinterpret_cast<float*>(r);}不应该被转换，但是编译器不会报错
        // {auto r8 = reinterpret_cast<FUNC>(testFunc2);}不应该被转换，函数签名不一样
    }

    // dynamic_cast 00_07_TEST8
    {
        A* pA = new A();
        B* pB = new B();

        auto r1 = dynamic_cast<A*>(pB); // 子类向父类转换，安全
        //dynamic_cast<B*>(pA); // 因为类A中没有虚函数，所以编译失败
    }

}

#endif // TEST7

#ifdef TEST8

#include <iostream>
#include <utility>  // std::as_const
#include <string>
#include <functional>
#include <format>

int main()
{
    std::cout << std::boolalpha;

    {
        std::string str1 = "str";
        const std::string str2 = "str";
        auto constStr1 = std::as_const(str1);
        auto& constStr2 = std::as_const(str1);
        const std::string constStr3 = std::as_const(str1);
        const std::string& constStr4 = std::as_const(str1);

        std::cout << std::is_const_v<decltype(str1)> << '\n';
        std::cout << std::is_const_v<decltype(str2)> << '\n';
        std::cout << std::is_const_v<decltype(constStr1)> << '\n';
        std::cout << std::is_const_v<decltype(constStr2)> << '\n';
        std::cout << std::is_const_v<decltype(constStr3)> << '\n';
        std::cout << std::is_const_v<decltype(constStr4)> << '\n';
    }
    std::cout << "----------------------\n";
    {
        int A1 = 2;
        const int A2 = 2;
        auto constA1 = std::as_const(A1);
        auto& constA2 = std::as_const(A1);
        const int constA3 = std::as_const(A1);
        const int& constA4 = std::as_const(A1);

        std::cout<<std::is_const_v<decltype(A1)> << '\n';
        std::cout<<std::is_const_v<decltype(A2)> << '\n';
        std::cout<<std::is_const_v<decltype(constA1)> << '\n';
        std::cout<<std::is_const_v<decltype(constA2)> << '\n';
        std::cout<<std::is_const_v<decltype(constA3)> << '\n';
        std::cout<<std::is_const_v<decltype(constA4)> << '\n';
    }
    std::cout << "----------------------\n";
    {
        const char* name_const{ "Tom" };
        std::cout << std::format("type of name_const is: {}\n", typeid(name_const).name());
        std::cout << std::is_const_v<decltype(name_const)> << '\n';

        auto name_remove_const = const_cast<char*>(name_const);
        std::cout << std::format("type of name_remove_const is: {}\n", typeid(name_remove_const).name());
        std::cout << std::is_const_v<decltype(name_remove_const)> << '\n';

        auto name_add_const = const_cast<const char*>(name_remove_const);
        std::cout << std::format("type of name_add_const is: {}\n", typeid(name_add_const).name());
        std::cout << std::is_const_v<decltype(name_add_const)> << '\n';

        std::string str{ "Hello world" };
        const std::string& str_const_ref = std::as_const(str);
        std::cout << std::format("type of str_const_ref is : {}\n", typeid(str_const_ref).name());
        std::cout << std::is_const_v<decltype(str_const_ref)> << '\n';

    }
}


#endif // TEST8
