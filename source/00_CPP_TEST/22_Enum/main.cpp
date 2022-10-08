
/*
* 1. 宏定义获取枚举对应的字符串 https://zhuanlan.zhihu.com/p/388703062?utm_id=0 评论区
* 2. magic_enum https://github.com/Neargye/magic_enum 枚举转字符串第三方库
* 3. QMetaEnum 枚举转字符串 查看Qt测试程序
* 4. 枚举与或非操作 00_06_TEST1
*/

#define TEST2

#ifdef TEST1

#include <iostream>

#define COLOR_TABLE()\
DEF_COLOR(Red) \
DEF_COLOR(Blue) \
DEF_COLOR(Green)

enum Color {
#define DEF_COLOR(v) v,
    COLOR_TABLE()
#undef DEF_COLOR
};

const char* StringOf(Color c) {
    switch (c) {

#define DEF_COLOR(v) case Color::v: return #v;
        COLOR_TABLE()
#undef DEF_COLOR
    default:
        return  "";
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
    RED = 2, 
    BLUE = 4, 
    GREEN = 8 
};

int main()
{
    {
        Color color = Color::RED;
        auto color_name = magic_enum::enum_name(color);
        std::cout << color_name << '\n';
    }

    {
        std::string color_name{ "GREEN" };
        auto color = magic_enum::enum_cast<Color>(color_name);
        if (color.has_value()) {
            auto value = color.value(); // Color::GREEN
            std::cout << (int)value << '\n';
        }
    }

    {
        int color_integer = 2;
        auto color = magic_enum::enum_cast<Color>(color_integer);
        if (color.has_value()) {
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
{}

#endif // TEST3


#ifdef TEST4


#endif // TEST4