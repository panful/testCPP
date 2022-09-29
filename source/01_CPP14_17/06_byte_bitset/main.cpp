/*
1. std::bitset
2. std::hash + std::bitset 对std::bitset做哈希操作
3. std::byte
4. 位移
*/

#define TEST4

#ifdef TEST1

#include <bitset>
#include <iostream>
using namespace std;

int main()
{
    try
    {
        bitset<4> b1(123);  //十进制123 = 二进制01111011，所以b1是1101（从第0个元素开始算起）
        //bitset<4> b2("ss");  //抛出异常
        bitset<4> b3(0x4); //16进制4 = 二进制100，b3 = 0010，高位不够用0补齐
        bitset<4> b4("11110000"); //只取4位 b4 = 1111

        //auto ret1 = b4[55];      //如果越界直接中断

        bitset<8> foo("10011011");

        cout << foo.count() << endl;  //5（count函数用来求bitset中1的位数，foo中共有５个１
        cout << foo.size() << endl;   //8（size函数用来求bitset的大小，一共有８位

        cout << foo.test(0) << endl;  //true（test函数用来查下标处的元素是０还是１，并返回false或true，此处foo[0]为１，返回true
        cout << foo.test(2) << endl;  //false（同理，foo[2]为０，返回false

        cout << foo.any() << endl;    //true（any函数检查bitset中是否有１
        cout << foo.none() << endl;   //false（none函数检查bitset中是否没有１
        cout << foo.all() << endl;    //false（all函数检查bitset中是全部为１

        bitset<4> bit1(string("1001"));
        bitset<4> bit2(string("0011"));

        cout << (bit1 ^= bit2) << endl;       // 1010 (bit1对bit2按位异或后赋值给bit1)
        cout << (bit1 &= bit2) << endl;       // 0010 (按位与后赋值给bit1)
        cout << (bit1 |= bit2) << endl;       // 0011 (按位或后赋值给bit1)

        cout << (bit1 <<= 2) << endl;        // 1100 (左移２位，低位补０，有自身赋值)
        cout << (bit1 >>= 1) << endl;        // 0110 (右移１位，高位补０，有自身赋值)

        cout << (~bit2) << endl;             // 1100 (按位取反)
        cout << (bit2 << 1) << endl;         // 0110 (左移，不赋值)
        cout << (bit2 >> 1) << endl;         // 0001 (右移，不赋值)

        cout << (bit1 == bit2) << endl;       // false (0110==0011为false)
        cout << (bit1 != bit2) << endl;       // true  (0110!=0011为true)

        cout << (bit1 & bit2) << endl;        // 0010 (按位与，不赋值)
        cout << (bit1 | bit2) << endl;        // 0111 (按位或，不赋值)
        cout << (bit1 ^ bit2) << endl;        // 0101 (按位异或，不赋值)

        bitset<8> bit3("10011011");

        cout << bit3.flip(2) << endl;    //10011111　　（flip函数传参数时，用于将参数位取反，本行代码将bit3下标２处"反转"，即０变１，１变０
        cout << bit3.flip() << endl;     //01100000　　（flip函数不指定参数时，将bitset每一位全部取反

        cout << bit3.set() << endl;      //11111111　　（set函数不指定参数时，将bitset的每一位全部置为１
        cout << bit3.set(3, 0) << endl;  //11110111　　（set函数指定两位参数时，将第一参数位的元素置为第二参数的值，本行对bit3的操作相当于bit3[3]=0
        cout << bit3.set(3) << endl;     //11111111　　（set函数只有一个参数时，将参数下标处置为１

        cout << bit3.reset(4) << endl;   //11101111　　（reset函数传一个参数时将参数下标处置为０
        cout << bit3.reset() << endl;    //00000000　　（reset函数不传参数时将bitset的每一位全部置为０

        bitset<8> bit4("10011011");

        string s = bit4.to_string();             //将bitset转换成string类型
        unsigned long a = bit4.to_ulong();       //将bitset转换成unsigned long类型
        unsigned long long b = bit4.to_ullong(); //将bitset转换成unsigned long long类型

        cout << s << endl; //10011011
        cout << a << endl; //155
        cout << b << endl; //155

        int test = 0;
    }
    catch (...)
    {
        cout << "error\n";
    }
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <bitset>
#include <functional>

int main()
{
    std::bitset<4> b1(1);
    std::bitset<4> b2(2);
    std::bitset<4> b3(2);
    std::bitset<4> b4(b2);

    std::hash<std::bitset<4>> hash_fn;

    //b2,b3,b4的hash值相同
    std::cout << hash_fn(b1) << '\n';
    std::cout << hash_fn(b2) << '\n';
    std::cout << hash_fn(b3) << '\n';
    std::cout << hash_fn(b4) << '\n';

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <cstddef> // for std::byte
 // 0x为十六进制，0b为二进制
 // std::byte存储十进制 0-255
int main()
{
    std::byte b1{ 0x3F }; // 16*3+15 = 63
    std::byte b2{ 0b1111'0000 }; // 0xF0 = 240
    std::byte b4[4]{ b1, b2, std::byte{1} }; // 4 bytes (last is 0)

    std::cout << std::to_integer<int>(b1) << '\n';
    std::cout << std::to_integer<int>(b2) << '\n';

    //列表初始化(使用大括号)是直接初始化std::byte对象的单个值的唯一方法。
    std::byte b5{ 42 }; // OK (as for all enums with fixed underlying type since C++17)
    //std::byte b6(42); // ERROR
    //std::byte b7 = 42; // ERROR
    //std::byte b8 = { 42 }; // ERROR
}


#endif // TEST3

#ifdef TEST4

int main()
{
    unsigned char c1 = 255;
    auto ret1 = c1 >> 1;
    auto ret2 = c1 >> 2;
    auto ret3 = c1 >> 3;
    auto ret4 = c1 >> 7;
    auto ret5 = c1 >> 8; // 数据将丢失
}


#endif // TEST4

