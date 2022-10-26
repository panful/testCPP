/*
* 1. 结构化绑定，还支持array tuple struct，自定义类型也支持
* 2. std::is_same_v = std::is_same::value
* 3. if switch中初始化，执行语句
*/

#define TEST3

#include <iostream>
#include <string>
#include <map>
#include <tuple>

#ifdef TEST1

struct MyStruct
{
    int a{ 0 };
    float b{ 1.1f };
    char c{ 'c' };
};

// 自定义类需要实现相关的tuple_size和tuple_element和get<N>方法。
class Entry {
public:
    void Init() {
        name_ = "name";
        age_ = 10;
    }

    std::string GetName() const { return name_; }
    int GetAge() const { return age_; }
private:
    std::string name_;
    int age_;
};

template <size_t I>
auto get(const Entry& e) {
    if constexpr (I == 0) return e.GetName();
    else if constexpr (I == 1) return e.GetAge();
}

namespace std {
    template<> struct tuple_size<Entry> : integral_constant<size_t, 2> {};
    template<> struct tuple_element<0, Entry> { using type = std::string; };
    template<> struct tuple_element<1, Entry> { using type = int; };
}

int main()
{
    {
        std::map<int, std::string> myMap;
        myMap.emplace(1, "test1");
        myMap.emplace(2, "test2");
        myMap.emplace(3, "test3");

        for (const auto& [key, value] : myMap)
        {
            std::cout << key << ',' << value << '\n';
        }
    }

    {
        std::tuple<char, int, double, std::string> myTuple{ 'a',2,2.2,"ss" };
        auto& [a, b, c, d] = myTuple;
        std::cout << a << '\t' << b << '\t' << c << '\t' << d << '\n';
    }

    {
        MyStruct s{ 99,8.8f,'x' };
        auto [x, y, z] = s;
        std::cout << x << ',' << y << ',' << z << '\n';
    }

    {
        Entry e;
        e.Init();
        auto [name, age] = e;
        std::cout << name << " " << age << std::endl; // name 10
    }

    {
        //constexpr auto [x, y] = std::pair(1, 2.3f); // compile error, C++20可以
    }


}

#endif // TEST1

#ifdef TEST2


template <typename T>
std::string convert(T input)
{
    // 必须有constexpr
    if constexpr (std::is_same_v<T, const char*>)
    {
        std::cout << "const char*\n";
        return input;
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        std::cout << "std::string\n";
        return input;
    }
    else
    {
        std::cout << "else\n";
        return std::to_string(input);
    }
}

int main()
{
    auto ret1 = convert("ss");
    auto ret2 = convert(std::string("ss"));
    const std::string str1("ss");
    std::string str2("22");
    auto ret3 = convert(str1);
    auto ret4 = convert(str2);

    // 编译不过去，没有std::to_string(char*)
    const char* sz = "1234";
    auto sz1 = const_cast<char*>(sz);
    //auto ret4 = convert(sz1);

    //std::to_string(sz1);
}

#endif // TEST2

#ifdef TEST3

int* Func()
{
    return nullptr;
}

std::map<int, int> myMap;

int main()
{
    if (int i = 0; true)
    {
        std::cout << "true1\n";
    }

    if (auto it = myMap.find(1); 1 == 1)
    {
        if (it == myMap.end())
            std::cout << "true2\n";
    }

    if (auto ret = Func(); ret == nullptr)
    {
        std::cout << "true3\n";
    }

    if (auto ret = Func(); ret != nullptr)
    {
        // 不会进来
        std::cout << "true4\n";
    }

    // switch()中的变量只能是整形或枚举
    switch (int a = 2, b = 3; a)
    {
    case 2:
        std::cout << "case 2\n";
        break;
    case 3:
        std::cout << "case 3\n";
    default:
        std::cout << "default\n";
        break;
    }

    return 0;
}
#endif // TEST3
