
/*
* 1. 三目运算符多层嵌套
* 2. ||的第一个条件满足后，不会再判断第二个条件
* 3. for基于范围的循环遍历，右值引用，修改for循环元素
* 4.[[deprecated]]标记
*/

#define TEST4

#ifdef TEST1

#include <iostream>

int main()
{
    std::cout << "test\n";

    int a = 1, b = 2, c = 3, d = 4, e = 5;
    // a如果等于2则执行b==2? 否则执行ret = e
    auto ret = a == 2 ? b == 2 ? c : d : e;
    std::cout << ret;

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

int get(int i)
{
    return i;
}
int main()
{
    // "||"的第一个条件满足就不会再去判断第二个条件
    // 第一个条件不满足才会再去判断第二个条件
    if (get(2) == 3 || get(3) == 3)
    {
        std::cout << "test";
    }
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <vector>

class Test
{
public:
    Test() { std::cout << " 111 construct\n"; }
    ~Test() { std::cout << " --- destruct\n"; }

    Test& operator=(const Test&)noexcept { std::cout << " 222 operator=\n"; return *this; }
    Test& operator=(const Test&&)noexcept { std::cout << " 888 operator=\n"; return *this; }
    Test& operator=(Test&&)noexcept { std::cout << " 333 operator=\n"; return *this; }
    Test& operator=(Test&)noexcept { std::cout << " 999 operator=\n"; return *this; }

    Test(const Test&) { std::cout << " 444 copy construct\n"; }
    Test(const Test&&)noexcept { std::cout << " 555 move construct\n"; }
    Test(Test&&)noexcept { std::cout << " 666 move construct\n"; }
    Test(Test&)noexcept { std::cout << " 777 copy construct\n"; }
};

std::vector<int> func()
{
    return { 1,2,3,4,5 };
}

int main()
{
    {
        Test t1, t2, t3;
        std::vector<Test> vec;
        std::cout << "==============\n";
        //vec.reserve(3);
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);

        // 遍历不会调用任何构造函数
        for (auto&& elem : vec)
        {

        }
        std::cout << "=====================\n";
        for (auto& elem : vec)
        {

        }
        std::cout << "=====================\n";
        for (const auto& elem : vec)
        {

        }
        std::cout << "=====================\n";
        for (const auto elem : vec)
        {

        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        for (auto elem : vec)
        {
            elem = 3;
        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        // 可以修改vec的元素
        for (auto& elem : vec)
        {
            elem = 3;
        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        // 可以修改vec的元素
        for (auto&& elem : vec)
        {
            elem = 3;
        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        // 
        for (const auto& elem : vec)
        {
            // 报错：表达式必须是可修改的左值
            //elem = 3;
        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        // 报错：无法将右值引用绑定到左值
        //for (const auto&& elem : vec)
        {

        }
    }
    std::cout << "--------------------\n";
    {
        std::vector<int> vec{ 1,2,3,4,5,6,7 };

        for (const auto& elem : vec)
        {
            //elem++;  // error 常引用，不可以修改
        }

        for (int elem : vec)
        {
            std::cout << elem << std::endl; // 除过第一次之外输出的都是非法值
            vec.emplace_back(99);
            // 容器尾部插入元素，会导致遍历迭代器失效
            // 可以查看vector底层实现机制
        }
    }
    {
        // func()函数只会被执行一次
        for (auto elem : func())
        {
            std::cout << elem << '\t';
        }
        std::cout << "\n-----------------------\n";
        // elem是char类型
        for (auto elem : "abcdefg")
        {
            std::cout << elem << '\t';
        }
    }
}

#endif // TEST3

#ifdef TEST4

// C++14中增加了deprecated标记，修饰类、变、函数等，当程序中使用到了被其修饰的代码时，
// 编译时被产生警告，用户提示开发者该标记修饰的内容将来可能会被丢弃，尽量不要使用。

struct [[deprecated]] A { };

struct B { };

int main() 
{
    A a;
    B b;
    return 0;
}

#endif // TEST4
