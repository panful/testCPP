/*
 * 1. 普通构造，移动构造，拷贝构造，移动赋值，拷贝赋值
 * 2. 类初始化列表的顺序按成员变量在类中声明的顺序从上往下依次执行，不按初始化列表
 * 3. 类成员变量析构顺序
 * 4. 成员变量定义比构造函数更先执行
 * 5. 特种成员函数的生成机制 拷贝操作移动操作使用=default
 * 6. delete this，状态模式中进行状态转换时可能会用到delete this

 * 9. this指针，指针为nullptr访问成员函数

 * 11.深拷贝，浅拷贝
 * 12.成员函数的存储位置 00_05_TEST17 TEST9
 * 13.explicit
 */

#define TEST5

#ifdef TEST1

#include <iostream>

class A
{
public:
    int* p = nullptr;

public:
    A()
    {
        std::cout << "construction\n";
        p = new int(1);
    }

    explicit A(const A& a) noexcept
    {
        std::cout << "copy construction\n";
        this->p = new int(*a.p); // 深拷贝
    }

    A(A&& a) noexcept
    {
        std::cout << "move construction\n";
        this->p = new int(*a.p);
        a.p     = nullptr;
    }

    // 这种参数没必要存在，因为对一个const对象执行std::move方法是不合理的
    A(const A&& a) noexcept
    {
        std::cout << "const move construction\n";
    }

    const A& operator=(const A& a) noexcept
    { // 返回常引用，因为不可能将返回值赋值给一个const A 例如{A a1;const A a2; a2 = a1 //编译报错}
      // {A a1; auto a2 = a1; //调用拷贝构造函数}
        std::cout << "assignment\n";
        this->p = new int(*a.p);
        return *this;
    }

    const A& operator=(A&& a) noexcept
    {
        std::cout << "move assignment\n";
        this->p = new int(*a.p);
        a.p     = nullptr;
        return *this;
    }

    // 这种形参没必要，因为对一个const对象执行std::move方法是不合理的
    const A& operator=(const A&& a) noexcept
    {
        std::cout << "const move assignment\n";
        return *this;
    }

    ~A()
    {
        std::cout << "destruction\n";
        delete p;
        p = nullptr;
    }

    void setN(int _n)
    {
        n = _n;
    }

private:
    int n { 0 };
};

