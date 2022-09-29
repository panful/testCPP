/*
* 1. std::sort使用lambda
* 2. 
* 
* 5. lambda各种捕获方式的区别 mutable值捕获方式修改捕获的值
*/

#define TEST1

#ifdef TEST1

#include <vector>
#include <algorithm>
#include <iostream>

bool Compare(int a, int b)
{
    std::cout << a << '\t' << b << '\n';
    //return a > b;
    return b > a;
}

int main()
{
    std::vector<int> vec{ 1,3,5,4,2,6 };
    //std::sort(vec.begin(), vec.end());
    std::sort(vec.begin(), vec.end(), Compare);
}

#endif // TEST1


#ifdef TEST5

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    {//test1 值捕获
        int i = 1;
        auto f = [i] {return i; }; //等价于 auto f = [i](){return i;};  ()可写可不写
        i = 10;
        int j = f();
        cout << j << endl;  // 1
    }

    {//test2 引用捕获
        int i = 1;
        auto f = [&i] {return i; };
        i = 10;
        int j = f();
        cout << j << endl;  // 10
    }

    {//test3 隐式值捕获
        int i = 1;
        int j = 2;
        auto f = [=] {return i + j; };
        i = 3;
        int m = f();
        cout << m << endl;  //3
    }

    {//test4 隐式引用捕获
        int i = 1;
        int j = 2;
        auto f = [&] {return i + j; };
        i = 3;
        int m = f();
        cout << m << endl;  //5
    }

    {//test5 隐式、显式混合捕获1
        int i = 1;
        int j = 2;
        //i为值捕获，j为引用捕获
        auto f = [=, &j] {return i + j; };
        i = 3;
        j = 4;
        int m = f();
        cout << m << endl;  //5
    }

    {//test5 隐式、显式混合捕获2 
        int i = 1;
        int j = 2;
        //i为引用捕获，j为值捕获
        auto f = [&, j]()->int {return i + j; };  //->int是C++返回值类型后置语法
        i = 3;
        j = 4;
        int m = f();
        cout << m << endl;  //5
    }

    {//test6 可变lambda 1
        int i = 10;
        auto f = [&i]/*()mutable*/ {return ++i; }; // 引用捕获mutable可写可不写
        i = 5;
        int j = f();
        cout << j << endl;  //6
    }

    {//test6 可变lambda 2
        int i = 10;
        auto f = [i]()mutable {return ++i; };  // 值捕获必须有mutable，不然报错，此处不能省略()
        i = 5;
        int j = f();
        cout << j << endl;  //6
    }

    {//值捕获，引用捕获均编译错误，因为i为const
        //const int i = 10;
        //auto f = [&i]() mutable {return ++i; };
        //auto f = [i]() mutable {return ++i; };
        //int j = f();
        //cout << j << endl;
    }

    {// test7 捕获列表和参数列表
     // 参数列表是需要在调用该lambda的时候传入参数，捕获列表是用来声明那些变量可以在lambda内部使用
        int i = 1;
        auto f = [i](int x) {return -(i + x); }; //参数列表：调用时需要传入一个int值，捕获列表：变量i可以在lambda内部使用
        auto j = f(3);
        cout << j << endl;  // -(1+3) = -4
    }

    {//test8 lambda的返回类型
        vector<int> ivec{ -12,2,-22,3,0 };
        //改变ivec里的值，负数变成整数                                                
        //此lambda不写返回类型没有问题.                                               
        transform(ivec.begin(), ivec.end(), ivec.begin(), [](int i) {return i < 0 ? -i : i; });

        //此lambda不写返回类型也没有问题.  
        vector<int> ret;
        ret.resize(ivec.size());
        transform(ivec.begin(), ivec.end(), ret.begin(), [](int i) {if (i < 0) return -i; else return i; });

        for (const auto& s : ret)
        {
            cout << s << " ";
        }
        cout << endl;
    }

    return 0;
}


#endif // TEST5