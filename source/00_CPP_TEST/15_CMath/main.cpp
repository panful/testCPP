

#define TEST2

#ifdef TEST1

#include <iostream>
#include <cmath>
#include <numbers>

// std::numbers::pi  c++20 3.1415926

constexpr double pi = 3.14159265359;
constexpr double e = 2.718281828459;

#define MATH(m) do                       \
{                                        \
    std::cout<<#m<<'\t'<<m<<std::endl;   \
}while (false);              

int main()
{
    MATH(std::numbers::pi);
    MATH(std::numbers::pi_v<double>);

    MATH(std::sin(pi / 6.));  // pi/6 = 0°
    MATH(std::sin(pi / 2.));
    MATH(std::sin(pi * 1.1));
    MATH(std::cos(pi / 3.));
    MATH(std::tan(pi / 4.));

    MATH(std::pow(2, 3));// 求幂
    MATH(std::sqrt(9));  // 平方根
    MATH(std::hypot(3, 4));
    MATH(std::cbrt(8));  // 立方根

    MATH(std::abs(-5.5));
    MATH(std::abs(9.9));
    MATH(std::fmod(5, 2));  // 注意区分和std::modf的区别  double modf(double x, double *ipart)：将x的小数部分返回，整数部分存放到ipart
    MATH(std::fmod(2.2, 2));

    MATH(std::exp(2));      // e的2次方
    MATH(std::log(e*e));    // 计算以e为底的对数
    MATH(std::log(std::exp(3)));
    MATH(std::log10(1000)); // 计算以10为底的对数

    MATH(std::acos(-1)); // 反余弦函数
    MATH(std::acos(pi / 6.));

    return 0;
}

#endif // TEST1

#ifdef TEST2

int main()
{}


#endif // TEST2