int main()
{

    {
        A a;               // 普通构造函数
        A b(a);            // 拷贝构造
        A c(std::move(a)); // 移动构造，没有移动构造函数则调用拷贝构造
    }

    std::cout << "-----------------------------\n";

    {
        A aa, bb, cc;       // 普通构造(3次）
        bb = std::move(aa); // 移动赋值，没有移动赋值运算符则调用普通赋值
        cc = bb;            // 普通赋值
    }

    std::cout << "-----------------------------\n";

    {
        const A dd1, dd2;
        // dd.setN(1); // error dd是const的，setN()不是const
        A ee;
        ee = std::move(dd1); // 会有警告：不要对常量使用std::move 调用参数为const&&的赋值操作
        ee.setN(1);

        const A ff = std::move(dd2); // 调用参数为const&&的拷贝构造函数
    }

    std::cout << "-----------------------------\n";

    {
        A a(A());  // 啥都不调用 gcc这里会有警告
        A b(A {}); // 普通构造函数，上面的'()'应该替换为该行的'{}'
    }

    std::cout << "-----------------------------\n";

    {
        A cc;
        A dd = std::move(cc); // 移动构造，没有移动构造函数则调用拷贝构造
        // A ee = dd;            //拷贝构造，注意和 {A a;a = b;}的区别，如果拷贝构造带explicit关键字，则会报错
    }

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <iostream>

class A
{
public:
    A()
    {
        std::cout << "A\n";
    }

    ~A()
    {
        std::cout << "~A\n";
    }
};

class B
{
public:
    B()
    {
        std::cout << "B\n";
    }

    ~B()
    {
        std::cout << "~B\n";
    }
};

class C
{
public:
    C()
    {
        std::cout << "C\n";
    }

    ~C()
    {
        std::cout << "~C\n";
    }
};

class Test
{
public:
    Test() : b(new B()), c(new C()), a(new A())
    {
        std::cout << "Test\n";
    }

private:
    A* a;
    B* b;
    C* c;
};

int main()
{
    // 先A 后B 再C 最后Test
    // 和初始化列表中的顺序没有关系，只和声明的顺序有关
    Test t;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <memory>

class A
{
public:
    A()
    {
        std::cout << "A\n";
    }

    ~A()
    {
        std::cout << "~A\n";
    }
};

class B
{
public:
    B()
    {
        std::cout << "B\n";
    }

    ~B()
    {
        std::cout << "~B\n";
    }
};

class C
{
public:
    C()
    {
        std::cout << "C\n";
    }

    ~C()
    {
        std::cout << "~C\n";
    }
};

class Test
{
public:
    Test()
    {
        m_b = std::make_unique<B>();
        m_c = std::make_unique<C>();
        m_a = std::make_unique<A>();
    }

private:
    std::unique_ptr<A> m_a { nullptr };
    std::unique_ptr<B> m_b { nullptr };
    std::unique_ptr<C> m_c { nullptr };
};

int main()
{
    // 构造：B C A，按照Test构造函数的函数体顺序执行

    // 析构：C B A，按照声明的顺序逆序执行
    Test t;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>

class A
{
public:
    A()
    {
        std::cout << "A\n";
    }
};

class Test
{
public:
    Test()
    {
        std::cout << "Test\n";
    }

    A a;
};

int main()
{
    Test t; // 先A 后Test
}

#endif // TEST4

#ifdef TEST5

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

class Test
{
private:
    Helper m_help;
};

class Test2
{
private:
    Helper m_help;

public:
    // 移动操作仅当类中未包含用户显式声明的复制操作、移动操作、析构函数时才生成
    // 所以Test2不会生成移动构造和移动赋值
    // 在已经存在显式声明析构函数的条件下生成复制操作已经成为废弃行为
    // 所以不建议声明了析构函数时不声明复制操作
    ~Test2()
    {
    }
};

class Test3
{
private:
    Helper m_help;

public:
    Test3()
    {
    }

    // 如果显式声明了移动操作，复制操作将被删除
    Test3(Test3&&) = default;

    // 如果显式声明了移动构造函数，移动赋值将不会自动生成
    // Test3& operator=(Test3&&) = default;
};

class Test4
{
private:
    Helper m_help;

public:
    // 将析构声明为 = default 移动操作将不会自动生成，生成复制操作已成为废弃行为
    // 即 =default 就相当于显式声明，只是实现是由编译器自动生成
    ~Test4() = default;
};

class Test5
{
private:
    Helper m_help;

public:
    Test5()                   = default;
    ~Test5()                  = default;
    Test5(const Test5&)       = default;
    Test5(Test5&&)            = default; // 注意形参必须是 Object&& 不能是 const Object&& 等其他参数
    Test5& operator=(Test5&)  = default; // 返回值类型必须是Obejct& 不能带const
    Test5& operator=(Test5&&) = default; // 同上
};

class Test6
{
private:
    Helper m_help;

public:
    Test6()
    {
    }

    ~Test6()
    {
    }

    Test6(const Test6&) = default;

    // 只要没有显式声明拷贝赋值和移动操作就会自动生成拷贝赋值
    // 不过如果声明了析构函数的条件下，生成拷贝操作已经被废弃
    // 拷贝构造同理
};

int main()
{
    std::cout << "------------------------------------\n";
    {
        Test t1;
        Test t2 {};
    }
    std::cout << "------------------------------------\n";
    {
        Test t1 {};
        Test t2(t1);
        Test t3(std::move(t1));
        t2 = t1;
        t2 = std::move(t1);
    }
    std::cout << "------------------------------------\n";
    {
        Test2 t1 {};
        Test2 t2(t1);
        Test2 t3(std::move(t1)); // 调用拷贝构造 因为Test2不会自动生成移动拷贝操作
        t2 = t1;
        t2 = std::move(t1);
    }
    std::cout << "------------------------------------\n";
    {
        Test3 t1 {};
        Test3 t2(std::move(t1)); // 移动构造
        // Test3 t3(t1);            // error
        // t2 = t1;                 // error
        // t2 = std::move(t1);      // error
    }
    std::cout << "------------------------------------\n";
    {
        Test4 t1 {};
        Test4 t2(std::move(t1)); // 拷贝构造
        Test4 t3(t1);            // 拷贝构造
        t2 = t1;                 // 拷贝赋值
        t2 = std::move(t1);      // 拷贝赋值
    }
    std::cout << "------------------------------------\n";
    {
        Test5 t1 {};
        Test5 t2(std::move(t1)); // 移动构造
        Test5 t3(t1);            // 拷贝构造
        t2 = t1;                 // 拷贝赋值
        t2 = std::move(t1);      // 移动赋值
    }

    std::cout << "------------------------------------\n";
    {
        Test6 t1 {};
        Test6 t2(std::move(t1)); // 拷贝构造
        Test6 t3(t1);            // 拷贝构造
        t2 = t1;                 // 拷贝赋值
        t2 = std::move(t1);      // 拷贝赋值
    }

    return 0;
}

#endif // TEST5

#ifdef TEST6

/*
 * 如何安全地使用delete
 *
 * 确保对象是new出来的，对象是分配在heap上的 查看05_Pointer placement new
 * 确保delete完后不会用该对象调用其他（非静态）成员函数
 * 确保delete完后不能访问对象的任何部分
 * 确保delete完后this指针不会被访问
 * 确保delete this不是在其基类实例函数成员中调用的
 */

// https://blog.csdn.net/lonelysky/article/details/52000276

#include <iostream>
#include <memory>

class Test
{
public:
    int m_int { 100 };

    void Print()
    {
        std::cout << "Test::m_int:\t" << m_int << std::endl;
    }
};

class A
{
public:
    A()
    {
        m_int  = 99;
        m_test = std::make_unique<Test>();
        std::cout << "construct\n";
    }

    ~A()
    {
        std::cout << "destruct\n";
    }

    void Destroy()
    {
        delete this;
    }

    A* Instance()
    {
        return this;
    }

    void PrintYes()
    {
        std::cout << "call member\t" << m_int << std::endl;
    }

    void PrintNo()
    {
        std::cout << "does not call\n";
    }

    void PrintPointer()
    {
        m_test->Print();
    }

private:
    int m_int;
    std::unique_ptr<Test> m_test;
};

int main()
{
    {
        A* a     = new A();
        auto ret = a->Instance();
        if (a == ret)
        {
            std::cout << "Instance == this\n";
        }
        ret->PrintYes();
        std::cout << " === call Destroy\n";
        a->Destroy();
        a->PrintNo();
        // a->PrintYes();      // 输出的值是一个随机值
        // a->PrintPointer();  // 崩溃
    }
    {
        A a;
        auto ret = a.Instance();
        if (ret == &a)
        {
            std::cout << "Instance == &A\n";
        }
        a.Destroy(); // 崩溃，因为a不是new出来的，所以不能delete this
        a.PrintNo();
        a.PrintYes();
    }
}
#endif // TEST6

#ifdef TEST9

#include <iostream>

class Test
{
public:
    static void f1()
    {
        std::cout << y << std::endl;
    }

    static void f2()
    {
        // 静态函数不能调用非静态成员变量
        // 因为调用静态函数并不会实例化对象，即非静态成员变量不会初始化，是一个非法值

        // std::cout << x << std::endl;
    }

    void f3()
    {
        // 相当于 std::cout<< this->x << std::endl;
        std::cout << x << std::endl;
    }

    void f4()
    {
        // 非静态函数可以调用静态成员变量
        // 因为静态成员变量程序启动时就会初始化，所有实例公用一个静态成员变量

        std::cout << y << std::endl;
    }

    void f5(int _x)
    {
        // 如果f5的参数为'int x'，那么还没执行'this->x = x;'之前，成员变量就已经被设置为了5
        this->x = _x;
    }

    void f6()
    {
        std::cout << "does not call member\n";
    }

private:
    int x { 0 };
    static int y;
};

int Test::y = 1;

class A
{
public:
    virtual void Func1()
    {
        std::cout << "A::Func1\n";
    }

    void Func2()
    {
        std::cout << "A::Func2\n";
    }
};

class B : public A
{
public:
    void Func3()
    {
        std::cout << "B::Func3\n";
    }

    void Func1() override
    {
        std::cout << "B::Func1\n";
    }
};

int main()
{
    // 普通类
    {
        // 只要函数没有调用【非静态成员变量】就可以使用nullptr访问成员函数
        Test* p = nullptr;
        p->f1(); // 正确
        p->f2(); // 正确
        // p->f3();  // 相当于调用 std::cout << nullptr->x << std::endl; 错误
        p->f4(); // 正确
        // p->f5(5); // 错误
        p->f6(); // 正确
    }

    // 含有继承关系的类
    {
        B* b = nullptr;
        // b->Func1(); // error 因为Func1()是虚函数，而b是nullptr没有虚表，所以调用失败
        b->Func2(); // Func2()不是虚函数，不需要虚表
        b->Func3();

        A* a = nullptr;
        // a->Func1();  // error
        a->Func2();
    }

    return 0;
}

#endif // TEST9

#ifdef TEST11
#include <iostream>

class Object
{
public:
    Object() : pValue(new int(0))
    {
        std::cout << "construct\n";
    }

    // Object(const Object& o) :pValue(o.pValue) {std::cout << "Shallow copy\n"; }  // 浅拷贝，类如果含有指针，调用析构函数会崩溃
    Object(const Object& obj) : pValue(new int(*obj.pValue))
    {
        std::cout << "Deep copy\n";
    } // 深拷贝

    ~Object()
    {
        std::cout << "destruct\n";
        if (pValue)
        {
            delete pValue;
            pValue = nullptr;
        }
    }

private:
    int* pValue;
};

int main()
{
    {
        Object obj1;
        Object obj2(obj1);
    }

    return 0;
}
#endif // TEST11

#ifdef TEST12

// https://blog.csdn.net/fuzhongmin05/article/details/59112081
// 每个对象所占用的存储空间只是该对象的数据部分（虚函数指针和虚基类指针也属于数据部分）所占用的存储空间，而不包括函数代码所占用的存储空间。
// 类成员函数存放在代码区
// this指针就是用来指向不同的对象

#endif // TEST12

#ifdef TEST13

#include <iostream>

class Test1
{
public:
    Test1(int x) : m_num1(x), m_num2(0)
    {
    }

    Test1(const Test1& t) : m_num1(0), m_num2(0)
    {
    }

private:
    int m_num1;
    int m_num2;
};

class Test2
{
public:
    explicit Test2(int x) : m_num1(x), m_num2(0)
    {
    }

    explicit Test2(const Test2& t) : m_num1(0), m_num2(0)
    {
    }

private:
    int m_num1;
    int m_num2;
};

int main()
{
    // 构造函数使用和不使用explicit的区别
    {
        Test1 t1 = 1;  // 会执行隐式转换
        Test1 t2 = t1; // 调用的是拷贝构造函数，不是operator =
    }

    {
        // Test2 t1 = 1; // error，使用了explicit之后，不支持隐式转换
        Test2 t2(1); // ok
    }

    // 拷贝构造函数使用explicit
    {
        Test2 t1(1);
        // Test2 t2 = t1; // error，拷贝构造函数被声明为explicit，不能使用=调用拷贝构造函数
        Test2 t3(t1); // ok，只能使用这种方式调用拷贝构造函数
    }

    return 0;
}

#endif // TEST13
