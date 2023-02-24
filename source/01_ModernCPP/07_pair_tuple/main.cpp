/*
* 1. std::tuple
* 2. std::pair std::piecewise_construct 分段构造  https://www.cnblogs.com/guxuanqing/p/11396511.html
* 3.
* 4. std::apply 将std::tuple、std::pair、std::array等展开作为函数对象的参数传入
* 5. std::integer_sequence 模板实参推导，变参模板
* 6. std::make_from_tuple
*/

#define TEST1

#ifdef TEST1

#include <iostream>     // std::cout
#include <tuple>        // std::tuple, std::make_tuple, std::tie
#include <string>
#include <array>

int main()
{
    // 创建合并std::tuple
    {
        int myInt;
        std::string myString;
        int arr[] = { 1,2,3 };
        std::array<int, 3> myarray{ 7,8,9 };

        //1.直接定义一个tuple
        std::tuple<int, std::string, int> mytuple1{ 1,"abc",2 };
        //2.使用make_tuple构造tuple
        std::tuple<int, float, std::string> mytuple2 = std::make_tuple(10, 2.6, "abc");
        //3.自动识别tuple元素类型
        auto mytuple3 = std::make_tuple(arr, 1, "abc");
        auto mytuple4 = std::make_tuple(124, myarray, "abc");
        //4.拼接tuple,可以将多个tuple连接到一起
        auto mytuple5 = std::tuple_cat(mytuple1, mytuple2, mytuple3, mytuple4);
        //5.{}构造std::tuple
        std::tuple<int, std::string, int> mytuple6 = { 1,"aaa",2 };

        // 交换两个std::tuple的元素（两个元组的类型必须一致）
        mytuple1.swap(mytuple6);

        // 修改std::tuple元素的值，std::get<N> N的值如果大于类型个数则编译失败
        std::get<0>(mytuple1) = 99;

        // 获取std::tuple对应位置元素的值
        auto ret = std::get<0>(mytuple1);
        std::cout << ret << '\n';
    }

    std::cout << "-----------------------\n";

    // std::forward_as_tuple 以右值为参数时拥有右值引用数据成员，否则拥有左值引用数据成员。
    {
        auto t1 = std::forward_as_tuple(1, 2.2, 'c'); // {int&&, double&&, char&&}
        auto t2 = std::make_tuple(1, 2.2, 'c');       // {int, double, char}

        int a = 1; double b = 2.2; char c = 'c';

        auto t3 = std::forward_as_tuple(a, b, c);     // {int&, double&, char&}
        auto t4 = std::make_tuple(a, b, c);           // {int, double, char}
        
        std::get<0>(t3) = 88;
        std::cout << a << '\n'; // a的值被修改为88，t3存储的是a的引用
        std::cout << std::get<0>(t4) << '\n'; // 打印1，因为t4存储的是a的拷贝

        std::cout << "forward_as_tuple <-> make_tuple\t" << std::is_same_v<decltype(t1), decltype(t2)> << '\n';
    }

    std::cout << "-----------------------\n";

    // 获取std::tuple的信息
    {
        // 获取tuple的大小，即模板参数的个数
        std::tuple<int, char, std::string> tuple1{ 1,'a',"sss" };
        auto size1 = std::tuple_size<decltype(tuple1)>::value;
        auto size2 = std::tuple_size_v<std::tuple<int, char, std::string>>;

        // 获取tuple元素的类型，tuple_element的第一个参数指定要获取tuple第几个值的类型
        // 如果第一个参数数值大于std::tuple的个数在编译期就会报错
        // 也可以访问std::pair元素的类型，只不过第一个参数只能是1或0
        std::tuple_element<0, decltype(tuple1)>::type myInt;
        std::cout << typeid(myInt).name() << '\n';
        std::cout << typeid(std::tuple_element_t<1, std::tuple<char, int, float>>).name() << '\n';
    }

    std::cout << "-----------------------\n";

    // 解包
    {
        std::tuple<int, char, std::string> mytuple{ 1,'a',"abc" };

        int myInt = 0;
        std::string myString;

        //解包，即获取tuple元素的值,myInt和myString的值为tuple元素对应位置的值
        //std::ignore表示该位置的元素值不获取
        std::tie(myInt, std::ignore, myString) = mytuple;
        std::cout << myInt << '\t' << myString << '\n';

        // 结构化绑定，类似解包
        const auto& [a, b, c] = mytuple;
        std::cout << a << '\t' << b << '\t' << c << '\n';
    }

    return 0;
}


