/*
* 1. 求C++可变参数列表长度
* 2. 逗号表达式初始化列表方式访问可变参数列表
* 3. 递归方式 访问可变参数列表  //https://blog.csdn.net/King_weng/article/details/100099399
* 4. 简单宏定义 #define # ## #$
* 5. 宏定义中使用可变长参数 __VA_ARGS__
* 6. va_list va_start va_end va_arg 可变参数
* 7. std::cout打印可变参数 折叠表达式Fold Expression C++17 //https://www.cnblogs.com/GEEK-ZHAO/p/15775026.html
* 8. C++中的变长参数深入理解 左值右值 内存池 https://www.qb5200.com/article/295128.html
*/



#define TEST7

#ifdef TEST1

#include <iostream>

void test(int a, int b)
{
    std::cout << a << ',' << b << '\n';
}

template <typename ...Args>
void func(Args...args)
{
    //test(std::forward<Args>(args)...);
    std::cout << "参数个数:" << sizeof...(args) << '\n';
    test(args...);
}

int main()
{
    func(1, 2);
}
#endif // TEST1

#ifdef TEST2

#include <iostream>
using namespace std;

template <class T>
void print(T arg)
{
    cout << arg << endl;
}

template <class ... Args>
void expand(Args ... args)
{
    // expand函数的逗号表达式：(print(args), 0)， 先执行print(args)，再得到逗号表达式的结果0。
    // 通过初始化列表来初始化一个变长数组，{ (print(args), 0)… }将会展开成((print(args1), 0), (print(args2), 0), (print(args3), 0), etc…), 
    // 最终会创建一个元素值都为0的数组int a[sizeof…(args)]。
    int a[] = { (print(args), 9)... }; //参数类型无需相同，因为print是一个模板函数 执行完之后 a = {9,9,9,9}

    //auto ret = {args...};  //参数列表中的所有元素类型必须相同
}

