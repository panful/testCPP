
/*
* 1. C方式生成随机数
* 2. C++方式生成随机数
* 3. 随机数引擎如果是类成员，不设置种子，每次运行程序生成的随机数一样
* 4. 返回各种数据类型的极值
* 5. 二进制字面量与整形字面量分隔符
*/

#define TEST5

#ifdef TEST1

#include <iostream>

int main()
{

    //srand(ungigned int)参数不一样才可以生成不一样的随机数，
    //不然每次执行程序生成的随机数和上次程序运行生成的一样
    //time(nullptr) 返回的是当前系统时间，一直在变
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 10; i++)
    {
        std::cout << rand() % 50 << "\t";  //[0,50) 即 0 <= x < 50
    }
    std::cout << std::endl;
    for (int j = 0; j < 10; j++)
    {
        std::cout << rand() % 20 - 10 << "\t";  //[-10,10)
    }
    std::cout << std::endl;
    for (int k = 0; k < 10; k++)
    {
        //RAND_MAX = 0x7FFF
        //rand()产生一个[0,0x7FFF]即0-32767之间的随机数
        std::cout << rand() / static_cast<double>(RAND_MAX + 1) << "\t"; //[0,1)浮点数
    }
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <random>
#include <chrono>

int main()
{
    //参数表示生成随机数的种子，种子不同每次运行程序生成的随机数才能不同
    std::default_random_engine generator1(static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::default_random_engine generator2(static_cast<uint32_t>(time(nullptr)));

    std::default_random_engine generator;  //使用默认种子，每次运行程序生成的随机数相同
    std::uniform_int_distribution<int> distribution1(0, 255);  //生成[0,255]之间的int型随机数

    std::uniform_real_distribution<float> distribution2(0.0f, 1.0f);  //生成[0,1]之间的随机float值

    for (int i = 0; i < 10; i++)
    {
        std::cout << distribution1(generator);  // 0-255

        std::cout << distribution2(generator1) << "\n";

        std::cout << 1 + (int)(10.0 * (rand() / (RAND_MAX + 1.0))) << " ";  //[1,10]  不要使用rand()%10
    }
}
#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <random>
#include <chrono>

class A
{
private:
    std::default_random_engine engine;
public:
    void fun()
    {
        std::uniform_int_distribution<int> num(0, 10);
        std::cout << num(engine);
    }
};

int main()
{
    A a;
    for (int i = 0; i < 5; i++)
    {
        a.fun();
    }

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <limits>

#define MAX(x) std::cout << #x << "\tMax:\t" << std::numeric_limits<x>::max() << std::endl;
#define MIN(x) std::cout << #x << "\tMin:\t" << std::numeric_limits<x>::min() << std::endl;

int main()
{
    MAX(double);
    MIN(double);

    MAX(float);
    MIN(float);

    MAX(char);
    MIN(char);

    MAX(int);
    MIN(int);

    MAX(unsigned int);
    MIN(unsigned int);

    MAX(long);
    MIN(long);

    MAX(long long);
    MIN(long long);

    MAX(long double); // 和double一样
    MIN(long double);
}

#endif // TEST4

#ifdef TEST5

#include <iostream>

int main()
{
    // 0b或0B表示二进制
    // 0x或0X表示十六进制
    int a = 0b0001'0011'1010;
    int b = 0x00'11;
    double c = 2.34'56;

    std::cout << a << '\n' << b << '\n' << c << '\n';
}
#endif // TEST5
