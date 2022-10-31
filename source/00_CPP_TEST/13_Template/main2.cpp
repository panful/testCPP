/*
* 01. C++编译期与运行期 https://wenku.baidu.com/view/73b3f307cfbff121dd36a32d7375a417866fc169.html
* 02. 模板实例化惰性原则
* 03. 函数模板和普通函数的重载
* 04. 模板函数返回值推导 std::conditional_t std::common_type_t
* 05. C++20 concepts std::totally_ordered 函数重载
* 06. 类外定义模板类的成员函数
* 07. 类外定义模板类的模板成员函数 static_assert std::is_convertible_v：类型是否可以转换，类模板友元
* 08. C++20 concepts requires std::convertibel_to 对TEST7使用c++20重写
* 09. 类模板的继承
* 10. 别名模板
* 11. 模板形参使用非类型
* 12. 模板实参，通用引用和右值引用，引用折叠
* 13. 模板实参传入C风格数组
* 14. 类模板的自动类型推导
* 15. C++20自动类型推导
* 16. 类模板构造函数可变参数  00_03_TEST7 01_02_TEST3
* 17. 模板特化，偏特化
* 18. 类模板特化 type-traits
* 19. 函数模板特化
* 20. 模板的默认参数
*/

// 模板系列 https://www.zhihu.com/people/mei-you-ming-zi-56-10
// 模板的实例化过程 https://cppinsights.io/

#define TEST16

#ifdef TEST1

#include <iostream>

class A
{
public:
    virtual void Func()
    {
        std::cout << "A\n";
    }
};

class B :
    public A
{
private:
    void Func() override
    {
        std::cout << "B\n";
    }
};

int main()
{
    // 类私有函数在类外调用
    A* p = new B();
    p->Func();

    return 0;
}

#endif // TEST1

#ifdef TEST2

// https://zhuanlan.zhihu.com/p/440167406

#include <iostream>

template<typename T>
class Test
{
public:
    int Func()
    {
        return std::abs(m_value);
    }
private:
    T m_value;
};

int main()
{
    Test<std::string> s1; // 不会报错，因为Test::Func()并没有实例化，体现了模板实例化的惰性原则
    //s1.Func(); // 只要不用std::string实例化Test::Func()就不会报错，实例化才会在编译期报错
}

#endif // TEST2

#ifdef TEST3
// https://zhuanlan.zhihu.com/p/457918701

#include <iostream>

// [1]
template<typename T>
T max(T t1, T t2)
{
    std::cout << "template<typename T> T max(T t1, T t2)\n";
    return t1 > t2 ? t1 : t2;
}

// [2]
double max(double d1, double d2)
{
    std::cout << "double max(double d1, double d2)\n";
    return d1 > d2 ? d1 : d2;
}

int main()
{
    // 当模板函数和普通函数都可以使用时，编译器会选择普通函数

    auto ret1 = max(1.0, 2.0); // 编译器会使用最合适的函数，即[2]
    auto ret2 = max(6, 2);  // [3]
    auto ret3 = max(3, 2);  // [4]
    auto ret4 = max<>(1.0, 2.0); // 会调用模板函数，<>指定了编译器应该只考虑模板函数，且模板函数的参数应该由编译器自动推导

    // [3] [4] 两次实例化的类型一样，编译器只会生成一份具体的函数，可以使用 C++ Insights 查看
}

#endif // TEST3

#ifdef TEST4

// https://zhuanlan.zhihu.com/p/453755804

#include <iostream>

// std::conditional_t是一种编译器的三元运算符
// std::conditional_t<bool B,T1,T2>，如果TB为true返回类型T1，否则返回T2
template<typename T1, typename T2>
typename std::conditional_t<(sizeof(T1) > sizeof(T2)), T1, T2>
max1(T1 t1, T2 t2)
{
    return t1 > t2 ? t1 : t2;
}

// std::common_type_t在编译期返回所有参数的公共类型，如果没有公共类型则报错
template<typename T1, typename T2>
typename std::common_type_t<T1, T2>
max2(T1 t1, T2 t2)
{
    return t1 > t2 ? t1 : t2;
}

