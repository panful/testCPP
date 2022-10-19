// c++17 https://oreki.blog.csdn.net/article/details/124345662
// 
// C++11 14 17 20 https://blog.csdn.net/qq_41854911/article/details/119657617
#define TEST1

#ifdef TEST1

/*
* auto
* decltype
* using
* Args...
* std::tuple
* {}初始化列表
* lambda
* union
* for(:)
* constexpr
*/

#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <type_traits>
#include <cstring>

template<typename T>
using str_map = std::map<std::string, T>;

struct MyPODType
{
    int a;
    char b[9];
    float c;
};

struct MyNotPODType
{
    int a;
    std::vector<char> b;
    //std::array<char, 9> b;
    float c;
};

union MyUnion
{
    //std::string s;
    float b;
    int a;
};

std::vector<int> vec{ 1,2,3,4,5,6 };

constexpr int test_constexpr()
{
    int a = 2;
    a = 3;
    //return 1;
    return a;
}

constexpr int test_constexpr(int x)
{
    int a = 2 + x * x;
    a = 3;
    //return 1;
    //return a; // 输入3返回3
    //return x; // 输入3返回3
    return 2 + x * x; // 输入3返回11
}

int main()
{
    {
        MyPODType* pod1 = new MyPODType{ 1, "111", 2.2f };
        MyPODType* pod2 = new MyPODType{ 0,"",0.0f };
        std::memcpy(pod2, pod1, sizeof(MyPODType));

        MyNotPODType* npod1 = new MyNotPODType{ 2,{'a','b','c'},2.2f };
        MyNotPODType* npod2 = new MyNotPODType{ 0, {}, 0.0f };
        std::memcpy(npod2, npod1, sizeof(MyNotPODType));

        auto ret1 = std::is_standard_layout_v<MyNotPODType>; // false
        auto ret2 = std::is_standard_layout_v<MyPODType>;    // true

        //std::memset(pod1, 0, sizeof(MyPODType));
        //std::memcpy();
        //std::strcat();
        //std::memcmp()
    }

    {
        MyUnion u;
    }

    {
        for (const auto& elem : vec)
        {
            //elem++;
        }

        for (int elem : vec)
        {
            std::cout << elem << std::endl; // 除过第一次之外输出的都是非法值
            vec.emplace_back(99); 
        }
    }

    {
        constexpr int num1 = 1 + 2 + 3;
        char arr1[num1] = { 0 };
        //num1++;

        const int num2 = 1 + 2 + 3;
        char arr2[num2] = { 0 };
        //num2++;

        //int num3 = 1 + 2 + 3;
        //char arr3[num3] = { 0 };
    }

    {
        auto ret1 = test_constexpr();
        constexpr int ret2 = test_constexpr();

        auto ret3 = test_constexpr(2);
        constexpr int ret4 = test_constexpr(3);
        std::cout << ret4 << '\n'; // 打印的是3

        const int x = ret4;
        //constexpr int x = ret4;
        //int x = ret4; // 编译不过去
        constexpr int ret5 = test_constexpr(x); // 参数x必须是常量值

        char arr[test_constexpr(5)] = { 0 };
    }

    int x = 0;
}

#endif // TEST1