int main()
{
    expand(1, 2, 'a', 'b');

    system("pause");
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
using namespace std;

//递归终止函数
void debug()
{
    cout << "empty\n";
}

//展开函数
template <class T, class ... Args>
void debug(T first, Args ... last)
{
    cout << "parameter " << first << endl;
    debug(last...);
}

int main()
{
    debug(1, 2, 3, 4);

    system("pause");
    return 0;
}
#endif // TEST3

#ifdef TEST4

/*
# 用来把参数转换成字符串，即用来给参数加双引号
#@ 用来给参数加单引号
## 用来把两个参数连接起来
*/

#include <iostream>

#define ADD(x,y) std::cout << x + y << std::endl;
#define STR(x) std::cout << #x << std::endl;

#ifdef WIN32
# define GLUE_STR(x,y) std::cout << x##y << std::endl;
# define STR2(x) std::cout << #@x << std::endl;
#endif // WIN32

int main()
{
    ADD(3, 4);
    STR(float);

#ifdef WIN32
    GLUE_STR("aa","bb");
    STR2(x);
    STR2(xx);  // 打印'xx'对应的int值
#endif // WIN32

#if defined WIN32
    std::cout << "defined\n";
#endif

    return 0;
}
#endif // TEST4

#ifdef TEST5

#include <iostream>

#define FUN(fun,...) fun(__VA_ARGS__)

void test(int x, int y, int z)
{
    std::cout << x + y + z << '\n';
}

int main()
{
    FUN(test, 1, 2, 3);

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <cstdarg>
#include <iostream>

int sum(int num_args, ...)  //num_args是参数的个数
{
    int val = 0;
    va_list ap;

    va_start(ap, num_args);
    for (int i = 0; i < num_args; i++)
    {
        val += va_arg(ap, int);
    }
    va_end(ap);

    return val;
}

int main(void)
{
    std::cout << "10 + 20 + 30 = " << sum(3, 10, 20, 30) << std::endl;
    std::cout << "4 + 20 + 25 + 30 = " << sum(4, 4, 20, 25, 30) << std::endl;

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <type_traits>

template<typename... Ts>
void printAll1(Ts&&... mXs)
{
    // std::cout使用可变参数
    (std::cout << ... << mXs) << std::endl;
}

template<typename... Args>
void printAll2(Args&& ...args)
{
    // 每一个参数后面都加一个制表符，最后一个参数打印完进行换行
    ((std::cout << args << '\t'), ...) << '\n';
}

enum class MyEnumClass
{
    Enum1 = 0,
    Enum2 = 1,
    Enum3 = 2
};

template<typename... Args>
void printAll3(MyEnumClass x = MyEnumClass::Enum1, Args&&...args)
{
    //((std::cout << x << args << '\t'), ...) << '\n'; // args的每个参数前都会带一个参数x
    
    //std::cout << "x = " << x << " +++ ";
    ((std::cout << args << '\t'), ...) << '\n';


}

// mFn是一个函数对象，forArgs作用就是将可变参数mXs依次当作函数对象的参数执行
// forArg是一个参数的，forArgs是任意个参数的
template<typename Func,typename T>
void forArg(Func&& f,T t)
{
    f(t);
}
template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mXs)
{
    (mFn(mXs), ...);
}

template<typename ...T>
auto sum(T&& ...t)
{
    return (t + ...);
}

int main()
{
    {
        printAll1(3, 4.0, "5"); // 345
        printAll1(); // 空行

        std::cout << "******\n";
        printAll2(1, 2.2, "abc");

        std::cout << "******\n";
        //printAll3(3, 'a', "bb", 2.2); // error 有默认值的参数必须放在形参的最后面
        printAll3(MyEnumClass::Enum1, 3, 'a', "bb", 2.2);
    }

    std::cout << "--------------------\n";

    {
        forArg([](auto x) {std::cout << x << '\n'; }, 1);
        //forArg([](auto x) {std::cout << x << '\n'; }, 1, 2, 3); // error，参数太多
        //forArg([](auto x) {std::cout << x << '\n'; }); // error 参数太少
    }

    std::cout << "--------------------\n";

    {
        forArgs([](auto a) {std::cout << a << '\n'; }, 3, 4.0, "5"); // 345
        forArgs([](auto a) {std::cout << a << '\n'; }); // 空操作
    }

    std::cout << "--------------------\n";

    {
        std::cout << sum(1, 2, 3) << '\n';
        std::string s1("aa");
        std::string s2("bb");
        std::string s3("cc");
        std::cout << sum(s1, s2, s3) << '\n';
        //std::cout << sum() << '\n'; // error: “ + ”上的一元 fold 表达式必须具有非空扩展
    }

    return 0;
}
#endif // TEST7

#ifdef TEST8

#include <stdlib.h>

#define MEMPOOL_NEW_SIZE 8

template<typename T, size_t count = 1>
class MemPool
{
private:
    union MemObj {
        char _obj[1];
        MemObj* _freelink;
    };

public:
    static void* Allocate()
    {
        if (!_freelist) {
            refill();
        }
        MemObj* alloc_mem = _freelist;
        _freelist = _freelist->_freelink;
        ++_size;
        return (void*)alloc_mem;
    }

    static void DeAllocate(void* p)
    {
        MemObj* q = (MemObj*)p;
        q->_freelink = _freelist;
        _freelist = q;
        --_size;
    }

    static size_t TotalSize() {
        return _totalsize;
    }

    static size_t Size() {
        return _size;
    }
private:
    static void refill()
    {
        size_t size = sizeof(T) * count;
        char* new_mem = (char*)malloc(size * MEMPOOL_NEW_SIZE);
        for (int i = 0; i < MEMPOOL_NEW_SIZE; ++i) {
            MemObj* free_mem = (MemObj*)(new_mem + i * size);
            free_mem->_freelink = _freelist;
            _freelist = free_mem;
        }
        _totalsize += MEMPOOL_NEW_SIZE;
    }

    static MemObj* _freelist;
    static size_t _totalsize;
    static size_t _size;
};

template<typename T, size_t count>
typename MemPool<T, count>::MemObj* MemPool<T, count>::_freelist = NULL;

template<typename T, size_t count>
size_t MemPool<T, count>::_totalsize = 0;

template<typename T, size_t count>
size_t MemPool<T, count>::_size = 0;

template<class T>
T* MemNew(size_t count)
{
    T* p = (T*)MemPool<T, count>::Allocate();
    if (p != NULL)
    {
        if (!std::is_pod<T>::value)
        {
            for (size_t i = 0; i < count; ++i)
            {
                new (&p[i]) T();
            }
        }
    }
    return p;
}

template<class T>
T* MemDelete(T* p, size_t count)
{
    if (p != NULL)
    {
        if (!std::is_pod<T>::value)
        {
            for (size_t i = 0; i < count; ++i)
            {
                p[i].~T();
            }
        }
        MemPool<T, count>::DeAllocate(p);
    }
}

template<class T, class... Args>
T* MemNew(size_t count, Args&&... args)
{
    T* p = (T*)MemPool<T, count>::Allocate();
    if (p != NULL)
    {
        if (!std::is_pod<T>::value)
        {
            for (size_t i = 0; i < count; ++i)
            {
                new (&p[i]) T(std::forward<Args>(args)...);
            }
        }
    }
    return p;
}

#endif // TEST8