// c++14才可以使用auto返回值类型推导
template<typename T1, typename T2>
auto max3(T1 t1, T2 t2)
{
    return t1 > t2 ? t1 : t2;
}

int main()
{
    auto ret1 = max1(1.0, 2);
    auto ret2 = max1('a', 2);

    auto ret3 = max2(1.0, 2);
    auto ret4 = max2('a', 2);

    auto ret5 = max3(1.0, 2);
    auto ret6 = max3('a', 2);
}

#endif // TEST4

#ifdef TEST5
// https://zhuanlan.zhihu.com/p/457918701

#include <iostream>
#include <concepts>

class Account
{
public:
    explicit Account(double b) : balance(b) {}
    double getBalance() const
    {
        return balance;
    }
private:
    double balance;
};
Account max(const Account& lhs, const Account& rhs)    // (*)
{
    std::cout << "max function\n";
    return (lhs.getBalance() > rhs.getBalance()) ? lhs : rhs;
}

template <std::totally_ordered T>                       // (**)
T max(const T& lhs, const T& rhs)
{
    std::cout << "max restricted function template\n";
    return (lhs > rhs) ? lhs : rhs;
}
template <typename T>                                   // (***)
T max(const T& lhs, const T& rhs)
{
    std::cout << "max unrestriced function template\n";
    return (lhs > rhs) ? lhs : rhs;
}

int main()
{
    Account account1(50.5);
    Account account2(60.5);
    Account maxAccount = max(account1, account2);       // (****)

    int i1{ 50 };
    int i2(60);
    int maxI = max(i2, i2);                             // (*****)
}
#endif // TEST5

#ifdef TEST6
// https://zhuanlan.zhihu.com/p/460522856

#include <iostream>

template<typename T, size_t N>
class MyArray
{
public:
    size_t GetSize() const;
private:
    T m_value[N];
};

// 在类外定义模板类的成员函数时：
// 1. 必须指定它是一个模板，即使这个函数没用用到任何和模板参数有关的内容也必须指定
// 2. 必须指定类模板的完整类型限定
template<typename T, size_t N>
size_t MyArray<T, N>::GetSize() const
{
    return N;
}

int main()
{
    MyArray<int, 5> arr1;
    std::cout << "MyArray<int, 5>:\t\t" << arr1.GetSize() << '\n';

    MyArray<double, 5> arr2;
    std::cout << "MyArray<double, 5>:\t\t" << arr2.GetSize() << '\n';

    MyArray<std::string, 6> arr3;
    std::cout << "MyArray<std::string, 6>:\t" << arr3.GetSize() << '\n';
}

#endif // TEST6

#ifdef TEST7
// https://zhuanlan.zhihu.com/p/460522856

#include <iostream>

//#define INSIDE
#define OUTSIDE

template<typename T, size_t N>
class MyArray
{
public:

#ifdef INSIDE
    // 类内重载=
    template<typename T2>
    MyArray<T, N>& operator= (const MyArray<T2, N>& arr)
    {
        std::cout << " indide\toperator = \n";
        //std::copy(std::begin(arr), std::end(arr), std::begin(m_value));

        static_assert(std::is_convertible_v<T2, T>, "Cannot convert T2 type to T type");

        for (size_t i = 0; i < N; ++i)
        {
            this->m_value[i] = static_cast<T>(arr.m_value[i]);
        }

        return *this;
    }
#endif // INSIDE

#ifdef OUTSIDE
    template<typename T2>
    MyArray<T, N>& operator= (const MyArray<T2, N>& arr);

    // 如果成员变量m_value为私有的，必须使用友元
    template<typename, size_t> friend class MyArray;

#endif // OUTSIDE
private:
    T m_value[N];
};

#ifdef OUTSIDE

// 类外重载=
// 必须指定类和成员函数是模板
// 必须指定通用成员函数的完整类型限定
template<typename T, size_t N>
template<typename T2>
MyArray<T, N>& MyArray<T, N>::operator=(const MyArray<T2, N>& arr)
{
    std::cout << "outside\toperator = \n";

    // std::is_convertible_v<T1,T2>如果T1类型不能转换为T2类型则返回false
    static_assert(std::is_convertible_v<T2, T>, "Cannot convert T2 type to T type");

    for (size_t i = 0; i < N; ++i)
    {
        this->m_value[i] = static_cast<T>(arr.m_value[i]);
    }

    return *this;
}