#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <utility>  // std::piecewise_construct
#include <string>
#include <map>
#include <complex>

struct Foo {
    Foo(std::tuple<int, float>)
    {
        std::cout << "Constructed a Foo from a tuple\n";
    }
    Foo(int, float)
    {
        std::cout << "Constructed a Foo from an int and a float\n";
    }
    Foo(int, float, std::string)
    {
        std::cout << "Constructed a Foo from int float std::string\n";
    }
};

int main()
{
    // 构造、访问std::pair
    {
        std::pair p1(1, 2.2f);
        std::pair<int, char> p2;
        p2 = std::make_pair(2, 'c');

        auto IntP1 = std::make_pair<int, int>(2, 3);
        auto IntP2 = std::make_pair(3, 4);  //可以自动识别类型

        p2.first;  // 访问p2的首个元素
        p2.second;

        auto p2First = std::get<0>(p2); //访问p2的首个元素<>内的数字指示访问第几个元素

        typedef std::tuple_element<1, decltype(p2)>::type TypeChar;
        TypeChar c; //c的类型为char
    }

    // 分段构造
    // std::piecewise_construct就好像把std::tuple解包了一样
    // 主要是为了消除歧义
    {
        std::tuple<int, float> t(1, 3.14);

        std::pair<Foo, Foo> p1(t, t);
        std::cout << "----------------\n";
        std::pair<Foo, Foo> p2(std::piecewise_construct, t, t);
        std::cout << "----------------\n";
        std::pair<Foo, Foo> p3(std::piecewise_construct, std::make_tuple(1, 2.2f, ""), std::make_tuple(1, 3.3f, ""));
        //std::pair<Foo, Foo> p4(std::make_tuple(1, 2.2f, ""), std::make_tuple(1, 3.3f, "")); // 编译错误，参数不匹配

        std::map<std::string, std::complex<int>> myMap;
        //myMap.emplace("hello", 1, 2); // error，map不能识别那个是key那个是value
        //myMap.emplace(std::make_tuple("hello"), std::make_tuple(1, 2)); // error
        //std::pair<std::string, std::complex<int>> scp1("hello", 1, 2);  // error
        std::pair<std::string, std::complex<int>> scp2("hello", { 1, 2 }); // ok
        //std::pair<std::string, std::complex<int>> scp3(std::make_tuple("hello"), std::make_tuple(1, 2)); // error
        std::pair<std::string, std::complex<int>> scp4(std::piecewise_construct, std::make_tuple("hello"), std::make_tuple(1, 2)); // ok

        //myMap.emplace("hello", { 1,2 }); // error
        myMap.emplace(std::pair<std::string, std::complex<int>>{"hello", { 1,2 }}); // ok
        myMap.emplace(std::piecewise_construct, std::make_tuple("hello"), std::make_tuple(1, 2)); // ok
        myMap.emplace(scp2); // ok
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3



#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <tuple>
#include <utility>
#include <array>
#include <vector>

int add(int first, int second) { return first + second; }

template<typename T>
T add_generic(T first, T second) { return first + second; }

auto add_lambda = [](auto first, auto second) { return first + second; };

auto print_lambda = [](auto first, auto second) {std::cout << "first:" << first << "\tsecond:" << second << '\n'; };

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, std::tuple<Ts...> const& theTuple)
{
    std::apply
    (
        [&os](Ts const&... tupleArgs)
        {
            os << '[';
            std::size_t n{ 0 };
            ((os << tupleArgs << (++n != sizeof...(Ts) ? ", " : "")), ...);
            os << ']';
        }, theTuple
    );
    return os;
}

struct TT {
    int sum(int a, int b, int c) {
        return a + b + c;
    }
};

//https://blog.csdn.net/fl2011sx/article/details/119217893?spm=1001.2014.3001.5502

int main()
{
    // 将参数1和2作为add的参数传入
    std::cout << std::apply(add, std::pair(1, 2)) << '\n';

    //std::cout << std::apply(add_generic, std::make_pair(2.0f, 3.0f)) << '\n'; // error 无法推导函数类型
    std::cout << std::apply(add_generic<float>, std::make_pair(2.0f, 3.0f)) << '\n'; // ok
    std::cout << std::apply(add_generic<float>, std::make_pair(2.0f, 3.0)) << '\n';  // ok

    std::cout << std::apply(add_lambda, std::pair(2.0f, 3.0f)) << '\n'; // ok
    std::cout << std::apply(add_lambda, std::pair(2.0f, 3.0)) << '\n';  // ok

    // 只要可以通过std::get<N>获取元素的容器都可以使用std::apply
    std::apply(print_lambda, std::pair{ 1,2 });
    std::apply(print_lambda, std::tuple{ 3,"abc" });
    std::apply(print_lambda, std::array<int, 2>{4, 5});

    // 非静态成员函数
    std::tuple tu(TT(), 1, 2, 3); // 第一个成员就是调用成员
    auto res = std::apply(&TT::sum, std::move(tu)); // 这里传成员函数指针
    // 效果相当于std::get<0>(tu).sum(1, 2, 3)
    std::cout << res << std::endl;

    // 打印变参std::tuple
    std::cout << std::tuple{ 25, "Hello", 9.31f, 'c' } << '\n'; // ok
    //std::cout << std::tuple{ 25, "Hello", 9.31f, std::pair{1, 2} } << '\n'; // error，没有对 std::ostream<<std::pair

    // 变参lambda
    auto myLambda = [](auto&& ...args) {((std::cout << args << '\t'), ...); std::cout << std::endl; };
    std::apply(myLambda, std::tuple{ 1,1.1f,"abc" });
    std::apply(myLambda, std::tuple{ "abcd"});
}

#endif // TEST4

#ifdef TEST5

#include <tuple>
#include <iostream>
#include <array>
#include <utility>

// 调试辅助
template<typename T, T... ints>
void print_sequence(std::integer_sequence<T, ints...> int_seq)
{
    std::cout << "The sequence of size " << int_seq.size() << ": ";
    ((std::cout << ints << ' '), ...);
    std::cout << '\n';
}

// 转换数组为 tuple
template<typename Array, std::size_t... I>
auto a2t_impl(const Array& a, std::index_sequence<I...>)
{
    return std::make_tuple(a[I]...);
}

template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
auto a2t(const std::array<T, N>& a)
{
    return a2t_impl(a, Indices{});
}

// 漂亮地打印 tuple
template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr>& os,
    const Tuple& t,
    std::index_sequence<Is...>)
{
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<class Ch, class Tr, class... Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t)
{
    os << "(";
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os << ")";
}

