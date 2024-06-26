/*
 * 1. std::optional 可选返回值 pointer
 * 11 std::optional 拷贝时，存储的值是深拷贝
 * 
 * 2. std::variant 值多态 union
 * 3. std::visit 可以用来访问std::variant
 * 4. std::any 万能容器 void*
 * 5. 自定义any
 * 6. std::any存储仿函数以及模板类 std::functional
 * 7. 函数顺序执行队列  命令模式
 * 8. union 内存共享 new(p) pp;
 * 9. memcpy pod union
 * 10 std::aligned_union
 * 11 位域
 * 12 std::visit使用泛型lambda对std::variant不同类型的值进行不同操作
 */

#define TEST1

#ifdef TEST1

// std::optional 可以用在函数返回结果不一定正确的时候
// 例如输入文件名读取文件并返回文件内容，有可能打开文件失败，有可能文件本身就为空
// 所以返回空字符串就有歧义，如果文件打开失败返回std::nullopt就比较好理解

#include <iostream>
#include <optional>
#include <string>

// 将std::string转换为 int 型数据
std::optional<int> asInt(const std::string& s)
{
    try
    {
        return std::stoi(s); // 转换成功返回结果
    }
    catch (...)
    {
        return std::nullopt; // 转换失败返回 std::nullopt 可以理解为nullptr
    }
}

int main()
{

    // 使用 解引用* 获取 std::optional 的值
    for (const auto& s : { "42", " 077", "hello", "0x33" })
    {
        std::optional<int> oi = asInt(s);

        // std::optional<T>对bool()进行了重载，所以直接可以用在if语句中
        if (oi)
        {
            // 如果 std::optional 没值，则 *oi 是未定义的行为
            std::cout << "convert \t'" << s << "' \t\tto int: " << *oi << "\n";
        }
        else
        {
            std::cout << "can't convert \t'" << s << "' \tto int\n";
        }
    }

    std::cout << "--------------------\n";

    // 使用 has_value() 判断是否有值，使用 value() 获取 std::optional 的值
    {
        std::optional<int> optInt1 = 2.2; // 会强制将 double 转换为 int
        std::cout << (optInt1.has_value() ? optInt1.value() : 0) << std::endl;

        std::optional<int> optInt2;
        try
        {
            std::cout << optInt2.value() << '\n'; // 此处会抛出异常
        }
        catch (std::bad_optional_access)
        {
            std::cout << "value() throw error\n";
        }
    }

    std::cout << "--------------------\n";

    // 使用 value_or() 获取 std::optional 的值
    {
        // 如果 std::optional 有值则返回 std::optional 的值，如果没值则返回 value_or() 的参数
        std::optional<int> optInt1 = 2;
        std::cout << optInt1.value_or(1) << '\n'; // 2

        std::optional<int> optInt2;
        std::cout << optInt2.value_or(3) << '\n'; // 3
    }

    // 比较 std::optional
    {
        auto x = std::make_optional(1);
        auto y = std::make_optional(2);
        auto z = std::optional<int> { 1 };
        auto s = std::nullopt;

        if (x == y)
        {
            std::cout << "x == y\n";
        }
        if (x == z)
        {
            std::cout << "x == z\n";
        }
        if (y == z)
        {
            std::cout << "y == z\n";
        }
        if (x == s)
        {
            std::cout << "x == s\n";
        }
        if (x == 1)
        {
            std::cout << "x == 1\n";
        }
    }
}

#endif // TEST1

#ifdef TEST11

#include <iostream>
#include <optional>
#include <vector>

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

    Helper(Helper&&) noexcept
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&) noexcept
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

std::optional<Helper> get(bool x)
{
    if (x)
    {
        return std::nullopt;
    }

    Helper h {}; // construct
    return h;    // move construct
}

void f(Helper&&)
{
}

int main()
{
    std::cout << "0-------------\n";
    {
        auto opt = get(false);  // move construct
        auto h   = opt.value(); // copy construct，auto 是左值
    }
    std::cout << "1-------------\n";
    {
        auto opt = get(false);

        std::cout << "has: " << opt.has_value() << '\n';
        auto&& h = opt.value(); // 引用，既不调用移动也不调用拷贝

        f(std::move(h));

        std::cout << opt.has_value() << '\n'; // 即使move，optional还是有值
        opt.reset();
        std::cout << opt.has_value() << '\n';
    }
    std::cout << "2-------------\n";
}

