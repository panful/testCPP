/*
* 1. enum枚举的按位与或非
* 2. 
* 3. 位运算符 & | ！ >> <<
* 4. functional头文件中的位操作函数，逻辑操作函数
*/

#define TEST1

#ifdef TEST1

enum class TEST_ENUM1 :int
{
    ENUM1 = 0x00,
    ENUM2 = 0x01,
    ENUM3 = 0x02,
    ENUM4 = 0x04,
};

enum TEST_ENUM2 :int
{
    ENUM1 = 0x00,
    ENUM2 = 0x01,
    ENUM3 = 0x02,
    ENUM4 = 0x04,
};

#define tt1 0x00
#define tt2 0x01

int main()
{
    auto ret1 = tt1 & tt2;
    auto ret2 = TEST_ENUM2::ENUM1 | TEST_ENUM2::ENUM3;

    //auto ret3 = TEST_ENUM1::ENUM1 & TEST_ENUM1::ENUM2; // 错误
}
#endif // TEST1

#ifdef TEST2
// https://www.cnblogs.com/mmmmmmmmm/p/14586206.html


#endif // TEST2

#ifdef TEST3

#include <iostream>

int main()
{
    auto c0 = 0x01020304 >> 8;  //c0 = 0x010203 类型为int

    unsigned int c1 = 0x01020304 >> 16;  //c1 = 0x0102
    unsigned char c2 = 0x01020304 >> 16; //c2 = 0x02   取uint的最低8位


    uint32_t ui1 = 3;
    uint32_t ui2 = 5;
    auto ret1 = ui1 | ui2;  //ret1 = 7  按位或（二进制）

    ui1 |= ui2;  //ui1 = ui1|ui2;

    int test = 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <functional>

int main()
{
    std::bit_or<int> a;
    auto ret1 = a(2, 5);  // 等价于 auto ret1 = 2|5

    std::logical_and<bool> b;
    auto ret2 = b(false, true); //等价于 auto ret2 = false && true;

    auto ret3 = std::not_fn(ret2); //等价于 auto ret3 = !ret2;

    std::plus<int> c;
    auto ret4 = c(2, 4);  //等价于 auto ret4 = 2 + 4;

    //std::equal_to d;  //可以自动识别类型
    std::equal_to<int> d;
    auto ret5 = d(2, 3); //等价于 auto ret5 = (2 == 3);

    std::ranges::equal_to e;  // C++ 20
    auto ret6 = e(2, 3.3);  //    auto ret6 = (2 == 3.3);
    auto ret7 = e(2, 'b');  //    auto ret7 = (2 == 'b');

    return 0;
}
#endif // TEST4
