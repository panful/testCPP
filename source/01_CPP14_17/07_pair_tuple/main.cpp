/*
* 1. std::tuple
* 2. std::pair
* 3. std::piecewise_construct 分段构造  https://www.cnblogs.com/guxuanqing/p/11396511.html
* 4. std::apply 变参模板
* 5. std::integer_sequence 模板实参推导，变参模板
* 6. std::make_from_tuple
*/

#define TEST6

#ifdef TEST1

#include <iostream>     // std::cout
#include <tuple>        // std::tuple, std::make_tuple, std::tie

int main()
{
    int myint;
    char mychar;

    std::tuple<int, float, char> mytuple;

    mytuple = std::make_tuple(10, 2.6, 'a');  //构造tuple

    std::tie(myint, std::ignore, mychar) = mytuple;   //解包tuple

    std::cout << "myint contains: " << myint << '\n';
    std::cout << "mychar contains: " << mychar << '\n';

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

int main()
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

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <utility>
#include <tuple>

struct Foo {
    Foo(std::tuple<int, float>)
    {
        std::cout << "Constructed a Foo from a tuple\n";
    }
    Foo(int, float)
    {
        std::cout << "Constructed a Foo from an int and a float\n";
    }
};

int main()
{
    std::tuple<int, float> t(1, 3.14);
    std::pair<Foo, Foo> p1(t, t);
    std::pair<Foo, Foo> p2(std::piecewise_construct, t, t);
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <tuple>
#include <utility>

int add(int first, int second) { return first + second; }

template<typename T>
T add_generic(T first, T second) { return first + second; }

auto add_lambda = [](auto first, auto second) { return first + second; };

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

int main()
{
    // OK
    std::cout << std::apply(add, std::pair(1, 2)) << '\n';

    // 错误：无法推导函数类型
    // std::cout << std::apply(add_generic, std::make_pair(2.0f, 3.0f)) << '\n'; 

    // OK
    std::cout << std::apply(add_lambda, std::pair(2.0f, 3.0f)) << '\n';

    // 进阶示例
    std::tuple myTuple(25, "Hello", 9.31f, 'c');
    std::cout << myTuple << '\n';
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

int main()
{
    print_sequence(std::integer_sequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
    print_sequence(std::make_integer_sequence<int, 20>{});
    print_sequence(std::make_index_sequence<10>{});
    print_sequence(std::index_sequence_for<float, std::iostream, char>{});

    std::array<int, 4> array = { 1,2,3,4 };

    // 转换 array 为 tuple
    auto tuple = a2t(array);
    static_assert(std::is_same<decltype(tuple),
        std::tuple<int, int, int, int>>::value, "");

    // 打印到 cout
    std::cout << tuple << '\n';
}

#endif // TEST5

#ifdef TEST6

// https://blog.csdn.net/fl2011sx/article/details/119217893?spm=1001.2014.3001.5502

#include <tuple>
#include <iostream>

int main()
{
    // auto t1 = std::make_from_tuple<int,double>(1, 1.1);
}
#endif // TEST6