#endif // OUTSIDE

int main()
{
    MyArray<int, 5> arr1;
    MyArray<double, 5> arr2;
    MyArray<std::string, 5> arr3;

    arr1 = arr2;
    //arr1 = arr3; // std::string无法转换为int，静态断言失败
}

#endif // TEST7

#ifdef TEST8
// https://zhuanlan.zhihu.com/p/460522856

#include <iostream>
#include <concepts>

template<typename T, size_t N>
class MyArray
{
public:
    template<typename T2>
    MyArray<T, N>& operator=(const MyArray<T2, N>& arr) requires std::convertible_to<T2, T>;

    T m_value[N];
};

template<typename T, size_t N>
template<typename T2>
MyArray<T, N>& MyArray<T, N>::operator=(const MyArray<T2, N>& arr)
    requires std::convertible_to<T2, T>
{
    std::cout << "requires std::convertible_to<T1,T2>\n";

    for (size_t i = 0; i < N; ++i)
    {
        this->m_value[i] = static_cast<T>(arr.m_value[i]);
    }

    return *this;
}

int main()
{
    MyArray<int, 5> arr1;
    MyArray<double, 5> arr2;
    MyArray<std::string, 5> arr3;

    arr1 = arr2;
    arr1 = arr3; // std::string无法转换为int，VS报错：二元“ = ”: 没有找到接受“MyArray<std::string, 5>”类型的右操作数的运算符(或没有可接受的转换)
}

#endif // TEST8

#ifdef TEST9
// https://zhuanlan.zhihu.com/p/463066090

#include <iostream>

//#define TEST9_ERROR

template <typename T>
class Base
{
public:
#ifdef TEST9_ERROR
    void func()
    {
        std::cout << "func\n";
    }
#else
    void func1() const
    {
        std::cout << "func1()\n";
    }
    void func2() const
    {
        std::cout << "func2()\n";
    }
    void func3() const
    {
        std::cout << "func3()\n";
    }
#endif // TEST9_ERROR
};

template <typename T>
class Derived : public Base<T>
{
public:
#ifdef TEST9_ERROR
    // 两阶段查找 https://stackoverflow.com/questions/7767626/two-phase-lookup-explanation-needed
    void callBase()
    {
        func();  // 错误
    }
#else
    using Base<T>::func2;                // (2)
    void callAllBaseFunctions()
    {
        this->func1();                   // (1)
        func2();                         // (2)
        Base<T>::func3();                // (3)
    }
#endif // TEST9_ERROR

};

int main()
{
    std::cout << '\n';
    Derived<int> derived;

#ifdef TEST9_ERROR
    derived.callBase();
#else
    derived.callAllBaseFunctions();
#endif // TEST9_ERROR

    std::cout << '\n';
}

#endif // TEST9

#ifdef TEST10
// https://zhuanlan.zhihu.com/p/464310620

#include <iostream>

// 定义一个Row行，Col列，类型为T的矩阵
template<typename T, int Row, int Col>
class Matrix
{
};

// 别名模板：定义一个行和列相等的类型为T的矩阵（方阵）
template<typename T, int Row>
using Square = Matrix<T, Row, Row>;

// 别名模板：定义一个Row行一列的矩阵（列向量）
template<typename T, int Row>
using Vector = Matrix<T, Row, 1>;

// type_traits库中的别名模板 https://en.cppreference.com/w/cpp/header/type_traits
template<typename T>
struct removeReference
{
    using type = T;
};

// typename 必须有
template<typename T>
using removeReference_t = typename removeReference<T>::type;

int main()
{
    // 定义一个5x5的方阵
    Matrix<float, 5, 5> matrix1;
    Square<float, 5> square1;

    // 定义一个3维向量
    Matrix<double, 3, 1> matrix2;
    Vector<double, 3> vector1;

    // 1和2的输出相同
    std::cout << typeid(matrix1).name() << '\n'; // [1]
    std::cout << typeid(square1).name() << '\n'; // [2]

    // 3和4的输出相同
    std::cout << typeid(matrix2).name() << '\n'; // [3]
    std::cout << typeid(vector1).name() << '\n'; // [4]

    std::cout << "--------------\n";

    removeReference<int>::type t1;
    removeReference_t<int> t2;
}

