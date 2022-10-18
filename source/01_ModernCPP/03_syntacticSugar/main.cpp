/*
* 1. 结构化绑定，还支持array tuple struct，自定义类型没试过
* 2. std::is_same_v = std::is_same::value
* 3. if中初始化，执行语句
*/

#define TEST3

#include <iostream>
#include <string>
#include <map>
#include <tuple>

#ifdef TEST1

int main()
{
    std::map<int, std::string> myMap;
    myMap.emplace(1, "test1");
    myMap.emplace(2, "test2");
    myMap.emplace(3, "test3");

    std::tuple<char, int, double, std::string> myTuple{ 'a',2,2.2,"ss" };
    auto& [a, b, c, d] = myTuple;
    std::cout << a << '\t' << b << '\t' << c << '\t' << d << '\n';

    for (const auto& [key, value] : myMap)
    {
        std::cout << key << ',' << value << '\n';
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

    if (auto ret = Func())
    {
        std::cout << "true3\n";
    }
}
#endif // TEST3
