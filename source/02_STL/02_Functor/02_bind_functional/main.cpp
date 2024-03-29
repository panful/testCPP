/*
1. 标准库hash函数
2. std::function 指向成员函数
3. std::mem_fn 从成员指针创建出函数对象,有点类似function，bind
4. 回调函数
5. std::bind会调用参数的拷贝构造，即总是复制其实参
6. 将std::bind的返回值绑定到std::bind的实参需要强制类型转换

9.

13.std::invoke std::invoke_r https://zh.cppreference.com/w/cpp/utility/functional/invoke
14.为什么要使用std::invoke
15.使用标准库std::hash对自定义类型求hash值
16.
*/
#define TEST6

#ifdef TEST1
// hash example
#include <functional>
#include <iostream>
#include <map>
#include <string>

int main()
{
    int a               = 0;
    int b               = 1;
    int c               = 2;
    std::string type    = "fluid";
    std::string mesh    = "Flow";
    std::string physics = "press";

    std::hash<std::string> strHash;
    auto ret1 = strHash(std::to_string(a) + type + mesh + physics);
    auto ret2 = strHash(std::to_string(b) + type + mesh + physics);
    auto ret3 = strHash(std::to_string(c) + type + mesh + physics);
    auto ret4 = strHash(std::to_string(c) + mesh + type + physics);
    auto ret5 = strHash(std::to_string(a) + mesh + type + physics);
    auto ret6 = strHash(std::to_string(c) + type + mesh + physics);

    std::cout << (ret1 == ret2) << std::endl;
    std::cout << (ret1 == ret3) << std::endl;
    std::cout << (ret3 == ret4) << std::endl;
    std::cout << (ret4 == ret5) << std::endl;
    std::cout << (ret3 == ret6) << std::endl;

    char nts1[] = "Test";
    char nts2[] = "Test";
    std::string str1(nts1);
    std::string str2(nts2);

    std::hash<char*> ptr_hash;
    std::hash<std::string> str_hash;

    std::cout << "same hashes:\n" << std::boolalpha;
    std::cout << "nts1 and nts2: " << (ptr_hash(nts1) == ptr_hash(nts2)) << '\n'; // 指针是一个地址
    std::cout << "str1 and str2: " << (str_hash(str1) == str_hash(str2)) << '\n'; // string是一个字符串
    std::cout << "str1 and nts2: " << (str_hash(str1) == ptr_hash(nts2)) << '\n';

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <functional>

class Test
{
public:
    int fun(int x, int y)
    {
        return x + y;
    }
};

int main()
{
    Test test;
    // 方式1：fun1的类型可以为auto
    std::function<int(int, int)> fun1 = std::bind(&Test::fun, test, std::placeholders::_1, std::placeholders::_2);
    int result1                       = fun1(1, 2);

    // 方式2：fun2的类型必须明确指定，不能为auto
    std::function<int(Test, int, int)> fun2 = &Test::fun;
    int result2                             = fun2(test, 1, 2);

    return 0;
}

#endif // TEST2

#ifdef TEST3

// std::mem_fn比 std::bind功能更简单，std::bind功能更复杂，如果使用mem_fn能解决问题就不要用std::bind

#include <functional>
#include <iostream>

class Foo
{
public:
    void no_arg()
    {
        std::cout << "Hello, world.\n";
    }

    void has_arg(int i)
    {
        std::cout << "number: " << i << '\n';
    }

    int data = 7;
};

int main()
{
    Foo f;
    // 参数必须为类成员变量或函数
    auto fn1 = std::mem_fn(&Foo::no_arg);
    fn1(&f);
    auto b1 = std::bind(&Foo::no_arg, f);
    b1();

    auto fn2 = std::mem_fn(&Foo::has_arg);
    fn2(&f, 42);
    auto b2 = std::bind(&Foo::has_arg, f, 3);
    b2();

    auto fn3 = std::mem_fn(&Foo::data);
    std::cout << "data: " << fn3(&f) << '\n';

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <functional>
#include <iostream>

using FuncType = std::function<void(int)>;

void callBack(int i)
{
    std::cout << "callback  " << i << '\n';
}

class A
{
public:
    void callback(int i)
    {
        std::cout << "class A callback   " << i << "\n";
    }
};

void test(FuncType f)
{
    f(2);
    std::cout << "test\n";
}

int main()
{
    test(callBack);               // 2

    test(std::bind(callBack, 4)); // 4

    A a;
    test(std::bind(&A::callback, a, 7)); // 7

    A aa;
    test(std::bind(&A::callback, aa, std::placeholders::_1)); // 2

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <functional>
#include <iostream>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    Helper(const Helper&)
    {
        std::cout << "copy construct\n";
    }

    Helper(Helper&&)
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&)
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

void f3(const Helper& h)
{
    std::cout << &h << '\n';
}

int main()
{
    {
        Helper h;
        std::cout << &h << '\n';
        // 此处的std::bind调用了一次Helper的拷贝构造
        auto f = std::bind(f3, h);
        // 实际上调用f3的时候，实参并不是h，而是使用h拷贝构造的一个新的对象
        // 如果要传入引用，需要使用std::ref
        f();
    }

    std::cout << "------------------------------------\n";

    {
        Helper h;
        std::cout << &h << '\n';
        auto f = std::bind(f3, std::placeholders::_1);
        // 传给f3的实参就是h
        f(h);
    }
}

#endif // TEST5

#ifdef TEST6

#include <functional>
#include <iostream>

void f1(int)
{
    std::cout << "f1(int)\n";
}

void f2(std::function<void(int)>)
{
    std::cout << "f2(std::function<void(int)>)\n";
}

int main()
{

    {
        // error
        // auto f = std::bind(f2, std::bind(f1, 2));
        // f();

        // error
        // auto f = std::bind(f2, std::bind(f1, std::placeholders::_1));
        // f(2);
    }

    // std::bind不能直接绑定到std::function类型的实参
    {
        auto f = std::bind(f2, static_cast<std::function<void(int)>>(std::bind(f1, 2)));
        f();
    }
    {
        auto f = std::bind(f2, static_cast<std::function<void(int)>>(std::bind(f1, std::placeholders::_1)));
        f(2);
    }

    // 使用lambda传递std::function类型的参数
    {
        auto f = std::bind(f2, [](int) {});
        f();
    }
}

#endif // TEST6

#ifdef TEST13

#include <functional>
#include <iostream>
#include <type_traits>

struct Foo
{
    Foo(int num) : num_(num)
    {
    }

    void print_add(int i) const
    {
        std::cout << num_ + i << '\n';
    }

    int num_;
};

void print_num(int i)
{
    std::cout << i << '\n';
}

struct PrintNum
{
    void operator()(int i) const
    {
        std::cout << i << '\n';
    }
};

int main()
{
    // 调用自由函数
    std::invoke(print_num, -9);

    // 调用 lambda
    std::invoke([]() { print_num(42); });

    // 调用成员函数
    const Foo foo(314159);
    std::invoke(&Foo::print_add, foo, 1);

    // 调用（访问）数据成员
    std::cout << "num_: " << std::invoke(&Foo::num_, foo) << '\n';

    // 调用函数对象
    std::invoke(PrintNum(), 18);

    // c++23
#if defined(__cpp_lib_invoke_r)
    auto add = [](int x, int y) { return x + y; };
    auto ret = std::invoke_r<float>(add, 11, 22);
    static_assert(std::is_same<decltype(ret), float>());
    std::cout << ret << '\n';
    std::invoke_r<void>(print_num, 44);
#endif
}

#endif // TEST13

#ifdef TEST14
#include <iostream>

#define WARP_CALL(fun, ...) fun(__VA_ARGS__)

template <typename Fun, typename... Args>
auto warp_call1(Fun f, Args... args) -> decltype(f(args...))
{
    return f(args...); // 注意此处args后面的...不要忘记
}

template <typename Fun, typename... Args>
auto warp_call2(Fun&& f, Args&&... args)
{
    // 只是给f和args多了一步完美转发，注意...的位置
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template <typename Fun, typename... Args>
decltype(auto) warp_call3(Fun&& f, Args&&... args) noexcept
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template <typename Fun, typename... Args>
constexpr auto warp_call4(Fun&& f, Args&&... args) noexcept -> decltype(std::forward<Fun>(f)(std::forward<Args>(args)...))
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

template <typename Fun, typename... Args>
std::invoke_result_t<Fun, Args...> warp_call5(Fun&& f, Args... args) noexcept
{
    return std::forward<Fun>(f)(std::forward<Args>(args)...);
}

int fun(int x, int y)
{
    return x + y;
}

int main()
{
    auto ret1 = WARP_CALL(fun, 2, 2);
    std::cout << "x + y = " << ret1 << std::endl;

    auto ret2 = warp_call1(fun, 2, 4);
    std::cout << "x + y = " << ret2 << std::endl;

    auto ret3 = warp_call2(fun, 1, 4);
    std::cout << "x + y = " << ret3 << std::endl;

    auto ret4 = warp_call3(fun, 4, 4);
    std::cout << "x + y = " << ret4 << std::endl;

    // std::invoke就相当于warp_call4
    auto ret5 = warp_call4(fun, 3, 4);
    std::cout << "x + y = " << ret5 << std::endl;

    auto ret6 = warp_call5(fun, 3, 2);
    std::cout << "x + y = " << ret6 << std::endl;

    return 0;
}
#endif // TEST14

#ifdef TEST15

#include <functional>

class A
{
    friend struct std::hash<A>;
};

namespace std {
template <>
struct hash<A>
{
    size_t operator()(A const& a) const noexcept
    {
        return 1; /*返回哈希值*/
    }
};
} // namespace std

int main()
{
    A a;
    std::hash<A> s;
    auto hashCode = s(a);
}
#endif // TEST15

#ifdef TEST16

#include <iostream>
#include <mutex>

using namespace std;

namespace detail {
template <typename MemPointer>
struct MemPointerTraits;

template <typename Obj, typename V>
struct MemPointerTraits<V Obj::*>
{
    using Object = Obj;
    using Value  = V;
};

template <typename T>
struct Tag
{
};

template <auto mfptr>
struct MemberFunctionToStaticGuard
{
    using MemFunc = decltype(mfptr);
    using Obj     = typename MemPointerTraits<MemFunc>::Object;
    using Func    = typename MemPointerTraits<MemFunc>::Value;
    inline static Obj* sm_ptr { nullptr };
    inline static mutex sm_m;

    MemberFunctionToStaticGuard(Obj* ptr)
    {
        sm_ptr = ptr;
        sm_m.lock();
    }

    MemberFunctionToStaticGuard(const Obj* ptr) : MemberFunctionToStaticGuard { const_cast<Obj*>(ptr) }
    {
    }

    ~MemberFunctionToStaticGuard()
    {
        sm_ptr = nullptr;
        sm_m.unlock();
    }

    static constexpr auto GetStaticFuncPointer()
    {
        return GetStaticFuncPointer(Tag<Func> {});
    }

    template <typename Ret, typename... Args>
    static constexpr auto GetStaticFuncPointer(Tag<Ret(Args...)>)
    {
        return static_cast<Ret (*)(Args...)>([](Args... args) { return (sm_ptr->*mfptr)(std::forward<Args>(args)...); });
    }

    template <typename Ret, typename... Args>
    static constexpr auto GetStaticFuncPointer(Tag<Ret(Args...) const>)
    {
        return GetStaticFuncPointer(Tag<Ret(Args...)> {});
    }
};
} // namespace detail

typedef int (*pFunc)(int);

void test(pFunc f)
{
    std::cout << "test\n";
    f(1);
}

class A
{
public:
    int foo(int x)
    {
        std::cout << "foo\t" << x << '\n';
        return 0;
    }

    void bar()
    {
        std::cout << "bar\n";
        detail::MemberFunctionToStaticGuard<&A::foo> guard { this };
        test(guard.GetStaticFuncPointer());
    }

private:
    int x { 666 };
};

// https://www.zhihu.com/question/411373289
// std::invoke是否可以实现？

int main()
{
    A a;
    a.bar();
}

#endif // TEST16