#endif // TEST10

#ifdef TEST11

// https://zhuanlan.zhihu.com/p/464310620

// 模板形参使用非类型值最经典的例子就是std::array，它的第二个参数就是一个size_t型变量
//
// 模板形参还可以使用以下非类型：
//   左值引用
//   nullptr
//   指针值
//   枚举类型的值
//   整型值
//   浮点型值（C++20）


#include <iostream>
#include <vector>
#include <list>
#include <set>

// Container前面的typename在C++17之前需要使用class
template<typename T, template<typename> typename Container>
class MyVector
{
public:
    // 用初始化列表构造MyVector
    explicit MyVector(const std::initializer_list<T>& init_list) :
        m_value(init_list)
    {
    }

    void Print()
    {
        for (const auto& elem : m_value)
        {
            std::cout << elem << '\t';
        }
        std::cout << std::endl;
    }
private:
    Container<T> m_value;
};

int main()
{
    MyVector<int, std::vector> vec{ 1,2,3,4,5 };
    vec.Print();

    std::cout << "------------\n";

    MyVector<int, std::list> list{ 1,2,3,4,5 };
    list.Print();

    std::cout << "------------\n";

    MyVector<int, std::set> set{ 1,2,3,4,5 };
    set.Print();

    // std::array不可以，因为array还需要一个额外的非类型来在编译时指定容器大小
}
#endif // TEST11

#ifdef TEST12

// https://zhuanlan.zhihu.com/p/488186756

// 结论：模板中最好使用通用引用

// 通用引用的特点：
// 1. 严格形如 T&& 的格式；
// 2. T 涉及类型推导。
// auto&&也是一个通用引用
// 除此以外的其他&&都是右值引用

template <typename T>
void funcValue(T param) { }

template <typename T>
void funcReference(T& param) { }

// 此处的"&&"是通用引用不是右值引用
template <typename T>
void funcUniversalReference(T&& param) { }

class RVal {};

int main() {

    const int lVal{};
    const int& ref = lVal;

    funcValue(lVal);                  // (1)
    funcValue(ref);

    funcReference(lVal);              // (2)

    funcUniversalReference(lVal);     // (3)
    funcUniversalReference(RVal());

}

#endif // TEST12

#ifdef TEST13

// https://zhuanlan.zhihu.com/p/488186756

#include <cstddef>
#include <iostream>

template <typename T, std::size_t N>
std::size_t funcArraySize(T(&arr)[N]) {
    return N;
}

int main() {

    std::cout << '\n';

    int intArray[10]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    //funcArraySize(intArray);

    std::cout << "funcArraySize(intArray): " << funcArraySize(intArray) << '\n';

    std::cout << '\n';

}
#endif // TEST13

#ifdef TEST14

// https://zhuanlan.zhihu.com/p/492722317

#include <iostream>
#include <array>
#include <vector>
#include <mutex>
#include <memory>

template <typename T>
void showMe(const T& t) {
    std::cout << t << '\n';
}

// class 也可以改为struct
template <typename T>
class ShowMe {
public:
    ShowMe(const T& t) {
        std::cout << t << '\n';
    }
};

int main() {

    std::cout << "------------------\n";

    showMe(5.5);          // not showMe<double>(5.5);
    showMe(5);            // not showMe<int>(5);

    // c++17之后才支持
    ShowMe(5.5);          // not ShowMe<double>(5.5);
    ShowMe(5);            // not ShowMe<int>(5);

    std::cout << "------------------\n";

    // c++17之后才支持
    std::array myArr{ 1, 2, 3 };          // deduces std::array<int, 3> 
    std::vector myVec{ 1.5, 2.5 };        // deduces std::vector<double>

    //std::vector myVec2{ 1,1.1f,1.1 };        // 编译报错
    //std::vector myVec3{ 1,1.1f,1.1,"ss" };   // error，因为实参类型为 int,float,double,const char[3]，没有公共类型

    std::mutex mut;
    std::lock_guard myLock(mut);       // deduces std::lock_guard<mutex>(mut);

    std::pair myPair(5, 5.5);          // deduces std::pair<int, double>
    std::tuple myTup(5, myArr, myVec); // deduces std::tuple<int, std::array<int, 3>, std::vector<double>>

}

