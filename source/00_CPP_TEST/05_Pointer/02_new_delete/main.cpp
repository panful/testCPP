/*

 * 4. new 和 delete 作用域
 

 * 17 类中new出来的成员变量内存位置 C++内存模型 00_07_TEST12
 * 18 堆和栈 优点缺点 https://www.zhihu.com/question/379456802/answer/1115546749
 * 19 APR内存池 http://www.wjhsh.net/jiangzhaowei-p-10383065.html
 * 20 NULL nullptr nullptr_t

 * 22 new和delete应该一对一，越界赋值，new崩溃
 * 23 operator new
 */

#define TEST23


#ifdef TEST4

#include <iostream>

class A
{
public:
    A() { std::cout << "construct\n"; }
    ~A() { std::cout << "destruct\n"; }
};

int main()
{
    std::cout << "start\n";
    A* a1 = nullptr;
    std::cout << "111\n";
    {
        a1 = new A();
    }
    std::cout << "222\n";
    delete a1;
    a1 = nullptr;
    std::cout << "end\n";
}

#endif // TEST4


#ifdef TEST17

/*

1.类的成员变量并不能决定自身的存储空间位置。决定存储位置的是对象的创建方式。
即：https://blog.csdn.net/qq_28584889/article/details/117037810

如果对象是函数内的非静态局部变量，则对象，对象的成员变量保存在栈区。
如果对象是全局变量，则对象，对象的成员变量保存在静态区。
如果对象是函数内的静态局部变量，则对象，对象的成员变量保存在静态区。
如果对象是new出来的，则对象，对象的成员变量保存在堆区。
下面是一个示例：当对象是new出来的时，其对象地址和成员变量、成员变量的成员变量、成员变量存储的数据都在堆区存储。

2.通过内存地址判断是在堆上还是栈上：
https://blog.csdn.net/tulingwangbo/article/details/79729548

*/

#include <iostream>
#include <memory>

class Test
{
public:
    Test()
        : m_c1('x')
        , m_c2('y')
        , m_p1(new char[3] { 0 })
        , m_p2(new char[3] { 0 })
    {
    }
    ~Test()
    {
        if (m_p1)
        {
            delete[] m_p1;
            m_p1 = nullptr;
        }

        if (m_p2)
        {
            delete[] m_p2;
            m_p2 = nullptr;
        }
    }

    char m_c1;
    char m_c2;
    char* m_p1;
    char* m_p2;
};

#define PRINT_ADDRESS(x) printf("%p\t%s\n", x, #x);

int main()
{
    std::cout << "=======================================\n";
    {
        int a { 1 };
        int b { 2 };
        char c { 'x' };
        char d { 'y' };

        int* pa = new int(2);
        int* pb = new int(3);
        char* pc = new char('c');
        char* pd = new char('d');

        // 栈
        PRINT_ADDRESS(&a);
        PRINT_ADDRESS(&b);
        PRINT_ADDRESS(&c);
        PRINT_ADDRESS(&d);
        std::cout << "-----------------------\n";
        // 堆
        PRINT_ADDRESS(pa);
        PRINT_ADDRESS(pb);
        PRINT_ADDRESS(pc);
        PRINT_ADDRESS(pd);
    }
    std::cout << "=======================================\n";
    {
        Test t;
        char c1 = 'c';
        char c2 = 'c';
        char* p1 = new char[2] { 0 };
        char* p2 = new char[2] { 0 };

        // 栈
        PRINT_ADDRESS(&t);
        PRINT_ADDRESS(&t.m_c1);
        PRINT_ADDRESS(&t.m_c2);
        PRINT_ADDRESS(&c1);
        PRINT_ADDRESS(&c2);
        std::cout << "-----------------------\n";
        // 堆
        PRINT_ADDRESS(t.m_p1);
        PRINT_ADDRESS(t.m_p2);
        PRINT_ADDRESS(p1);
        PRINT_ADDRESS(p2);
    }
    std::cout << "=======================================\n";
    {
        Test* t = new Test();
        char c1 = 'c';
        char c2 = 'c';
        char* p1 = new char[2] { 0 };
        char* p2 = new char[2] { 0 };

        // 堆
        PRINT_ADDRESS(t);
        PRINT_ADDRESS(&t->m_c1);
        PRINT_ADDRESS(&t->m_c2);
        PRINT_ADDRESS(t->m_p1);
        PRINT_ADDRESS(t->m_p2);
        PRINT_ADDRESS(p1);
        PRINT_ADDRESS(p2);
        std::cout << "-----------------------\n";
        // 栈
        PRINT_ADDRESS(&c1);
        PRINT_ADDRESS(&c2);
    }
    std::cout << "=======================================\n";
    {
        std::unique_ptr<Test> t = std::make_unique<Test>();
        char c1 = 'c';
        char c2 = 'c';
        char* p1 = new char[2] { 0 };
        char* p2 = new char[2] { 0 };

        // 堆
        PRINT_ADDRESS(t.get());
        PRINT_ADDRESS(&t->m_c1);
        PRINT_ADDRESS(&t->m_c2);
        PRINT_ADDRESS(t->m_p1);
        PRINT_ADDRESS(t->m_p2);
        PRINT_ADDRESS(p1);
        PRINT_ADDRESS(p2);
        std::cout << "-----------------------\n";
        // 栈
        PRINT_ADDRESS(&c1);
        PRINT_ADDRESS(&c2);
    }
    std::cout << "=======================================\n";
    {
        std::shared_ptr<Test> t = std::make_shared<Test>();
        char c1 = 'c';
        char c2 = 'c';
        char* p1 = new char[2] { 0 };
        char* p2 = new char[2] { 0 };

        // 堆
        PRINT_ADDRESS(t.get());
        PRINT_ADDRESS(&t->m_c1);
        PRINT_ADDRESS(&t->m_c2);
        PRINT_ADDRESS(t->m_p1);
        PRINT_ADDRESS(t->m_p2);
        PRINT_ADDRESS(p1);
        PRINT_ADDRESS(p2);
        std::cout << "-----------------------\n";
        // 栈
        PRINT_ADDRESS(&c1);
        PRINT_ADDRESS(&c2);
    }

    return 0;
}