enum MyEnum { E1,E2,E3};

int main()
{
    {
        /*
        * std::integer_sequence作用是产生一个整型数列
        * 该struct只有一个函数size()，用来返回整型数列的大小
        */
        std::integer_sequence<int, 0, 1, 2, 3, 4> myIntegerSequence1;
        std::cout << "size:\t" << myIntegerSequence1.size() << "\tstd::integer_sequence<int, 0, 1, 2, 3, 4> type:\t" << typeid(decltype(myIntegerSequence1)).name() << '\n';

        std::integer_sequence<size_t, 3, 2, 1> myIntegerSequence2;
        std::cout << "size:\t" << myIntegerSequence2.size() << "\tstd::integer_sequence<size_t, 3, 2, 1> type:\t" << typeid(decltype(myIntegerSequence2)).name() << '\n';

        // ok 相当于struct std::integer_sequence<char,97,98,99,1,2,3>
        std::integer_sequence<char, 'a', 'b', 'c', 1, 2, 3> myIntegerSequence3;
        std::cout << "size:\t" << myIntegerSequence3.size() << "\tstd::integer_sequence<char, 'a', 'b', 'c', 1, 2, 3> type:\t" << typeid(decltype(myIntegerSequence3)).name() << '\n';

        // error std::integer_sequence模板的第一个参数必须为整形，例如 int, unsigned int, short等
        //std::integer_sequence<float, 1.1f,2.2f,3.3f> myIntegerSequence4;
    }

    std::cout << "----------------------------------\n";

    {
        /*
        * std::make_integer_sequence模板的第一个参数必须是整形，例如 int, unsigned int, short等
        * std::make_integer_sequence相当于如下定义：
        * template <class T, T N>
        * using make_integer_sequence = integer_sequnce<T, 0, 1, 2, 3, 4, ..., N - 1>
        */
        std::make_integer_sequence<int, 5> myIntegerSequence1;
        std::cout << "std::make_integer_sequence<int, 5> type:\t" << typeid(decltype(myIntegerSequence1)).name() << std::endl;

        std::make_integer_sequence<char, 6> myIntegerSequence2;
        std::cout << "std::make_integer_sequence<char, 6> type:\t" << typeid(decltype(myIntegerSequence2)).name() << std::endl;

        std::make_integer_sequence<short, 7> myIntegerSequence3;
        std::cout << "std::make_integer_sequence<short, 7> type:\t" << typeid(decltype(myIntegerSequence3)).name() << std::endl;

        // error
        //std::make_integer_sequence<MyEnum, MyEnum::E3> myIntegerSequence4;

        // error
        //std::make_integer_sequence<std::string, 6> myIntegerSequence3;
    }

    std::cout << "----------------------------------\n";

    {
        /*
        * 标准库有如下定义，即std::make_index_sequence只是一个别名模板
        * template <size_t _Size>
        * using make_index_sequence = make_integer_sequence<size_t, _Size>;
        */

        std::make_index_sequence<5> myIndexSequence1;
        std::cout << "std::make_index_sequence<5> type:\t" << typeid(decltype(myIndexSequence1)).name() << std::endl;

        std::make_integer_sequence<size_t, 5> myIndexSequence2;
        std::cout << "std::make_integer_sequence<size_t, 5> type:\t" << typeid(decltype(myIndexSequence2)).name() << std::endl;
    }

    std::cout << "---------------------------------------\n";

    {
        /*
        * std::index_sequence_for在标准库有如下定义：
        * 作用就是将index_sequence_for的模板参数【个数】设置为make_index_sequence的模板参数
        * index_sequence_for的模板参数必须是类型，不能是值，函数等
        * template <class... _Types>
        * using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
        */

        int n = 0;
        std::index_sequence_for<int, float, std::string, std::ios, decltype(n), decltype(std::abs(1))> myIndexSequenceFor;
        std::cout << "size:\t" << myIndexSequenceFor.size() <<
            "\tstd::index_sequence_for<int, float, std::string, std::ios, decltype(n), decltype(std::abs(1))> type:\t" << typeid(decltype(myIndexSequenceFor)).name() << '\n';
    }

    std::cout << "---------------------------------------\n";

    {
        std::array<int, 4> array = { 1,2,3,4 };

        // 转换 array 为 tuple
        auto tuple = a2t(array);
        static_assert(std::is_same<decltype(tuple),
            std::tuple<int, int, int, int>>::value, "different types");

        // 静态断言失败
        //static_assert(std::is_same<decltype(tuple),
        //    std::tuple<int, int, int, float>>::value, "different types");

        // 打印到 cout
        std::cout << tuple << '\n';
    }

    return 0;
}