#endif // TEST14

#ifdef TEST15

// https://zhuanlan.zhihu.com/p/492722317

#include <concepts>
#include <iostream>

// c++20才支持函数形参为auto
void foo(auto t) {
    std::cout << typeid(t).name() << "\t" << t << std::endl;
}

void bar(std::integral auto t) {
    std::cout << typeid(t).name() << "\t" << t << std::endl;
}

template <std::regular T>
struct Class {
    Class(T t) {
        std::cout << typeid(t).name() << "\t" << t << std::endl;
    }
};

int main() {

    foo(5.5);
    //foo(Class(1)); //error 因为Class没有重载<<
    bar(5);
    Class cl(true);

}

#endif // TEST15

#ifdef TEST16

// https://zhuanlan.zhihu.com/p/492722317

#include <iostream>
#include <string>
#include <tuple>

class Test
{
public:
    template<typename ...Args>
    Test(Args&& ...args)
    {
        std::cout << "=====================\n";

        // 打印所有参数
        //(std::cout << ... << args ) << std::endl;
        //((std::cout << args << '\t'), ...);

        // 获取size 1
        constexpr int size = sizeof...(args);
        std::cout << "size:" << size << '\t';

        // 获取size 2
        std::index_sequence_for<Args...> for_size;
        std::cout << "size:" << for_size.size() << '\n';
    }
};

//++++++++++++++++++++++++++++++++++++++++++++
// 类模板实参推导（CTAD） https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
template <typename ...Args>
struct MyStruct : Args...
{
    using Args::operator()...;
};

template <typename ...Args>
MyStruct(Args...)->MyStruct<Args...>;

auto MyOperator = MyStruct{
    [](int a) {std::cout << "int\t" << a << '\n'; },
    [](float a) {std::cout << "float\t" << a << '\n'; },
    [](std::string a) {std::cout << "std::string\t" << a << '\n'; }
};
//++++++++++++++++++++++++++++++++++++++++++++
template<typename T>
struct MyStruct2:T
{
};

template <typename T>
MyStruct2(T)->MyStruct2<T>;

void func()
{
    auto r1 = MyStruct2{ [](int) {} };
    auto r2 = MyStruct2{ []() {} };
    r1(0);
    r2();
}

//++++++++++++++++++++++++++++++++++++++++++++
struct S1 { void operator()(int); };
struct S2 { void operator()(float) { std::cout << "float\n"; } };

void S1::operator()(int) { std::cout << "int\n"; };

struct S3 :S1, S2 {
    // 使用using和不使用结果没区别，但是vs会有警告：S3::operator()不明确
    using S1::operator();
    using S2::operator();
};

auto MyOperatorS3 = S3{};
//+++++++++++++++++++++++++++++++++++++++++++++

int main()
{
    {
        Test(1);
        Test(1, 2.0f, 3.0, "123");
        Test(3, 1.1);
        //Test<int>(1); // error 不能包含模板参数列表
    }

    std::cout << "---------------------------\n";

    {
        MyOperator(1);
        MyOperator(1.1f);
        MyOperator("abcd");
        //MyOperator(1, 1.1f); // error 没有重载operator()(int,float)
        //MyOperator(2.2);  // error，没有重载operator()(double)
    }

    std::cout << "---------------------------\n";

    {
        //MyOperatorS3就是一个对象 调用重载函数operator()
        MyOperatorS3(1);
        MyOperatorS3(2.2f);
        //MyOperatorS3(1, 1.1f); // error 没有重载operator()(int,float)
        //MyOperatorS3(3.3); // error，没有重载operator()(double)
    }
}

#endif // TEST16

#ifdef TEST17

// https://zhuanlan.zhihu.com/p/496391389

#include<iostream>

// 主模板 main template
template<typename T, int, int>
class Matrix { public: Matrix() { std::cout << "main template\n"; } };

                     // 偏特化  Partial template specialization
                     template<typename T>