#endif // TEST17

#ifdef TEST18

/*
3.C++内存模型
https://blog.csdn.net/weixin_43340455/article/details/124786128

4.C++自由存储区和堆
https://blog.csdn.net/great_emperor_/article/details/123261184
*/

/*
 * 全局/静态存储区
 * 1.BSS段
 * 这个区域存放的是未曾初始化的静态变量、全局变量，但是不会存放常量，因为常量在定义的时候就一定会赋值了。未初始化的全局变量和静态变量，编译器在编译阶段都会将其默认为0
 * 2.DATA段
 * 这个区域主要用于存放编译阶段（非运行阶段时）就能确定的数据，也就是初始化的静态变量、全局变量和常量，这个区域是可读可写的。这也就是我们通常说的静态存储区
 *
 */

#include <iostream>

int g_a;
static int g_b;

int g_c = 7;
static int g_d = 8;

const int g_e = 9;
const static int g_f = 9;

const char* g_g = "abcd";

#define PRINT_ADDRESS(x) printf("address : %p\tvalue : %d\t arg : %s\n", &x, x, #x);
#define PRINT_ADDRESS_S(x) printf("address : %p\tvalue : %s\t arg : %s\n", &x, x, #x);

int main()
{
    // int m_a;
    static int m_b;

    int m_c = 7;
    static int m_d = 8;

    const int m_e = 9;
    const static int m_f = 9;

    const char* m_g = "abcd";
    static const char* m_h = "abcd";
    const static char* m_i = "abcd";

    PRINT_ADDRESS(g_a);
    PRINT_ADDRESS(g_b);
    PRINT_ADDRESS(m_b);

    PRINT_ADDRESS(g_c);
    PRINT_ADDRESS(g_d);
    PRINT_ADDRESS(m_d);

    PRINT_ADDRESS(g_e);
    PRINT_ADDRESS(g_f);
    std::cout << "------------------\n";
    // PRINT_ADDRESS(m_a);

    PRINT_ADDRESS(m_f);
    std::cout << "------------------\n";
    PRINT_ADDRESS(m_c);
    PRINT_ADDRESS(m_e);
    std::cout << "------------------\n";
    PRINT_ADDRESS_S(m_g);
    PRINT_ADDRESS_S(m_h);
    PRINT_ADDRESS_S(m_i);
    PRINT_ADDRESS_S(g_g);

    return 0;
}

#endif // TEST18

#ifdef TEST19

#endif // TEST19

#ifdef TEST20

#include <iostream>

void func(void*)
{
    std::cout << "func1\n";
}
void func(int)
{
    std::cout << "func2\n";
}

/*
 * NULL只是一个宏，有如下定义
 * #define NULL 0
 */

int main()
{
    func(0);                           // 2
    func(NULL);                        // 2
    func(nullptr);                     // 1
    func(static_cast<void*>(nullptr)); // 1

    // nullptr_t 是一个类型（指针空值类型），nullptr是nullptr_t类型的一个实例对象
    nullptr_t s = 0; // 必须初始化
    func(s);         // 1
}
#endif // TEST20



#ifdef TEST22

#include <iostream>
#include <thread>

int main()
{
    // 一个new对应一个delete
    {
        int* p = new int[1000] { 0 };
        long long count = 1000000;
        while (count-- > 0)
        {
            // 【注意】重新将new的值赋给p之前，必须释放原来的p，不然内存泄露
            //  一个new对应一个delete
            delete[] p;
            p = nullptr;
            p = new int[1000] { 0 };
        }

        // std::this_thread::sleep_for(std::chrono::seconds(10));

        if (p)
        {
            delete[] p;
            p = nullptr;
        }
    }

    std::cout << "--------------------\n";

    {
        int* p = new int[100] { 0 };
        for (size_t i = 0; i < 1000; i++)
        {
            auto x = p[i];
            p[i] = (int)i; // 此处越界赋值破坏了堆，下一次new会崩溃
        }

        int* p2 = new int[100] { 0 };

        delete[] p;
        p = nullptr;

        delete[] p2;
        p2 = nullptr;
    }
}

#endif // TEST22

#ifdef TEST23

#include <iostream>

void* operator new(size_t size)
{
    std::cout << "Allocing " << size << " bytes\n";
    return malloc(size);
}
void operator delete(void* memory, size_t size)
{
    std::cout << "Free " << size << " bytes\n";
    free(memory);
}
struct Entity
{
    int x, y, z;
};

int main()
{
    {
        std::string name = "abcd";
    }

    std::cout << "---------------------\n";

    {
        std::string name = "01234567890123456789";
    }

    std::cout << "---------------------\n";

    {
        Entity* e = new Entity();
        delete e;
    }

    return 0;
}

#endif // TEST23