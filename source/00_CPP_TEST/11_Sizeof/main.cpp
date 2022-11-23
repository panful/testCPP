//alignof
//alignas

/*
* 1. sizeof(基础类型）
* 2. sizeof(class)
*/

#define TEST2

#ifdef TEST1

#include <iostream>

#define SIZE_OF(x) do         \
{                             \
std::cout<<"sizeof("          \
<< #x <<") = \t"              \
<< sizeof(x)                  \
<< std::endl;                 \
}while(false)                 \

int main()
{
    std::cout << "==================x64====================\n";

    SIZE_OF(bool);
    SIZE_OF(short);
    SIZE_OF(short int);
    SIZE_OF(long);
    SIZE_OF(long long);
    SIZE_OF(long long int);
    SIZE_OF(long double);      // 8
    SIZE_OF(double);
    SIZE_OF(int);
    SIZE_OF(unsigned int);     // 4
#ifdef WIN32
    SIZE_OF(unsigned __int64); // 8 Linxu下不能识别 __int64
#endif // WIN32
    SIZE_OF(size_t);           // Windows = 8;Linux = 8
    SIZE_OF(int64_t);
    SIZE_OF(int32_t);
    SIZE_OF(int16_t);
    SIZE_OF(int8_t);
    SIZE_OF(uint8_t);
    SIZE_OF(unsigned char);
    SIZE_OF(char);
    SIZE_OF(float);

    std::cout << "==================x64====================\n";

    // x64都是8个字节
    SIZE_OF(void*);
    SIZE_OF(int*);
    SIZE_OF(double*);
    SIZE_OF(char*);

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <string>

enum class A
{};

enum class AA {
    ENUM1 = 0x00,              // int
    ENUM2 = 0xFFF,             // int
    
    // 报错：枚举值超出int的范围
    //ENUM3 = 0xFFFFFFFFFFFFFFF, // long long
    //ENUM4 = 0xFFFFFFFFFFFFFFFF // unsigned long long
};

class B
{};

enum class C:long long
{
    C1 = 0,
    C2 = 1,
    C3,
    C4,
    C5,
};

class D
{
    int a;
    char b;
};

class E :public D
{
    int c;
    char d;
};

class F
{
    int arr[10]{ 1,2,3,4,5,6,7,8,9,0 };
    int* p{ nullptr };
};

int main()
{
    std::cout << "==================x64====================\n";

    std::cout << sizeof(A) << std::endl; // 枚举无论有几个值都是 4
    std::cout << sizeof(AA) << std::endl; // 4，即使枚举值为long long，长度仍为4
    std::cout << sizeof(B) << std::endl; // 空类始终为1
    std::cout << sizeof(C) << std::endl; // 8，枚举缺省类型是int，指定为long long长度就变为8
    std::cout << sizeof(D) << std::endl; // 本来为 4+1，1需要和4对齐 所以结果为 4+1+3 = 8
    std::cout << sizeof(E) << std::endl; // 字节对齐 4+1+3+4+1+3 = 16
    std::cout << sizeof(F) << std::endl; // 长度为10的数组和int型指针，4*10+8 = 48

    std::cout << "==================x64====================\n";

    std::string str0;
    std::string str1 = "abc";
    std::string str2 = "1234567890abcd1234567890";

    std::cout << sizeof(std::string) << std::endl;  // 40 std::string内部维护的是一个字符串指针，所以x64下std::string无论存储多少个字符都是40字节
    std::cout << sizeof(str0) << std::endl;         // 40
    std::cout << sizeof(str1) << std::endl;         // 40
    std::cout << sizeof(str2) << std::endl;         // 40

    std::cout << "==================x64====================\n";

    std::vector<int> vec1{ 1,2,3 };
    std::vector<double> vec2{ 1.,2.,3. };

    std::cout << sizeof(vec1) << std::endl; // 32  vector内部维护的是指针
    std::cout << sizeof(vec2) << std::endl; // 32

    std::cout << "==================x64====================\n";

    std::cout << sizeof(std::map<int,double>) << std::endl;    // 24
    std::cout << sizeof(std::map<double,double>) << std::endl; // 24
    std::cout << sizeof(std::map<char,char>) << std::endl;     // 24

    std::cout << "==================x64====================\n";

    std::cout << sizeof(std::array<char,3>) << std::endl;      // 3 std::array<T,Size>内部维护的是一个 T elem[Size]数组，且只有这一个成员（MSVC）
    std::cout << sizeof(std::array<int,1>) << std::endl;       // 4
    std::cout << sizeof(std::array<double,2>) << std::endl;    // 16

    return 0;
}

#endif // TEST2
