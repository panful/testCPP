// alignof
// alignas

// https://www.cnblogs.com/mmmmmmmmm/p/14088273.html

/*
 * 1. sizeof(基础类型）
 * 2. 枚举类 enum class
 * 3. class 虚表
 * 4. 标准库容器
 * 5. #pragma pack 设置字节对齐
 * 6. _msize
 */

#define TEST3

#ifdef TEST1

#include <iostream>

#define SIZE_OF(x)                                                          \
    do                                                                      \
    {                                                                       \
        std::cout << "sizeof(" << #x << ") = \t" << sizeof(x) << std::endl; \
    } while (false)

int main()
{
    std::cout << "==================x64====================\n";

    SIZE_OF(bool);
    SIZE_OF(short);
    SIZE_OF(short int);
    SIZE_OF(long);
    SIZE_OF(long long);
    SIZE_OF(long long int);
    SIZE_OF(long double); // 8
    SIZE_OF(double);
    SIZE_OF(int);
    SIZE_OF(unsigned int);     // 4
#ifdef WIN32
    SIZE_OF(unsigned __int64); // 8 Linxu下不能识别 __int64
#endif                         // WIN32
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

    // 指针
    // x64都是8个字节，x86都是4个字节
    SIZE_OF(void*);
    SIZE_OF(int*);
    SIZE_OF(double*);
    SIZE_OF(char*);

    // 引用
    int number     = 1;
    int& refNumber = number;
    SIZE_OF(refNumber); // 4
    SIZE_OF(int&);      // 4
    SIZE_OF(int&&);     // 4

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

// enum class 默认类型是int
enum class A
{
};

enum class B
{
    ENUM1 = 0x00,  // int
    ENUM2 = 0xFFF, // int

    // 报错：枚举值超出int的范围
    // ENUM3 = 0xFFFFFFFFFFFFFFF, // long long
    // ENUM4 = 0xFFFFFFFFFFFFFFFF // unsigned long long
};

enum class C : long long
{
    C1 = 0,
    C2 = 1,
    C3,
    C4,
    C5,
};

enum class D : char
{
    D1 = 0,
    D2 = 1
};

// 报错：枚举类型的基础类型必须是整形
// enum class D : long double
// {
// };

int main()
{
    std::cout << "==================x64====================\n";

    std::cout << sizeof(A) << std::endl; // 枚举无论有几个值都是4，因为枚举其实就是一个int类型的数值(默认情况下)
    std::cout << sizeof(B) << std::endl; // 4
    std::cout << sizeof(C) << std::endl; // 8，枚举缺省类型是int，指定为long long长度就变为8
    std::cout << sizeof(D) << std::endl; // 1

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>

class A
{
    virtual void Func()
    {
    }
};

class B : A
{
};

class C : A
{
    virtual void Func2()
    {
    }
};

class D
{
    int a;
    char b;
};

// D的成员是私有的，也会被继承，只是不能访问
class E : public D
{
    int c;
    char d;
};

class F
{
    int arr[10] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    int* p { nullptr };
};

struct G
{
    int a;
    int& b = a;
    int& c = a;
    int& d = a;
};

class H
{
private:
    class H1
    {
    };

    class H2
    {
    };

private:
    H1 h1 {};
    H2 h2 {};
};

int main()
{
    std::cout << "==================x64====================\n";
    std::cout << sizeof(A) << std::endl; // 8 虚表指针
    std::cout << sizeof(B) << std::endl; // 8
    std::cout << sizeof(C) << std::endl; // 8
    std::cout << sizeof(D) << std::endl; // 本来为 4 + 1，1需要和4对齐 所以结果为 4 + (1 + 3) = 8
    std::cout << sizeof(E) << std::endl; // 字节对齐 4 + (1 + 3) + 4 + (1 + 3) = 16
    std::cout << sizeof(F) << std::endl; // 长度为10的数组和int型指针 4 * 10 + 8 = 48
    std::cout << sizeof(G) << std::endl; // 引用本质上是指针，所以是 4 + 4 + 8 * 3 = 32，第二个4是为了字节对齐(x64)，后面3个8是3个引用（指针）
    std::cout << sizeof(H) << std::endl; // 2，一个空类字节大小是1，两个就是2，只定义class不声明为成员变量不会增加字节数，即还是相当于空类

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define PRINT_SIZE_TYPE(var) std::cout << sizeof(var) << '\t' << typeid(var).name() << '\n';

// 以下测试都基于GCC11

int main()
{
    if (sizeof(char*) == 8)
    {
        std::cout << "--- x64 ---\n";
    }
    else
    {
        std::cout << "--- x86 ---\n";
    }

    std::cout << "---------------------------------------------\n";

    {
        std::string str0;
        std::string str1 = "abcd";
        std::string str2 = "123456789012345678901234567890";

        // std::string内部维护的是一个字符串指针，所以x64下std::string无论存储多少个字符都是固定值（MSVC和GCC不一样）
        // 可以重载一下new操作符查看std::string是怎么申请内存的
        PRINT_SIZE_TYPE(std::string); // 32
        PRINT_SIZE_TYPE(str0);        // 32
        PRINT_SIZE_TYPE(str1);        // 32
        PRINT_SIZE_TYPE(str2);        // 32
    }

    std::cout << "---------------------------------------------\n";

    {
        std::vector<int> vec1 { 1, 2, 3 };
        std::vector<double> vec2 { 1., 2., 3. };

        PRINT_SIZE_TYPE(vec1); // 24  vector内部维护的是指针
        PRINT_SIZE_TYPE(vec2); // 24
    }

    std::cout << "---------------------------------------------\n";

    {
        std::map<int, double> map1 {};
        std::map<int, double> map2 { { 1, 1.1 }, { 2, 2.2 }, { 3, 3.3 } };
        std::map<double, double> map3 {};

        PRINT_SIZE_TYPE(map1); // 48
        PRINT_SIZE_TYPE(map2); // 48
        PRINT_SIZE_TYPE(map3); // 48
    }

    std::cout << "---------------------------------------------\n";

    {
        std::array<char, 4> arr1 { 1, 2, 3, 4 };
        std::array<int, 1> arr2 {};
        std::array<int, 9> arr3 {};
        std::array<int, 1000> arr4 {};
        PRINT_SIZE_TYPE(arr1); // 1 * 4 std::array<T,Size>内部维护的是一个 T elem[Size]数组，且只有这一个成员
        PRINT_SIZE_TYPE(arr2); // 4 * 1
        PRINT_SIZE_TYPE(arr3); // 4 * 9
        PRINT_SIZE_TYPE(arr4); // 4 * 1000
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>

int main()
{
    {
        struct t
        {
            char m1;
            double m4;
            int m3;
        };

        std::cout << sizeof(t) << std::endl; // 24
    }

    {
#pragma pack(push) // 保存对齐状态    push和pop是一对,不用push和pop包住，pack(n)将会对后面的所有sizeof生效
#pragma pack(4)    // 设定为4字节对齐

        struct t
        {
            char m1;
            double m4;
            int m3;
        };

        std::cout << sizeof(t) << std::endl; // 16
#pragma pack(pop)                            // 恢复对齐状态
    }

    {
        struct t
        {
            char m1;
            double m4;
            int m3;
        };

        std::cout << sizeof(t) << std::endl; // 24
    }

    return 0;
}
#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <malloc.h>

void fun(int* p)
{
    std::cout << sizeof(p) << '\n'; // 8  (x64)

#if defined _WIN32
    // GCC11不能识别_msize
    std::cout << _msize(p) << '\n'; // 40 = 4*10
#endif
}

int main()
{
    int* p = new int[10]();
    fun(p);

    int p1[] = { 1, 2, 3 };
    // fun(p1);  //不是new或malloc出来的指针，程序会崩溃

    return 0;
}

#endif // TEST6