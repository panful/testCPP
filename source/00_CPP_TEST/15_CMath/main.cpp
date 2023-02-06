

#define TEST1

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

    std::cout << "------------------------------------------\n";

    // 正弦，余弦，正切
    // 参数都是弧度
    MATH(std::sin(pi / 6.));  // pi/6 = 30°
    MATH(std::sin(pi / 2.));
    MATH(std::sin(pi * 1.1));
    MATH(std::cos(pi / 3.));
    MATH(std::cos(-pi));
    MATH(std::tan(pi / 6.));

    std::cout << "------------------------------------------\n";

    // 反正弦，反余弦，反正切
    // 返回的都是弧度
    MATH(std::acos(-1));        // cos(-180°) = -1
    MATH(std::acos(0.5));       // cos(pi/3)即cos(60°) = 0.5，所以acos(0.5) = pi/3
    MATH(std::asin(1));         // sin(90°) = 1
    MATH(std::asin(0.5));       // sin(pi/6)即sin(30°) = 0.5，所以asin(0.5) = pi/6

    std::cout << "------------------------------------------\n";


    MATH(std::pow(2, 3));       // 求幂
    MATH(std::sqrt(9));         // 平方根
    MATH(std::hypot(3, 4));     // hypot(x,y) = sqrt(x*x+y*y)
    MATH(std::cbrt(8));         // 立方根

    std::cout << "------------------------------------------\n";

    MATH(std::abs(-5.5));       // 绝对值
    MATH(std::abs(9.9));
    MATH(std::fmod(5, 2));      // 注意区分和std::modf的区别  double modf(double x, double *ipart)：将x的小数部分返回，整数部分存放到ipart
    MATH(std::fmod(2.2, 2));
    MATH(std::floor(9.456));    // 向下取整
    MATH(std::ceil(9.456));     // 向上取整

    std::cout << "------------------------------------------\n";

    MATH(std::exp(2));          // e的2次方
    MATH(std::log(e*e));        // 计算以e为底的对数
    MATH(std::log(std::exp(3)));
    MATH(std::log10(1000));     // 计算以10为底的对数

    std::cout << "------------------------------------------\n";




    return 0;
}

#endif // TEST1

#ifdef TEST2

int main()
{}


#endif // TEST2

