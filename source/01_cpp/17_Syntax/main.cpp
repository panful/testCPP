
/*
* 1. 三目运算符多层嵌套
* 2. ||的第一个条件满足后，不会再判断第二个条件
* 3. for基于范围的循环遍历，右值引用，修改for循环元素
* 4. [[deprecated]]标记该函数或类等不建议使用 [[noreturn]]标记函数不应该有返回值
* 5. __has_include预处理表达式
* 6. C++17新增Attribute TEST4
* 7. 嵌套for循环 break continue
*/

#define TEST7

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

// ()中的是warning的提示信息，默认为'***': 被声明为已否决
[[deprecated("use new func instead")]] void func1() {}
void func2() {}

[[noreturn]] void func3() {}
[[noreturn]] int func4() { return 1; } // 会有警告信息

int main() 
{
    A a;
    B b;

    func1();
    func2();

    //auto r1 = func3(); // error
    auto r2 = func4();   // ok

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>

#if defined __has_include
# if __has_include(<direct.h>)
#include <direct.h>
#  define has_direct_h 1
# elif __has_include(<unistd.h>)
#include <unistd.h>
#  define has_unistd_h 1
# endif
#endif

int main()
{
#if defined has_direct_h
    std::cout << "windows\n";
#elif has_unistd_h
    std::cout << "linux\n";
#endif 

    std::cout << getcwd(0, 0) << '\n';

    return 0;
}

#endif // TEST5

#ifdef TEST6

/*
* [[fallthrough]]，用在switch中提示可以直接落下去，不需要break，让编译期忽略警告
* [[nodiscard]] ：表示修饰的内容不能被忽略，可用于修饰函数，标明返回值一定要被处理
* [[maybe_unused]] ：提示编译器修饰的内容可能暂时没有使用，避免产生警告
*/

#include <iostream>

enum class MyEnumClass
{
    E1,
    E2,
    E3,
    E4
};

struct MyStruct
{
    int a{ 0 };
};

union MyUnion
{
    int a{ 0 };
    float b;
};

void func1(MyEnumClass myEnum)
{
    switch (myEnum)
    {
    case MyEnumClass::E1:
        // do something       // warning
    case MyEnumClass::E2:
        // do something
        [[fallthrough]];      // 警告消除
    case MyEnumClass::E3:
        break;
    case MyEnumClass::E4:
        break;
    default:
        break;
    }
}

int func2() { return 9; }
[[nodiscard]] int func3() { return 9; }
void F() {
    func2(); // 不会有警告信息
    func3(); // warning 没有处理函数返回值
}

void func4() {}

int main()
{
    {
        func1(MyEnumClass::E1);
        func1(MyEnumClass::E2);
        func1(MyEnumClass::E3);
        func1(MyEnumClass::E4);
    }

    {
        MyUnion u1; // warning:未引用的局部变量
        [[maybe_unused]] MyUnion u2; // 消除warning
    }

    return 0;
}


#endif // TEST6

#ifdef TEST7

#include <iostream>

int main()
{
    constexpr size_t row = 5;
    constexpr size_t col = 5;
    int test[row][col]{ 0 };
    static int number = 0;
    for (size_t i = 0; i < row; ++i)
    {
        for (size_t j = 0; j < col; ++j)
        {
            test[i][j] = number++;
            std::cout << test[i][j] << '\t';
        }
        std::cout << '\n';
    }
    std::cout << "-------------------------------------------\n";

    bool bFind{ false };

    for (size_t i = 0; i < row; ++i)
    {
        if (bFind)
        {
            break;
        }
        for (size_t j = 0; j < col; ++j)
        {
            std::cout << "row: " << i << "\tcol: " << j << '\t';
            if (test[i][j] == 12)
            {
                bFind = true;
                std::cout << "success\n";
                break; // 只能跳出一层循环
            }
            else
            {
                std::cout << "\n";
            }
        }
    }

}


#endif // TEST7
