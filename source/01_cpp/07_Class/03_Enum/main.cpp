
/*
 * 1. 宏定义获取枚举对应的字符串 https://zhuanlan.zhihu.com/p/388703062?utm_id=0 评论区
 * 2. magic_enum https://github.com/Neargye/magic_enum 枚举转字符串第三方库
 * 3. QMetaEnum 枚举转字符串 查看Qt测试程序
 * 4. 枚举与或非操作 00_06_TEST1
 * 5. 匿名枚举，std::get<Enum>()
 * 6. enum 和 enum class 类型转换
 * 7. sizeof(enum) 00_11_TEST2
 */

#define TEST1

#ifdef TEST1

#include <iostream>

#define COLOR_TABLE() \
    DEF_COLOR(Red)    \
    DEF_COLOR(Blue)   \
    DEF_COLOR(Green)

enum class Color
{
#define DEF_COLOR(v) v,
    COLOR_TABLE()
#undef DEF_COLOR
};

const char* StringOf(Color c)
{
    switch (c)
    {
#define DEF_COLOR(v) \
    case Color::v:   \
        return #v;
        COLOR_TABLE()
#undef DEF_COLOR
    default:
        return "";
    }
}

int main()
{
    auto c1 = StringOf(Color::Green);
    auto c2 = StringOf((Color)0);

    std::cout << c1 << '\t' << c2 << '\n';
}

#endif // TEST1

#ifdef TEST2

#include "magic_enum.hpp"
#include <iostream>

enum class Color
{
    RED   = 2,
    BLUE  = 4,
    GREEN = 8
};

// magin_enum 解析，实现一个magic_enum
// https://blog.csdn.net/albertsh/article/details/126214355

int main()
{
    {
        Color color     = Color::RED;
        auto color_name = magic_enum::enum_name(color);
        std::cout << color_name << '\n';
    }

    {
        std::string color_name { "GREEN" };
        auto color = magic_enum::enum_cast<Color>(color_name);
        if (color.has_value())
        {
            auto value = color.value(); // Color::GREEN
            std::cout << (int)value << '\n';
        }
    }

    {
        int color_integer = 2;
        auto color        = magic_enum::enum_cast<Color>(color_integer);
        if (color.has_value())
        {
            auto value = color.value(); // Color::RED
            std::cout << (int)value << '\n';
        }
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

// QMetaEnum
int main()
{
}

#endif // TEST3

#ifdef TEST4

#include <iostream>

enum class ENUM_1
{
    E1 = 0x01,
    E2 = 0x02,
    E3 = 0x04,
    E4 = 0x08,
};

enum ENUM_2
{
    E1 = 0x01,
    E2 = 0x02,
    E3 = 0x04,
    E4 = 0x08,
};

#define E_1 0x01
#define E_2 0x02
#define E_3 0x04
#define E_4 0x08

// 重载位操作符就可以像enum一样使用按位与或非
ENUM_1 operator|(ENUM_1 e1, ENUM_1 e2)
{
    return static_cast<ENUM_1>(static_cast<uint32_t>(e1) | static_cast<uint32_t>(e2));
}

// ^ 异或 相同为0，相异为1
// & 与   有0则为0
// | 或   有1则为1

int main()
{
    {
        // auto r1 = ENUM_1::E1 & ENUM_1::E2; // error 因为ENUM_1::E1 和 ENUM_1::E2是ENUM类型的，并不是int或其他整形
        auto r1 = ENUM_1::E3 | ENUM_1::E4; // E3|E4 = 12  重载了operator |

        ENUM_1 test = ENUM_1::E1 | ENUM_1::E2; // E1|E2 = 3 走的是default分支

        switch (test)
        {
        case ENUM_1::E1:
            std::cout << "E1\n";
            break;
        case ENUM_1::E2:
            std::cout << "E2\n";
            break;
        case ENUM_1::E3:
            std::cout << "E3\n";
            break;
        case ENUM_1::E4:
            std::cout << "E4\n";
            break;
        default:
            std::cout << "ELSE\n";
            break;
        }
    }

    std::cout << "--------------------\n";

    {
        auto testEnum = [](ENUM_2 x) {
            if (x & ENUM_2::E1)
            {
                std::cout << "E1\n";
            }
            if (x & ENUM_2::E2)
            {
                std::cout << "E2\n";
            }
            if (x & ENUM_2::E3)
            {
                std::cout << "E3\n";
            }
            if (x & ENUM_2::E4)
            {
                std::cout << "E4\n";
            }
        };

        testEnum(ENUM_2::E1);
        testEnum(ENUM_2::E2);
        testEnum(ENUM_2::E3);
        testEnum(ENUM_2::E4);
    }
    std::cout << "--------------------\n";

    {
        auto r2 = ENUM_2::E1 & ENUM_2::E2;
        auto r3 = ENUM_2::E2 ^ ENUM_2::E3;
        auto r4 = ENUM_2::E3 | ENUM_2::E4;
    }
    std::cout << "--------------------\n";

    {
        auto r5 = E_1 & E_2;
        auto r6 = E_2 ^ E_3;
        auto r7 = E_3 | E_4;
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <string>
#include <tuple>

namespace MyEnum {
enum
{
    Int,
    Float,
    String,
};
}

int main()
{
    {
        enum
        {
            Sun,
            Mon,
            Tue,
            Wed,
            Thur,
            Fri,
            Sat
        };

        std::string week[] { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };

        auto theSun = week[Sun];
        auto theMon = week[Mon];
    }

    {
        std::tuple<int, float, std::string> myTuple { 1, 2.2f, "string" };

        auto theInt    = std::get<MyEnum::Int>(myTuple);
        auto theFloat  = std::get<MyEnum::Float>(myTuple);
        auto theString = std::get<MyEnum::String>(myTuple);
    }

    return 0;
}

#endif // TEST5

#ifdef TEST6

/*
C++98 的 enum 是“非域化的”；而 C++11 的 enum class 是“域化的”，限制了枚举成员只在域内可见
enum class 的缺省潜在类型 (underlying type) 是 int 型，而 enum 没有缺省潜在类型
enum class 一般总是前置声明，而 enum 只有在指定了潜在类型时才可以是前置声明
*/

#include <iostream>

void f1(size_t x)
{
    std::cout << x << '\n';
}

enum ENUM_1
{
    red,
    green,
    blue
};

enum class ENUM_2
{
    red_c,
    green_c,
    blue_c
};

// auto red = 1; // error red重定义
auto red_c = 1; // ok

// 枚举转换
constexpr size_t EnumConvert(ENUM_2 e) noexcept
{
    return static_cast<size_t>(e);
}

template <typename E>
constexpr typename std::underlying_type<E>::type toUType(E enumerator) noexcept
{
    return static_cast<typename std::underlying_type<E>::type>(enumerator);
}

int main()
{
    f1(red);

    f1(red_c); // 实参是全局变量red_c
    // f1(ENUM_2::red_c); // error 类型不兼容
    f1(static_cast<size_t>(ENUM_2::red_c)); // ok

    f1(EnumConvert(ENUM_2::green_c));

    f1(toUType(ENUM_2::blue_c));
    f1(toUType(ENUM_2::blue_c));

    return 0;
}

#endif // TEST6
