/*
* 1. std::optional
* 2. std::variant
* 3. std::visit 可以用来访问std::variant
* 4. std::any
* 5. 自定义any
* 6. std::any存储仿函数以及模板类 std::functional
* 7. 函数顺序执行队列  命令模式
* 8. union 内存共享 new(p) pp;
*/

#define TEST8

#ifdef TEST1

// std::optional c++17

#include <iostream>
#include <optional>
#include <string>

// convert string to int if possible:
std::optional<int> asInt(const std::string& s)
{
    try
    {
        return std::stoi(s);
    }
    catch (...)
    {
        return std::nullopt;
    }
}
int main()
{
    for (auto s : { "42", " 077", "hello", "0x33" })
    {
        // convert s to int and use the result if possible:
        std::optional<int> oi = asInt(s);
        if (oi) {
            std::cout << "convert '" << s << "' to int: " << *oi << "\n";
        }
        else {
            std::cout << "can't convert '" << s << "' to int\n";
        }
    }
}


#endif // TEST1

#ifdef TEST2


// std::variant 和 std::any的区别就是：any可以存任意类型的值，而variant只能存储指定类型的值（模板列表中的类型）
// std::variant类似于union TEST8
// std::tuple类似于struct

#include <variant>
#include <tuple>
#include <string>
#include <vector>
#include <iostream>

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
        //var1 = 2.0f;  //错误，variant模板初始化有多个float，不知道是给第几个float赋值
        std::variant<int, int, float, float, char, char> var11{ std::in_place_index<3>,2 }; // 初始化为第三个类型
        var1.emplace<1>(2.0f); //正确，指定具体第几个类型，会强制类型转换
        auto index1 = var1.index(); // 1
        var1.emplace<5>("test");    //正确
        auto index5 = var1.index(); // 5
        //var1.emplace<2>("test");  //错误
        //var1.emplace<int>(2);     //错误

        std::cout << index1 << '\t' << index5 << '\n';
    }

    {
        std::variant<int, float, char> var2;
        var2.emplace<int>(2); // 没有多个int类型时才可以这样赋值
        var2 = 2.0f;
        var2 = 3.0f; // 覆盖前面的值
        var2 = 'x';  // 覆盖前面的值
        auto index2 = var2.index(); // 此时var2的值为'x'，是var2的第2个类型（从0开始）
        //auto ret1 = std::get<1>(var2);   //抛出异常，因为var2的值为'x'是第2个类型
        //auto ret2 = std::get<int>(var2); //抛出异常，var2值的类型为char
        auto ret1 = std::get<2>(var2);     // ret1 = 'x'
        auto ret2 = std::get<char>(var2);  // ret2 = 'x'

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
        //vec.emplace_back("xxx");  //错误，因为variant中没有"xxx"的类型
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
        std::cout << "double:" << sizeof(double) << "\tfloat:" << sizeof(float) << "\tstd::string:" << sizeof(std::string) << "\tMyStruct:" << sizeof(MyStruct) << '\n';

        std::variant<int, float, double, char> var1;
        std::variant<int, float, char> var2;
        std::variant<int, float, char, std::string> var3;
        std::variant<int, int, int, int> var4;
        std::variant<int, char, int, int> var5;
        std::variant<char> var6;
        std::variant<int> var7;
        std::variant<MyStruct> var8;
        std::variant<MyStruct,int> var9;
        std::variant<MyStruct, int, MyStruct, double> var10;

        auto f = [](auto&& v) {
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

        union MyUnion1{
            int a; float b; double c; char d;
        };
        union MyUnion2{
            int a; float b; char c; std::string d;
        };

        struct MyStruct1{// 4+4+8+8 = 24
            int a; float b; double c; char d;
        };
        struct MyStruct2{// 4+4+4+40 + 4 = 56 因为std::string存储了一个指针（8位）因此sizeof(MyStruct2)是8（结构体中最大字节数的成员）的倍数
            int a; float b; char c; std::string d;
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
#include <variant>
#include <string>

template<typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

template<typename... Ts>
overload(Ts...)->overload<Ts...>;

auto twice = overload{
        [](std::string& s) { s += s; },
        [](auto& i) { i *= 2; },
};

int main()
{
    std::variant<int, std::string> var1(42), var3("visit");

    std::visit(twice, var1);
    std::visit(twice, var3);

    std::visit(overload{ // calls best matching lambda for current alternative
        [](int i) { std::cout << "int: " << i << '\n'; },
        [](const std::string& s) {
        std::cout << "string: " << s << '\n'; },
        },
        var1);

    std::visit(overload{ // calls best matching lambda for current alternative
       [](int i) { std::cout << "int: " << i << '\n'; },
       [](const std::string& s) {
       std::cout << "string: " << s << '\n'; },
        },
        var3);

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
//#include <initializer_list>
#include <vector>
#include <any>

int main()
{
    std::string a = "aa";
    std::vector<std::any>  vec{ 111,a,1.2f,"bbb" };
    const char* b = "aa";

    for (auto elem : vec)
    {
        int x = 0;
        //std::cout <<"vector type:" << elem.type().name() << std::endl;

        if (typeid(elem).name() == typeid(a).name())
        {
            std::cout << "string type:";
            std::cout << std::any_cast<std::string>(elem) << std::endl;  //进不来
        }
        if (elem.type().name() == typeid(a).name())
        {
            std::cout << "string type:";
            auto ret = std::any_cast<std::string>(elem);  //类型错误会抛出异常
            std::cout << ret << std::endl;
        }
        if (elem.type().name() == typeid(x).name())
        {
            std::cout << "int type:";
            x = std::any_cast<int>(elem);  //类型错误会抛出异常
            std::cout << x << std::endl;
        }
        if (elem.type().name() == typeid(b).name())
        {
            std::cout << "const char* type:";
            auto ret = std::any_cast<const char*>(elem);  //类型错误会抛出异常
            std::cout << ret << std::endl;
        }
    }
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <memory>
class MyAny
{
public:
    template <typename T>  //这里用模板函数不用模板类，因为如果是模板类，实例化一个MyAny对象就需要指定类型，这样做any就没有了意义，
    MyAny(T t) :base(std::make_unique<Data<T>>(t)) {}
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
        virtual ~Base() {} // 基类是一个多态类，必须定义析构函数
    };
    template <typename T>
    class Data :public Base
    {
    public:
        Data(T t) :value(t) {}

        T value;
    };
    //Data data;  //派生的子类Data需要模板参数才能创建，所以这里必须是没有模板的基类
    std::unique_ptr<Base> base; //如果是模板函数，不知道存储的数据类型，就需要在MyAny类内部再定义一个模板基类Base
    //Base base;  //不能用普通对象，因为无法把类型从Base转换到Data，没法访问value
};

int main()
{
    MyAny d(0);
    MyAny e = 'c';
    MyAny f = "test";

    auto ret0 = d.any_cast<int>(); // 这里的类型一定要一致，否则this指针为空
    auto ret1 = e.any_cast<char>();
    auto ret2 = f.any_cast<const char*>();
    auto ret3 = f.any_cast<char*>();  //throw error

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <any>

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
    std::function<int(int)> f1 = std::bind(&test1, 3);
    std::function<void(std::string)> f2 = std::bind(&test2, "sssssssss");
    std::vector<std::any> myFuns1;
    myFuns1.emplace_back(f1);
    myFuns1.emplace_back(f2);

    auto fun1 = std::any_cast<std::function<int(int)>>(myFuns1[0]);
    auto fun2 = std::any_cast<std::function<void(std::string)>>(myFuns1[1]);

    // 使用的参数还是std::bind中的实参
    fun1(2);
    fun2("www");

    std::cout << "====================================\n";

    // 2 必须用std::function接收std::bind返回值，才能用any_cast，如果用auto接收，any_cast会抛出异常
    auto f3 = std::bind(&test1, 999);
    auto f4 = std::bind(&test2, "test");

    std::vector<std::any> myFuns2;
    myFuns2.emplace_back(f3);
    myFuns2.emplace_back(f4);

    // 内部抛出异常
    try
    {
        auto fun3 = std::any_cast<std::function<int(int)>>(myFuns2[0]);
        auto fun4 = std::any_cast<std::function<int(int)>>(myFuns2[0]);
    }
    catch (...)
    {
        std::cout << "std::any_cast throw an error\n";
    }

    std::cout << "====================================\n";

    // 3 使用std::placeholders替换实参
    std::function<int(int)> f5 = std::bind(&test1, std::placeholders::_1);
    std::function<void(std::string)> f6 = std::bind(&test2, std::placeholders::_1);
    std::vector<std::any> myFuns3;
    myFuns3.emplace_back(f5);
    myFuns3.emplace_back(f6);

    auto fun5 = std::any_cast<std::function<int(int)>>(myFuns3[0]);
    auto fun6 = std::any_cast<std::function<void(std::string)>>(myFuns3[1]);

    // 实参将会使用此处的参数
    fun5(123);
    fun6("string");

    std::cout << "====================================\n";

    // 4 any存储模板类
    std::vector<std::any> myFuns4;
    A<int> a; a.t = 5;
    myFuns4.emplace_back(a);
    auto ret = std::any_cast<A<int>>(myFuns4[0]);
    std::cout << "value:" << ret.t << std::endl;

    return 0;
}

#endif // TEST6

#ifdef TEST7

 // https://blog.csdn.net/weixin_42837024/article/details/121252818

#include <iostream>
#include <queue>
#include <functional>

class FuncExecQueue
{
public:
    typedef std::function<void(void* param)> function_;

    struct FuncStruct
    {
        function_ func = nullptr;
        void* param = nullptr;

        FuncStruct() {}
        FuncStruct(const function_& f, void* p)
            : func(f), param(p) {}
    };

public:
    FuncExecQueue() {}
    ~FuncExecQueue() {}

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
    p->a = 100;
    p->b = 500;

    queue.push(FuncExecQueue::FuncStruct(&c, static_cast<void*>(p)));

    queue.run();

    system("pause");
}

#endif // TEST7

#ifdef TEST8

// 联合体的一些使用场景 https://zhuanlan.zhihu.com/p/495696559


// 利用union共享内存的机制组合拆分数据
#include <iostream>

union bit32_data
{
    uint32_t data{ 0 };
    struct
    {
        uint8_t byte0;
        uint8_t byte1;
        uint8_t byte2;
        uint8_t byte3;
    }byte;
};

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

        std::cout << std::hex
            << static_cast<int>(num.byte.byte3)
            << static_cast<int>(num.byte.byte2)
            << static_cast<int>(num.byte.byte1)
            << static_cast<int>(num.byte.byte0) << '\n';

        num.data = 0x12345678;

        std::cout << std::hex
            << static_cast<int>(num.byte.byte3)
            << static_cast<int>(num.byte.byte2)
            << static_cast<int>(num.byte.byte1)
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
        else {
            std::cout << "error\n";
        }
    }
}
#endif // TEST8