#endif // TEST11

#ifdef TEST2

// std::variant 和 std::any的区别就是：any可以存任意类型的值，而variant只能存储指定类型的值（模板列表中的类型）
// std::variant类似于union TEST8
// std::tuple类似于struct

#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

// 函数返回不同类型的值
std::variant<int, std::string> func(void* p)
{
    if (p)
    {
        return 1;
    }
    else
    {
        return "string";
    }
}

int main()
{
    {
        auto ret1 = func(nullptr);
        auto ret2 = func(new int(1));

        std::cout << std::get<1>(ret1) << '\n';
        std::cout << std::get<0>(ret2) << '\n';
    }
    std::cout << "-------------------------------------------------\n";
    {
        std::variant<int, int, int, float, float, std::string> var1;
        // var1 = 2.0f;  //错误，variant模板初始化有多个float，不知道是给第几个float赋值
        std::variant<int, int, float, float, char, char> var11 { std::in_place_index<3>, 2 }; // 初始化为第三个类型
        var1.emplace<1>(2.0f);      // 正确，指定具体第几个类型，会强制类型转换
        auto index1 = var1.index(); // 1
        var1.emplace<5>("test");    // 正确
        auto index5 = var1.index(); // 5
        // var1.emplace<2>("test");  //错误
        // var1.emplace<int>(2);     //错误

        std::cout << index1 << '\t' << index5 << '\n';
    }

    {
        std::variant<int, float, char> var2;
        var2.emplace<int>(2);       // 没有多个int类型时才可以这样赋值
        var2        = 2.0f;
        var2        = 3.0f;         // 覆盖前面的值
        var2        = 'x';          // 覆盖前面的值
        auto index2 = var2.index(); // 此时var2的值为'x'，是var2的第2个类型（从0开始）
        // auto ret1 = std::get<1>(var2);   //抛出异常，因为var2的值为'x'是第2个类型
        // auto ret2 = std::get<int>(var2); //抛出异常，var2值的类型为char
        auto ret1 = std::get<2>(var2);    // ret1 = 'x'
        auto ret2 = std::get<char>(var2); // ret2 = 'x'

        // 注意get_if的参数必须为指针
        auto ret3 = std::get_if<int>(&var2);  // 获取失败返回nullptr
        auto ret4 = std::get_if<char>(&var2); // 获取成功返回char*
        auto ret5 = std::get_if<1>(&var2);    // 获取失败返回nullptr
        auto ret6 = std::get_if<2>(&var2);    // 获取成功返回char*
    }

    {
        std::vector<std::variant<int, float, char>> vec;
        vec.emplace_back(2);
        vec.emplace_back(2.0f);
        vec.emplace_back('x');
        // vec.emplace_back("xxx");  //错误，因为variant中没有"xxx"的类型
    }

    {
        struct MyStruct
        {
            double a;
            double b;
            double c;
            double d;
            double e;
        };

        std::cout << "double:" << sizeof(double) << "\tfloat:" << sizeof(float) << "\tstd::string:" << sizeof(std::string)
                  << "\tMyStruct:" << sizeof(MyStruct) << '\n';

        std::variant<int, float, double, char> var1;
        std::variant<int, float, char> var2;
        std::variant<int, float, char, std::string> var3;
        std::variant<int, int, int, int> var4;
        std::variant<int, char, int, int> var5;
        std::variant<char> var6;
        std::variant<int> var7;
        std::variant<MyStruct> var8;
        std::variant<MyStruct, int> var9;
        std::variant<MyStruct, int, MyStruct, double> var10;

        auto f = [](auto&& v)
        {
            // variant中最大字节数的类型大于8则sizeof(variant)等于这个字节数+8
            // 小于8则翻倍，好像是这个规律
            std::cout << sizeof(v) << '\t' << typeid(v).name() << '\n';
        };
        f(var1);
        f(var2);
        f(var3);
        f(var4);
        f(var5);
        f(var6);
        f(var7);
        f(var8);
        f(var9);
        f(var10);
        std::cout << "------------------------------------------\n";

        union MyUnion1 {
            int a;
            float b;
            double c;
            char d;
        };

        union MyUnion2 {
            int a;
            float b;
            char c;
            std::string d;
        };

        struct MyStruct1
        { // 4+4+8+8 = 24
            int a;
            float b;
            double c;
            char d;
        };

        struct MyStruct2
        { // 4+4+4+40 + 4 = 56 因为std::string存储了一个指针（8位）因此sizeof(MyStruct2)是8（结构体中最大字节数的成员）的倍数
            int a;
            float b;
            char c;
            std::string d;
        };

        std::tuple<int, float, double, char> tuple1;
        std::tuple<int, float, char, std::string> tuple2;

        std::cout << "union:\t" << sizeof(MyUnion1) << '\t' << sizeof(MyUnion2) << '\n';
        std::cout << "struct:\t" << sizeof(MyStruct1) << '\t' << sizeof(MyStruct2) << '\n';
        std::cout << "tuple:\t" << sizeof(tuple1) << '\t' << sizeof(tuple2) << '\n';
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <string>
#include <variant>

// C++17之std::visit https://blog.csdn.net/janeqi1987/article/details/100568146
// 如何优雅的使用 std::variant 与 std::optional https://zhuanlan.zhihu.com/p/366537214

// 1.使用重载的Lambdas来访问
// https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
namespace T1 {
// 00_13_TEST16 类模板构造函数可变参数
// 00_07_02_TEST16 变长using声明
template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

// 由传递的参数推导基准类型
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

auto twice = overload {
    [](std::string& s)
    {
        s += s;
        std::cout << "twice\t" << s << '\n';
    },
    [](auto& i)
    {
        i *= 2;
        std::cout << "twice\t" << i << '\n';
    },
};

void f1()
{
    std::variant<int, std::string> var1(42), var3("abcde");

    std::visit(twice, var1);
    std::visit(twice, var3);

    std::visit(
        overload {
            // calls best matching lambda for current alternative
            [](int i) { std::cout << "int: " << i << '\n'; },
            [](const std::string& s) { std::cout << "string: " << s << '\n'; },
        },
        var1);

    std::visit(
        overload {
            // calls best matching lambda for current alternative
            [](int i) { std::cout << "int: " << i << '\n'; },
            [](const std::string& s) { std::cout << "string: " << s << '\n'; },
        },
        var3);

    // std::visit同时传入多个std::variant
    std::variant<int, float, std::string> varA, varB;
    varA = 3;
    varB = "string\n";
    std::visit(
        overload {
            [](auto a, auto b) { std::cout << "visit:\t" << a << '\t' << b; },
        },
        varA, varB);

    // std::visit返回值
    std::variant<int> abc, def;
    abc          = 3;
    def          = 4;
    bool testval = std::visit(
        overload {
            [](int a, int b) { return a < b; },
        },
        abc, def);
}
} // namespace T1

// 2.使用函数对象访问lambda
namespace T2 {
struct MyVisitor
{
    // 此处应该为const函数
    void operator()(double d) const
    {
        std::cout << d << '\n';
    }

    void operator()(int i) const
    {
        std::cout << i << '\n';
    }

    void operator()(const std::string& s) const
    {
        std::cout << s << '\n';
    }
};

void f2()
{
    std::variant<int, double, std::string> var1(42), var2(3.14), var3("visit");

    std::visit(MyVisitor(), var1); // calls operator() for matching int type

    std::visit(MyVisitor(), var2); // calls operator() for matching double type

    std::visit(MyVisitor(), var3); // calls operator() for matching std::string type
}
} // namespace T2

// 3.使用泛型lambda访问std::variant
namespace T3 {
void f3()
{
    //+++++++++++++++++++++
    std::variant<int, double, std::string> var1(42), var2(3.14), var3("visit");

    auto myLambda1 = [](const auto& val) { std::cout << val << std::endl; };

    std::visit(myLambda1, var1);
    std::visit(myLambda1, var2);
    std::visit(myLambda1, var3);
    //++++++++++++++++++++++

    std::visit(
        [](auto&& x)
        {
            x += x;
            std::cout << x << '\n';
        },
        var1);

    //++++++++++++++++++++++
    auto myLambda2 = [](auto&& x)
    {
        if constexpr (std::is_convertible_v<decltype(x), std::string>)
        {
            std::cout << "string is:" + x << '\n';
        }
        else
        {
            std::cout << x << "\tnot string\n";
        }
    };
    std::visit(myLambda2, var1);
    std::visit(myLambda2, var2);
    std::visit(myLambda2, var3);
    //+++++++++++++++++++++++
}
} // namespace T3

int main()
{
    T1::f1();
    std::cout << "---------------------------------------\n";
    T2::f2();
    std::cout << "---------------------------------------\n";
    T3::f3();

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
// #include <initializer_list>
#include <any>
#include <vector>

int main()
{
    std::string a = "aa";
    std::vector<std::any> vec { 111, a, 1.2f, "bbb" };
    const char* b = "aa";

    for (auto elem : vec)
    {
        int x = 0;
        // std::cout <<"vector type:" << elem.type().name() << std::endl;

        if (typeid(elem).name() == typeid(a).name())
        {
            std::cout << "string type:";
            std::cout << std::any_cast<std::string>(elem) << std::endl; // 进不来
        }
        if (elem.type().name() == typeid(a).name())
        {
            std::cout << "string type:";
            auto ret = std::any_cast<std::string>(elem); // 类型错误会抛出异常
            std::cout << ret << std::endl;
        }
        if (elem.type().name() == typeid(x).name())
        {
            std::cout << "int type:";
            x = std::any_cast<int>(elem); // 类型错误会抛出异常
            std::cout << x << std::endl;
        }
        if (elem.type().name() == typeid(b).name())
        {
            std::cout << "const char* type:";
            auto ret = std::any_cast<const char*>(elem); // 类型错误会抛出异常
            std::cout << ret << std::endl;
        }
    }
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <memory>

class MyAny
{
public:
    template <typename T> // 这里用模板函数不用模板类，因为如果是模板类，实例化一个MyAny对象就需要指定类型，这样做any就没有了意义，
    MyAny(T t) : base(std::make_unique<Data<T>>(t))
    {
    }

    template <typename T>
    T any_cast()
    {
        auto p = dynamic_cast<Data<T>*>(base.get());
        if (!p)
            throw "type is error\n";
        return p->value;
    }

private:
    class Base
    {
    public:
        virtual ~Base()
        {
        } // 基类是一个多态类，必须定义析构函数
    };

    template <typename T>
    class Data : public Base
    {
    public:
        Data(T t) : value(t)
        {
        }

        T value;
    };

    // Data data;  //派生的子类Data需要模板参数才能创建，所以这里必须是没有模板的基类
    std::unique_ptr<Base> base; // 如果是模板函数，不知道存储的数据类型，就需要在MyAny类内部再定义一个模板基类Base
    // Base base;  //不能用普通对象，因为无法把类型从Base转换到Data，没法访问value
};

int main()
{
    MyAny d(0);
    MyAny e = 'c';
    MyAny f = "test";

    auto ret0 = d.any_cast<int>(); // 这里的类型一定要一致，否则this指针为空
    auto ret1 = e.any_cast<char>();
    auto ret2 = f.any_cast<const char*>();
    std::cout << ret0 << '\t' << ret1 << '\t' << ret2 << '\n';

    try
    {
        auto ret3 = f.any_cast<char*>(); // throw error
    }
    catch (...)
    {
        std::cout << "An exception is thrown here\n";
    }

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <any>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
class A
{
public:
    T t;
};

int test1(int n)
{
    std::cout << "int(int): " << n << std::endl;
    return n;
}

void test2(std::string str)
{
    std::cout << "void(string): " << str << std::endl;
}

int main()
{
    // 1
    {
        std::function<int(int)> f1          = std::bind(&test1, 3);
        std::function<void(std::string)> f2 = std::bind(&test2, "sssssssss");
        std::vector<std::any> myFuns1;
        myFuns1.emplace_back(f1);
        myFuns1.emplace_back(f2);

        auto fun1 = std::any_cast<std::function<int(int)>>(myFuns1[0]);
        auto fun2 = std::any_cast<std::function<void(std::string)>>(myFuns1[1]);

        // 使用的参数还是std::bind中的实参
        fun1(2);
        fun2("www");
    }
    std::cout << "====================================\n";

    // 2 必须用std::function接收std::bind返回值，才能用any_cast，如果用auto接收，any_cast会抛出异常
    {
        auto f3 = std::bind(&test1, 999);
        auto f4 = std::bind(&test2, "test");

        std::vector<std::any> myFuns2;
        myFuns2.emplace_back(f3);
        myFuns2.emplace_back(f4);

        // 内部抛出异常
        try
        {
            auto fun3 = std::any_cast<std::function<int(int)>>(myFuns2[0]);
            auto fun4 = std::any_cast<std::function<void(std::string)>>(myFuns2[1]);
        }
        catch (...)
        {
            std::cout << "std::any_cast throw an error\n";
        }
    }
    std::cout << "====================================\n";

    // 3 使用std::placeholders替换实参
    {
        std::function<int(int)> f5          = std::bind(&test1, std::placeholders::_1);
        std::function<void(std::string)> f6 = std::bind(&test2, std::placeholders::_1);
        std::vector<std::any> myFuns3;
        myFuns3.emplace_back(f5);
        myFuns3.emplace_back(f6);

        auto fun5 = std::any_cast<std::function<int(int)>>(myFuns3[0]);
        auto fun6 = std::any_cast<std::function<void(std::string)>>(myFuns3[1]);

        // 实参将会使用此处的参数
        fun5(123);
        fun6("string");
    }
    std::cout << "====================================\n";

    // 4 any存储模板类
    {
        std::vector<std::any> myFuns4;
        A<int> a;
        a.t = 5;
        myFuns4.emplace_back(a);
        auto ret = std::any_cast<A<int>>(myFuns4[0]);
        std::cout << "value:" << ret.t << std::endl;
    }

    return 0;
}

#endif // TEST6

#ifdef TEST7

// https://blog.csdn.net/weixin_42837024/article/details/121252818

#include <functional>
#include <iostream>
#include <queue>

class FuncExecQueue
{
public:
    typedef std::function<void(void* param)> function_;

    struct FuncStruct
    {
        function_ func = nullptr;
        void* param    = nullptr;

        FuncStruct()
        {
        }

        FuncStruct(const function_& f, void* p) : func(f), param(p)
        {
        }
    };

public:
    FuncExecQueue()
    {
    }

    ~FuncExecQueue()
    {
    }

    void push(const FuncStruct& funcStruct)
    {
        m_funStructQueue.push(funcStruct);
    }

    void run()
    {
        while (false == m_funStructQueue.empty())
        {
            auto funcStruct = m_funStructQueue.front();
            m_funStructQueue.pop();
            if (funcStruct.func)
            {
                funcStruct.func(funcStruct.param);
            }
        }
    }

private:
    std::queue<FuncStruct> m_funStructQueue;
};

/*------------------------*/

void a(void* p)
{
    std::cout << "i am a()" << std::endl;
}

void b(void* p)
{
    std::cout << "i am b()" << std::endl;
}

struct CustomParam
{
    int a = 0;
    int b = 0;
};

void c(void* p)
{
    CustomParam* param = static_cast<CustomParam*>(p);

    if (param)
    {
        std::cout << "i am c()" << std::endl;
        std::cout << param->a << std::endl;
        std::cout << param->b << std::endl;
        delete param;
        param = nullptr;
    }
}

/*------------------------*/

int main()
{
    FuncExecQueue queue;

    queue.push(FuncExecQueue::FuncStruct(&a, nullptr));
    queue.push(FuncExecQueue::FuncStruct(&b, nullptr));

    CustomParam* p = new CustomParam();
    p->a           = 100;
    p->b           = 500;

    queue.push(FuncExecQueue::FuncStruct(&c, static_cast<void*>(p)));

    queue.run();

    return 0;
}

#endif // TEST7

#ifdef TEST8

// 联合体的一些使用场景 https://zhuanlan.zhihu.com/p/495696559

#include <iostream>
#include <string>

// 利用union共享内存的机制组合拆分数据
namespace {
union bit32_data {
    uint32_t data { 0 };

    struct
    {
        uint8_t byte0;
        uint8_t byte1;
        uint8_t byte2;
        uint8_t byte3;
    } byte;
};

// 让变量有多个名字
template <typename T>
union Vec3 {
    struct
    {
        T x, y, z;
    };

    struct
    {
        T r, g, b;
    };

    struct
    {
        T s, t, p;
    };

    Vec3(const T& v1, const T& v2, const T& v3) : x(v1), y(v2), z(v3)
    //, r(v1)
    //, g(v2)
    //, b(v3)
    //, s(v1)
    //, t(v2)
    //, p(v3)
    {
        std::cout << "type: " << typeid(T).name() << '\n';
    }
};

template <typename T>
struct Vec3Data
{
    T x, y, z;
};

template <typename T>
struct Vec3Struct
{
    Vec3Struct(const T& v1, const T& v2, const T& v3) : data(v1, v2, v3)
    {
    }

    T& x = data.x;
    T& y = data.y;
    T& z = data.z;
    T& r = data.x;
    T& g = data.y;
    T& b = data.z;
    T& s = data.x;
    T& t = data.y;
    T& p = data.z;

private:
    Vec3Data<T> data;
};
} // namespace

int main()
{
    // 组合数据
    {
        bit32_data num;
        std::cout << std::hex << num.data << '\n';

        num.byte.byte0 = 0x78;
        num.byte.byte1 = 0x56;
        num.byte.byte2 = 0x34;
        num.byte.byte3 = 0x12;

        std::cout << std::hex << num.data << '\n';
    }

    std::cout << "---------------------------------\n";

    // 拆分数据
    {
        bit32_data num;

        std::cout << std::hex << static_cast<int>(num.byte.byte3) << static_cast<int>(num.byte.byte2) << static_cast<int>(num.byte.byte1)
                  << static_cast<int>(num.byte.byte0) << '\n';

        num.data = 0x12345678;

        std::cout << std::hex << static_cast<int>(num.byte.byte3) << static_cast<int>(num.byte.byte2) << static_cast<int>(num.byte.byte1)
                  << static_cast<int>(num.byte.byte0) << '\n';
    }

    std::cout << "---------------------------------\n";

    // 判断大小端
    {
        bit32_data num;

        num.data = 0x12345678;

        if (0x78 == num.byte.byte0)
        {
            std::cout << "Little endian\n";
        }
        else if (0x78 == num.byte.byte3)
        {
            std::cout << "Big endian\n";
        }
        else
        {
            std::cout << "error\n";
        }
    }

    std::cout << "---------------------------------\n";

    // union同一个数据有多个变量名
    {
        Vec3 v1 { 1, 2, 3 };
        Vec3 v2 { 1.f, 2.f, 3.f };

        std::cout << std::dec << "sizeof: " << sizeof(Vec3<int>) << '\t' << sizeof(v1) << '\n';

        // 编译失败，因为union中不能有非pod类型
        // std::string str = "test";
        // Vec3 v3 { str, str, str };

        std::cout << v1.x << '\t' << v1.y << '\t' << v1.z << '\n'
                  << v1.r << '\t' << v1.g << '\t' << v1.b << '\n'
                  << v1.s << '\t' << v1.t << '\t' << v1.p << '\n';
    }

    std::cout << "---------------------------------\n";

    // struct同一个数据有多个变量名
    {
        Vec3Struct v1 { 1, 2, 3 };

        std::cout << std::dec << "sizeof: " << sizeof(Vec3Struct<int>) << '\t' << sizeof(v1) << '\n';

        std::cout << v1.x << '\t' << v1.y << '\t' << v1.z << '\n'
                  << v1.r << '\t' << v1.g << '\t' << v1.b << '\n'
                  << v1.s << '\t' << v1.t << '\t' << v1.p << '\n';

        v1.x = 4;
        v1.y = 5;
        v1.z = 6;

        std::cout << v1.x << '\t' << v1.y << '\t' << v1.z << '\n'
                  << v1.r << '\t' << v1.g << '\t' << v1.b << '\n'
                  << v1.s << '\t' << v1.t << '\t' << v1.p << '\n';
    }

    return 0;
}
#endif // TEST8

#ifdef TEST9

#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// pod（plain old data）类型特征：
// 平凡数据类型
// 标准内存布局

struct MyPODType
{
    int a;
    char b[9];
    float c;
};

struct MyNotPODType
{
    int a;
    std::vector<char> b;
    float c;
};

struct MyStruct
{
    MyNotPODType* a;
    MyPODType* b;
    int c;
    std::string d;
    std::vector<int> e;
};

class MyClass1
{
public:
    MyClass1()  = default;
    ~MyClass1() = default;

private:
    int a;
    float b;
};

class MyClass2
{
public:
    MyClass2()  = default;
    ~MyClass2() = default;

private:
    int a = 0;
    std::string b;
};

union MyUnion {
    int a;
    float b;
    // std::string c;  // error union中不能有非pod类型
    MyPODType d;
    // MyNotPODType e; //error
};

int main()
{
    {
        MyPODType* pod1 = new MyPODType { 1, "111", 2.2f };
        MyPODType* pod2 = new MyPODType { 0, "", 0.0f };
        std::memcpy(pod2, pod1, sizeof(MyPODType)); // ok

        MyNotPODType* npod1 = new MyNotPODType { 2, { 'a', 'b', 'c' }, 2.2f };
        MyNotPODType* npod2 = new MyNotPODType { 0, {}, 0.0f };
        std::memcpy(npod2, npod1, sizeof(MyNotPODType)); // ok

        MyStruct* s1 = new MyStruct { npod2, pod2, 2, std::string("abc"), { 1, 2, 3 } };
        MyStruct* s2 = new MyStruct { nullptr, nullptr, 0, "", {} };
        std::memcpy(s2, s1, sizeof(MyStruct));                            // ok

        std::cout << std::is_standard_layout_v<MyNotPODType> << '\n';     // false
        std::cout << std::is_standard_layout_v<MyPODType> << '\n';        // true
        std::cout << std::is_standard_layout_v<MyUnion> << '\n';          // true
        std::cout << std::is_standard_layout_v<std::vector<int>> << '\n'; // false
        std::cout << std::is_standard_layout_v<std::string> << '\n';      // false
        std::cout << std::is_standard_layout_v<MyClass1> << '\n';         // true
        std::cout << std::is_standard_layout_v<MyClass2> << '\n';         // false

        std::cout << "---------------------------------\n";

        std::cout << std::is_trivial_v<MyNotPODType> << '\n';     // false
        std::cout << std::is_trivial_v<MyPODType> << '\n';        // true
        std::cout << std::is_trivial_v<MyUnion> << '\n';          // true
        std::cout << std::is_trivial_v<std::vector<int>> << '\n'; // false
        std::cout << std::is_trivial_v<std::string> << '\n';      // false
        std::cout << std::is_trivial_v<MyClass1> << '\n';         // true
        std::cout << std::is_trivial_v<MyClass2> << '\n';         // false

        std::cout << "---------------------------------\n";

        // 标准库不建议使用std::is_pod_v
        std::cout << std::is_pod_v<MyNotPODType> << '\n';     // false
        std::cout << std::is_pod_v<MyPODType> << '\n';        // true
        std::cout << std::is_pod_v<MyUnion> << '\n';          // true
        std::cout << std::is_pod_v<std::vector<int>> << '\n'; // false
        std::cout << std::is_pod_v<std::string> << '\n';      // false
        std::cout << std::is_pod_v<MyClass1> << '\n';         // true
        std::cout << std::is_pod_v<MyClass2> << '\n';         // false
    }

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <iostream>
#include <type_traits>

// 一个简单的Union类，它可以存int、char或double类型
union U {
    int i;
    char c;
    double d;

    U(const char* str) : c(str[0])
    {
    }
}; // non-POD

// sizeof(U)会取union里最大的数据成员，即sizeof(double)
typedef std::aligned_union<sizeof(U), int, char, double>::type U_pod;

int main()
{
    std::cout << std::is_standard_layout_v<U> << '\n'; // yes

    // U_pod代表U这种union的内存分配器
    // 分配一块区间，这个区间可以作为<int,char,double>对象的union，并没有创建union
    U_pod a, b; // default-initialized (ok: type is POD)

    // 在a对应的地址，调用placement new，调用U的ctor
    // 创建正真的union
    new (&a) U("hello"); // call U's constructor in place

    // 由于b和a是同种类型，而且是POD，可以直接进行赋值
    b = a; // assignment (ok: type is POD)

    // 把b直接转换成U&类型，然后print其i
    std::cout << reinterpret_cast<U&>(b).c << std::endl;

    return 0;
}

#endif // TEST10

#ifdef TEST11

#include <iostream>

struct CPUID
{
    uint32_t Stepping : 4;
    uint32_t Model : 4;
    uint32_t FamilyID : 4;
    uint32_t Type : 2;
    uint32_t Reserved1 : 2;
    uint32_t ExtendedModel : 4;
    uint32_t ExtendedFamilyID : 8;
    uint32_t Reserved2 : 4;
};

CPUID to_id(const uint32_t value)
{
    CPUID id;

    id.Stepping         = value & 0xf;
    id.Model            = value >> 4 & 0xf;
    id.FamilyID         = value >> 8 & 0xf;
    id.Type             = value >> 12 & 0x3;
    id.Reserved1        = value >> 14 & 0x3;
    id.ExtendedModel    = value >> 16 & 0xf;
    id.ExtendedFamilyID = value >> 20 & 0xff;
    id.Reserved2        = value >> 28 & 0xf;

    return id;
}

uint32_t from_id(const CPUID id)
{
    // clang-format off
    return id.Stepping 
         | id.Model << 4 
         | id.FamilyID << 8
         | id.Type << 12 
         | id.Reserved1 << 14
         | id.ExtendedModel << 16
         | id.ExtendedFamilyID << 20 
         | id.Reserved2 << 28;
    // clang-format on
}

int main()
{
    // 字节对齐
    std::cout << "size: " << sizeof(CPUID) << '\n';

    CPUID id(0b1000, 0b0100, 0b0010, 0b10, 0b01, 0b1100, 0b11001100, 0b0011);

    std::cout << id.Stepping << '\t' << id.Model << '\t' << id.FamilyID << '\t' << id.Type << '\t' << id.Reserved1 << '\t' << id.ExtendedModel << '\t'
              << id.ExtendedFamilyID << '\t' << id.Reserved2 << '\n';

    // 00_18_TEST5 二进制字面量
    auto id2 = to_id(0b0011'11001100'1100'0110'0010'0100'1000);
    auto val = from_id(id2);
    auto id3 = to_id(val);

    std::cout << id2.Stepping << '\t' << id2.Model << '\t' << id2.FamilyID << '\t' << id2.Type << '\t' << id2.Reserved1 << '\t' << id2.ExtendedModel
              << '\t' << id2.ExtendedFamilyID << '\t' << id2.Reserved2 << '\n';
    std::cout << val << '\n';
    std::cout << id3.Stepping << '\t' << id3.Model << '\t' << id3.FamilyID << '\t' << id3.Type << '\t' << id3.Reserved1 << '\t' << id3.ExtendedModel
              << '\t' << id3.ExtendedFamilyID << '\t' << id3.Reserved2 << '\n';
}

#endif // TEST11

#ifdef TEST12

#include <iostream>
#include <string>
#include <variant>
#include <vector>

int main()
{
    std::vector<std::variant<int, std::string>> vec;
    vec.emplace_back(1);
    vec.emplace_back("abc");
    vec.emplace_back(2);
    vec.emplace_back("xyz");
    vec.emplace_back(3);
    vec.emplace_back("rgb");

    std::vector<int> vInt;
    std::vector<std::string> vStr;

    //--------------------------------------------------------
    // 通过检查std::variant是否持有某个类型，对不同类型的数据进行不同操作
    for (auto elem : vec)
    {
        if (std::holds_alternative<int>(elem))
        {
            vInt.emplace_back(std::get<int>(elem));
        }
        else if (std::holds_alternative<std::string>(elem))
        {
            vStr.emplace_back(std::get<std::string>(elem));
        }
    }

    vInt.clear();
    vStr.clear();

    //--------------------------------------------------------
    // 泛型lambda
    auto lambda = [&vInt, &vStr](auto&& elem)
    {
        using T = std::decay_t<decltype(elem)>;

        if constexpr (std::is_same_v<T, int>)
        {
            vInt.push_back(elem);
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            vStr.push_back(elem);
        }
    };

    for (auto& elem : vec)
    {
        std::visit(lambda, elem);
    }

    vInt.clear();
    vStr.clear();

    //--------------------------------------------------------
    // 泛型lambda
    for (auto& elem : vec)
    {
        std::visit(
            [&vInt, &vStr](auto&& elem)
            {
                using T = std::decay_t<decltype(elem)>;

                if constexpr (std::is_same_v<T, int>)
                {
                    vInt.push_back(elem);
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    vStr.push_back(elem);
                }
            },
            elem);
    }
}

#endif // TEST12
