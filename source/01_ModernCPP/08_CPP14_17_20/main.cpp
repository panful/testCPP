// c++17 https://oreki.blog.csdn.net/article/details/124345662
// 
// C++11 14 17 20 https://blog.csdn.net/qq_41854911/article/details/119657617
#define TEST1

#ifdef TEST1

/*
* ============C++11================
* auto
* decltype                  01_05_TEST2
* using                     00_13_TEST10 别名模板
* Args...                   00_03 可变参数模板
* std::tuple                01_07 
* {}                        初始化列表
* lambda                    02_02_01
* union                     01_02_TEST8 POD
* for(:)                    00_17_TEST3 基于范围的for循环
* constexpr                 00_10
* long long                 00_18_TEST4
* rvale                     02_02_01 02_07
* std::move std::forward    02_02_01 02_07 移动语义 完美转发
* nullptr                   00_05
* std::unique+shared_ptr    00_05
* ============C++14================
* 返回值类型推导
* lambda参数可以使用auto
* 变量模板                   00_13_TEST2
* constexpr减少限制
* [[deprecated]]标记         00_17_TEST4
* 二进制字面量                00_18_TEST5
* std::shared_lock 读写锁    00_04_TEST202
* std::integer_sequence     01_07_TEST5
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

    }


}

#endif // TEST1
