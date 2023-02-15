/*
* 1. main函数中的 argc argv
* 2. 查看C++版本
* 3. 宏定义函数
* 4. Windows下C++程序打开可执行程序exe，网页
* 5. std::string使用条件断点
* 6. Windows下读取并修改控制台颜色
*/

#define TEST2

#ifdef TEST1

#include <iostream>

// argc表示程序运行时，以空格间隔的参数个数
// argv依次存放所有的参数字符串
// 在dos环境下依次输入：00_01_test 1 2 3 enter，此时argc = 4, argv= {"1","2","3"}
int main(int argc, char** argv)
{
    std::cout << "----------------------\n";

    for (int i = 0; i < argc; ++i)
    {
        std::cout << i << '\t' << argv[i] << '\n';
    }

    std::cout << "----------------------\n";

    system("pause");
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
// g++ 指定c++标准: g++ -std=c++17 main.cpp -o o
int main()
{
#if defined __GNUC__
    std::cout<<"the platform is linux\n";
#elif defined _WIN32
    std::cout << "the platform is windows\n";
#endif

    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    
#if __cplusplus == 201103L
    std::cout << "C++11\n";
#elif __cplusplus == 201402L
    std::cout << "C++14\n";
#elif __cplusplus == 201703L
    std::cout << "C++17\n";
#elif __cplusplus == 202002L
    std::cout << "C++20\n";
#endif

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>

#define TEST(e) void e(){std::cout<<"test\n";}

TEST(ss);

int main()
{
    ss();
}

#endif // TEST3

#ifdef TEST4

#include <Windows.h>
#include <thread>
#include <chrono>

int main()
{
    //调用计算器
    //ShellExecute(0, (LPCWSTR)L"open", (LPCWSTR)L"CALC.EXE", (LPCWSTR)L"", (LPCWSTR)L"", SW_SHOWNORMAL);
    ShellExecute(0, "open", "calc.exe", "", "", SW_SHOWNORMAL);
    std::this_thread::sleep_for(std::chrono::seconds(5)); //等待5秒
    //调用记事本
    ShellExecute(0, "open", "notepad.exe", "", "", SW_SHOWNORMAL);
    //打开一个网页
    ShellExecute(0, "open", "http://www.baidu.com", "", "", SW_SHOWNORMAL);

    system("pause");
    return 0;
}
#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <string>

void f1(const std::string& str)
{
    int i1 = 0;

    // 判断字符串 str == ""
    // str.size() == 0
    // 
    // 判断字符串 str == "aaa"
    // str.size() > 0 && strcmp(&str[0],"aaa") == 0
    int i2 = 0;



    std::string s;
    s = "s";
    s = "b";

}

int main()
{
    f1("");
    f1("sss");
    f1("aaa");
    f1("");
}

#endif // TEST5

#ifdef TEST6

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    HANDLE hOut;
    WORD Attribute;
    COORD Where;
    unsigned long NumRead;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";

    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED);
    SetConsoleTextAttribute(hOut, 6); // 设置控制台颜色

    cout << "This text is yellow." << endl;

    Where.X = 4;
    Where.Y = 0;
    ReadConsoleOutputAttribute(hOut, &Attribute, 1, Where, &NumRead); // 读取控制台颜色

    cout << "Attribute is " << Attribute << endl;

    SetConsoleTextAttribute(hOut, Attribute);

    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
    std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>


//template<class T>
//struct node_traits
//{
//    typedef list_node_base<T>* base_ptr;
//    typedef list_node<T>* node _ptr;
//};
//
//template<class T>
//struct list_node_base
//{
//    typedef typename node_traits<T>::base_ptr base_ptr;
//    typedef typename node_traits<T>::node_ptr node_ptr;
//
//    base_ptr prev;
//    node_ptr next;
//
//    lsit_node_base() = default;
//
//    node_ptr as_node()
//    {
//        return static_cast<node_ptr>(self());
//    }
//
//    void unlink()
//    {
//        prev = next = self();
//    }
//
//    base_ptr self()
//    {
//        return static_cast<base_ptr>(&*this);
//    }
//};
//
//template<class T>
//struct list_node:public list_node_base<T>
//{
//    typedef typename node_traits<T>::base_ptr base_ptr;
//    typedef typename node_traits<T>::node_ptr node_ptr;
//};

int main()
{
}
#endif // TEST7