class Matrix<T, 3, 3> { public: Matrix() { std::cout << "partial template specialization\n"; } };

                              // 全特化  Explicit (full) template specialization
                              template<>
class Matrix<float, 3, 3> { public: Matrix() { std::cout << "full template specialization\n"; } };

                                  int main()
                                  {
                                      Matrix<float, 1, 1> m2; // 主模板
                                      Matrix<int, 3, 3> m1;   // 偏特化
                                      Matrix<float, 3, 3> m3; // 全特化
                                  }

#endif // TEST17

#ifdef TEST18

                                  // https://zhuanlan.zhihu.com/p/503376695

#include <iostream>

                                  template<typename T, typename U>
                                  struct isSame
                                  {
                                      static constexpr bool value = false;
                                  };

                                  template<typename T>
                                  struct isSame<T, T>
                                  {
                                      static constexpr bool value = true;
                                  };

                                  template<typename T>
                                  struct removeReference
                                  {
                                      using type = T;
                                  };

                                  template<typename T>
                                  struct removeReference<T&>
                                  {
                                      using type = T;
                                  };

                                  template<typename T>
                                  struct removeReference<T&&>
                                  {
                                      using type = T;
                                  };

                                  int main()
                                  {
                                      int a = 1;
                                      int& b = a;
                                      int&& c = 2;

                                      decltype(a) aa;
                                      decltype(b) bb = a;
                                      decltype(c) cc = 2;;

                                      removeReference<int>::type t1;
                                      removeReference<int&>::type t2;
                                      removeReference<int&&>::type t3;

                                      if (isSame<int, int>::value)
                                      {
                                          std::cout << "is same\n";
                                      }
                                  }

#endif // TEST18

#ifdef TEST19

                                  // https://zhuanlan.zhihu.com/p/509671916
                                  // 函数模板只能进行全特化，不能进行偏特化
                                  // 对模板函数进行全特化不如直接重载非泛型函数

#include <iostream>​

                                  template<typename T>
                                  std::string getTypeName(T)
                                  {
                                      return "unknown";
                                  }

                                  template<typename T>
                                  std::string getTypeName(T*)
                                  {
                                      return "pointer";
                                  }

                                  template<>
                                  std::string getTypeName(int*)
                                  {
                                      return "int pointer";
                                  }

                                  template<typename T>
                                  std::string getTypeName2(T)
                                  {
                                      return "unknown";
                                  }

                                  template<>
                                  std::string getTypeName2(int*)
                                  {
                                      return "int pointer";
                                  }

                                  template<typename T>
                                  std::string getTypeName2(T*)
                                  {
                                      return "pointer";
                                  }

                                  int main() {

                                      std::cout << '\n';

                                      int* p = nullptr;

                                      // 重载解析不会考虑函数模板的特例化，只考虑主模板和普通函数。
                                      std::cout << "getTypeName(p): " << getTypeName(p) << '\n';
                                      std::cout << "getTypeName2(p): " << getTypeName2(p) << '\n';

                                      std::cout << '\n';

                                  }

#endif // TEST19

#ifdef TEST20

                                  // https://zhuanlan.zhihu.com/p/509671916


#include <functional>
#include <string>

// 比较大小，默认情况下，如果t1<t2，返回true
                                  template<typename T, typename Pre = std::less<T>>
                                  bool compare(T&& t1, T&& t2, Pre&& pre = Pre())
                                  {
                                      return pre(t1, t2);
                                  }

                                  int main()
                                  {
                                      auto ret1 = compare<int>(1, 2);
                                      auto ret2 = compare(1.1, 2.2);
                                      auto ret3 = compare('d', 'a');
                                      auto ret4 = compare(std::string("abc"), std::string("xyz"));

                                      // 如果t1>t2返回true
                                      auto ret5 = compare(1, 2, std::greater());
                                      auto ret6 = compare('d', 'a', std::greater<int>());

                                      // 如果t1的最后一个字符大于t2的第一个字符，则返回true
                                      auto ret7 = compare(std::string("abc"), std::string("xyz"), [](auto&& first, auto&& second) {return first.back() > second.front(); });
                                  }

#endif // TEST20