#endif // TEST5

#ifdef TEST6

// https://blog.csdn.net/fl2011sx/article/details/119217893?spm=1001.2014.3001.5502

#include <string>
#include <tuple>
#include <iostream>

class Test {
public:
    Test(int a, double b, const std::string& c) : a_(a), b_(b), c_(c) {}
    void show() const { std::cout << a_ << " " << b_ << " " << c_ << std::endl; }
private:
    int a_;
    double b_;
    std::string c_;
};

// 自行封装构造过程
template <typename T, typename... Args>
T Create(Args &&...args) {
    return T(args...);
}

int main(int argc, const char* argv[]) 
{
    {
        Test&& t1 = std::apply([](auto &&...args)->Test {return Create<Test>(args...); }, std::tuple{ 1, 2.5, "abc" });
        Test&& t2 = std::apply([](auto &&...args)->Test {return Test{ args... }; }, std::tuple{ 2, 3.5, "xyz" });
        t1.show(); // 打印：1 2.5 abc
        t2.show();
    }

    // 使用std::make_from_tuple将std::tuple展开作为构造函数的参数
    {
        std::tuple tu(1, 2.5, "abc");
        Test&& t = std::make_from_tuple<Test>(std::move(tu));
        t.show();
    }

    return 0;
}
#endif // TEST6
