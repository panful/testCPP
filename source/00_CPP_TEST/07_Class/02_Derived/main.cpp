/*
 * 1. 继承方式的区别 虚继承（菱形继承）

 * 4. 基类的拷贝构造、拷贝赋值可以被继承
 * 5. 子类可以继承父类那些成员
 * 6. 基类的析构函数为什么要声明为虚函数
 * 7. 纯虚析构函数，抽象类的析构函数
 * 8. 父类函数带virtual和不带virtual 纯虚函数，dynamic_cast正确使用方法
 *
 * 10.crtp 单例基类 https://mp.weixin.qq.com/s/eai6rC0V5Ym1kgfTWl4QhQ
 * 11.虚基类、单一继承对象模型
 * 12.多继承对象模型
 * 13.虚继承对象模型
 * 14.Effective Modern C++ 需要改写的函数都应该加上override
 * 15.using 在类继承时的使用
 * 16.变长using声明
 */

#define TEST13

#ifdef TEST1

// public private protectd 继承：
// https://www.cnblogs.com/mmmmmmmmm/p/15166853.html

#include <iostream>

class A
{
public:
    void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t Member { 666 };
};

class B1 : public A
{
};

class B2 : public A
{
};

class C : public B1,
          public B2
{
};

// 虚继承
class D1 : virtual public A
{
};

class D2 : virtual public A
{
};

class E : public D1,
          public D2
{
};

int main()
{
    {
        C c;
        // c.Func(); // error 访问不明确
        // c.Member;

        c.B1::Func(); // ok
        c.B2::Func(); // ok
        std::cout << c.B1::Member << '\t' << c.B2::Member << '\n';
    }

    std::cout << "------------------------------------\n";

    {
        E e;
        e.Func();
        std::cout << e.Member << '\n';
    }

    std::cout << "------------------------------------\n";

    {
        D1 d1;
        D2 d2;
        d1.Func();
        d2.Func();
        std::cout << d1.Member << '\t' << d2.Member << '\n';
    }
}

#endif // TEST1

#ifdef TEST4

#include <iostream>

class Base
{
public:
    Base()
    {
    }

    virtual ~Base()
    {
    }

    Base(const Base&)            = delete;
    Base& operator=(const Base&) = delete;
};

class Derived : public Base
{
};

class Test
{
    // 编译器会自动生成：拷贝构造、拷贝赋值、移动构造、移动赋值
};

int main()
{
    {
        Test t;
        Test t2(t);
        Test t3(std::move(t2));
        t = t3;
    }

    {
        Derived d, d2;
        // Derived d3(d);               // 编译失败，基类拷贝构造函数被继承
        // Derived d4(std::move(d3));   // 编译失败
        // d = d2;                      // 编译失败
    }
}

#endif // TEST4

#ifdef TEST5

// operator= 参数是本类对象可以被继承，不是则不能被继承
// 除过=以外的其他重载运算符都可以被子类继承
// 拷贝构造（赋值）函数可以被继承(crtp单例模板基类)，子类不显式定义复制操作则不能使用复制操作
// 无参构造函数不能被继承
// 析构函数不能被继承
// 友元不会被继承
// static成员函数也可以继承、重写，但是不能使用virtual override修饰

#include <iostream>

//-------------------------------------

class A
{
public:
    A()
    {
        std::cout << "class A construct\n";
    };

    virtual ~A()
    {
        std::cout << "class A destruct\n";
    };
};

class B : public A
{
public:
    B()
    {
        std::cout << "class B construct\n";
    };

    virtual ~B()
    {
        std::cout << "class B destruct\n";
    };
};

//-------------------------------------
class C
{
public:
    C()
    {
        std::cout << "class C construct\n";
    };

    virtual ~C()
    {
        std::cout << "class C destruct\n";
    };
};

class D : public C
{
public:
    D()
    {
        std::cout << "class D construct\n";
    }

    ~D()
    {
        std::cout << "class D destruct\n";
    }
};

//-------------------------------------
class E
{
public:
    E()
    {
        std::cout << "class E construct\n";
    };

    virtual ~E()
    {
        std::cout << "class E destruct\n";
    };

public:
    void Func1()
    {
    }

protected:
    void Func2()
    {
    }

private:
    void Func3()
    {
    }
};

class F : public E
{
public:
    F()
    {
        std::cout << "class F construct\n";
    }

    ~F()
    {
        std::cout << "class F destruct\n";
    }

    void Func4()
    {
        this->Func1();
        this->Func2();
    }
};

//-------------------------------------
class G
{
public:
    G()
    {
        std::cout << "class G construct\n";
    };

    virtual ~G()
    {
        std::cout << "class G destruct\n";
    };

public:
    G(const G&)
    {
        std::cout << "copy construct\n";
    };

    const G& operator=(const G&)
    {
        return *this;
    }

    int operator+(int x)
    {
        return x;
    }
};

class H : public G
{
public:
    H()
    {
        std::cout << "class H construct\n";
    }

    ~H()
    {
        std::cout << "class H destruct\n";
    }
};

//-------------------------------------
class I
{
public:
    I()
    {
        std::cout << "class I construct\n";
    }

    I(const I&)                  = delete;
    const I& operator=(const I&) = delete;
};

class J : public I
{
public:
    J()
    {
    }

    J(const J&)
    {
    }

    const J& operator=(const J&)
    {
        return *this;
    }
};

//-------------------------------------
class K
{
public:
    K()
    {
        std::cout << "class K construct\n";
    }

    K(const K&)                  = delete;
    const K& operator=(const K&) = delete;
};

class L : public K
{
};

//-------------------------------------
class M
{
public:
    int operator=(int n)
    {
        return n * 2;
    }

    int operator=(const M&)
    {
        return 9;
    }
};

class N : public M
{
};

//-------------------------------------
class O
{
public:
    void operator=(const O&)
    {
        std::cout << "Object O\n";
    }
};

class P : public O
{
};

//-------------------------------------
class Q
{
public:
    void operator=(int n)
    {
        std::cout << "operator=(int)\n";
    }
};

class R : public Q
{
};

class S
{
public:
    static void Func()
    {
        std::cout << "static S::Func()\n";
    }

    static void Func2()
    {
        std::cout << "static S::Func()\n";
    }
};

class T : public S
{
public:
    static void Func2()
    {
        std::cout << "static T::Func2()\n";
    }
};

int main()
{
    {
        // 调用A的构造函数和析构函数
        auto&& a = B::A();
    }

    std::cout << "-----------------------------------\n";

    {
        // 调用顺序：
        // 1. 父类构造
        // 2. 子类构造
        // 3. 子类析构
        // 4. 父类析构
        D d;
    }
    std::cout << "-----------------------------------\n";
    {
        F f;
        f.Func1();
        f.Func4();
        // f.Func2(); // Func2只是没有权限访问（在F内部可以访问）并不是不能继承
    }
    std::cout << "-----------------------------------\n";
    {
        // operator+ 可以被继承
        G g;
        auto r1 = g + 1;
        std::cout << "G: operator + \t" << r1 << '\n';

        H h;
        auto r2 = h + 2;
        std::cout << "H: operator + \t" << r2 << '\n';

        // 赋值操作符返回的是当前对象类型时，可以继承
        G g1, g2;
        g1 = g2;
        G g3(g1);

        std::cout << "++++++++++\n";

        H h1, h2;
        h1 = h2;
        H h3(h1);
    }
    std::cout << "-----------------------------------\n";
    {
        J j1;
        J j2;
        j2 = j1;  // 父类赋值运算符声明为delete，子类重写了赋值运算符就可以使用
        J j3(j1); // 父类拷贝构造函数声明为delete，子类重写了拷贝构造函数就可以使用
    }
    std::cout << "-----------------------------------\n";
    {
        L l1;
        // L l2(l1); // error 父类拷贝构造函数是delete，子类没有重写
        L l3;
        // l3 = l1;  // error 父类赋值运算符是delete，子类没有重写
    }
    std::cout << "-----------------------------------\n";
    {
        M m1, m2;
        std::cout << (m1 = 2) << '\n';
        std::cout << (m2 = m1) << '\n';

        // 赋值操作符返回的不是当前对象类型时，不能继承
        N n1, n2;
        // n1 = 3; // error
        n2 = n1; // ok 如果父类和子类都没有重载=，编译器会自动添加这个函数
        // std::cout << (n1 = 3) << '\n';  // error
        // std::cout << (n2 = n1) << '\n'; // error
    }
    std::cout << "-----------------------------------\n";
    {
        O o1;
        O o2;
        o1 = o2;

        std::cout << "++++++++++++\n";

        P p1;
        P p2;
        p1 = p2;
    }
    std::cout << "-----------------------------------\n";
    {
        Q q1;
        Q q2;
        q1 = q2;
        q1 = 2;

        R r1;
        R r2;
        r1 = r2;
        // r1 = 2; // error
    }
    std::cout << "-----------------------------------\n";
    {
        T t;
        t.Func();
        t.Func2();
    }
    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <iostream>

class VirtualBase
{
public:
    VirtualBase()
    {
        std::cout << "VirtualBase construct\n";
    }

    virtual ~VirtualBase()
    {
        std::cout << "VirtualBase destruct\n";
    }
};

class VirtualDerived : public VirtualBase
{
public:
    VirtualDerived()
    {
        std::cout << "VirtualDerived construct\n";
    }

    ~VirtualDerived() override
    {
        std::cout << "VirtualDerived destruct\n";
    }
};

class NonVirtualBase
{
public:
    NonVirtualBase()
    {
        std::cout << "NonVirtualBase construct\n";
    }

    ~NonVirtualBase()
    {
        std::cout << "NonVirtualBase destruct\n";
    }
};

class NonVirtualDerived : public NonVirtualBase
{
public:
    NonVirtualDerived()
    {
        std::cout << "NonVirtualDerived construct\n";
    }

    ~NonVirtualDerived()
    {
        std::cout << "NonVirtualDerived destruct\n";
    }
};

// 在基类的析构函数为非虚析构函数的时候，并不一定会造成内存泄漏；
// 当派生类对象的析构函数中有内存需要收回，并且在编程过程中采用了基类指针指向派生类对象，如为了实现多态，并且通过基类指针将该对象销毁，
// 这时，就会因为基类的析构函数为非虚析构函数而不触发动态绑定，从而没有调用派生类的析构函数而导致内存泄漏。

int main()
{
    std::cout << "----------------------------------------\n";
    {
        // 正确释放
        NonVirtualDerived d;
    }
    std::cout << "----------------------------------------\n";
    {
        // 正确释放
        NonVirtualDerived* p = new NonVirtualDerived();
        delete p;
        p = nullptr;
    }
    std::cout << "----------------------------------------\n";
    {
        // 子类的析构函数不会被调用，可能会内存泄漏
        NonVirtualBase* p = new NonVirtualDerived();
        delete p;
        p = nullptr;
    }
    std::cout << "----------------------------------------\n";
    {
        // 正确析构
        NonVirtualBase* p = new NonVirtualDerived();
        auto p2           = static_cast<NonVirtualDerived*>(p);
        delete p2;
        p2 = nullptr;
    }
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++\n";
    {
        VirtualDerived d;
    }
    std::cout << "----------------------------------------\n";
    {
        VirtualDerived* p = new VirtualDerived();
        delete p;
        p = nullptr;
    }
    std::cout << "----------------------------------------\n";
    {
        // 因为基类的析构函数是虚函数，所以这里可以正确调用子类的析构函数
        VirtualBase* p = new VirtualDerived();
        delete p;
        p = nullptr;
    }
    std::cout << "----------------------------------------\n";
    {
        VirtualBase* p = new VirtualDerived();
        auto p2        = static_cast<VirtualDerived*>(p);
        delete p2;
        p2 = nullptr;
    }
    std::cout << "----------------------------------------\n";
}

#endif // TEST6

#ifdef TEST7

// https://blog.csdn.net/qq_33890670/article/details/80218731
// https://blog.csdn.net/suchto/article/details/54947869?fps=1&locationNum=9

#endif // TEST7

#ifdef TEST8

#include <iostream>

class A
{
public:
    void funA()
    {
        std::cout << "A\n";
    }
};

class B
{
public:
    // 使用 =0 函数必须带virtual关键字
    virtual void funB() = 0;
};

class C
{
public:
    virtual void funC()
    {
        std::cout << "C\n";
    }

    virtual ~C() = default;
};

class Test : public A,
             public B,
             public C
{
public:
    void funT()
    {
        std::cout << "Test\n";
    }

    void funA()
    {
        std::cout << "TA\n";
    };

    // void funA() override { std::cout << "TA\n"; }; //不能用override 因为基类B没有将函数funB声明为virtual

    // 子类必须定义父类的纯虚函数
    virtual void funB()
    {
        std::cout << "TB\n";
    }; // 重写父类的函数

    // void funB() override { std::cout << "TB\n"; } // 可以使用override父类对应的函数必然是virtual，所以子类可以不再使用virtual

    // 子类可以不实现父类的非纯虚函数，即可以不定义funC()
    void funC() override
    {
        std::cout << "TC\n";
    }
};

// 如果父类中的函数没有声明为virtual，调用父类还是子类的函数由调用对象的类型决定
// 如果父类中的函数声明为virtual，调用子类还是父类的函数由对象创建时的类型决定

int main()
{
    Test* t = new Test();
    t->funA(); // 子类
    t->funB(); // 子类
    t->funC(); // 子类

    A* a = new Test();
    a->funA(); // 父类

    B* b = new Test();
    b->funB(); // 子类，父类中的funB()为纯虚函数

    C* c = new Test();
    c->funC(); // 子类，因为父类函数是virtual，子类会重写该函数

    C* c1 = dynamic_cast<C*>(new Test()); // 子类转父类没必要使用dynamic_cast，直接转换就行，因为类型本来就是安全的
    c1->funC();                           // 子类

    // Test* t1 = new C(); //语法错误，编译器报错，不能用父类指针初始化子类指针
    // Test* t1 = new B(); //语法错误，因为类B是抽象类，不能实例化
    // Test* t1 = new A(); //语法错误，编译器报错，不能用父类指针初始化子类指针
    // Test* t1 = dynamic_cast<Test*>(new A()); //语法错误，因为类A不是多态基类，即没有使用virtual
    Test* t1 = dynamic_cast<Test*>(new C()); // 正确，但此处的t1为nullptr，如果转换引用此处会抛出异常，因为没有空引用
    t1->funT();                              // 正确，原因可以查看this指针博客，如果funT调用了成员变量就会报错

    // dynamic_cast的正确用法：
    C* cc    = new Test();
    Test* t2 = dynamic_cast<Test*>(cc); // 此处的t2不为nullptr
    t2->funT();                         // 正确，无论是否调用成员变量

    return 0;
}

#endif // TEST8

#ifdef TEST11

// https://www.cnblogs.com/QG-whz/p/4909359.html

#include <iostream>

class Base
{
public:
    virtual void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    Base()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual ~Base()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Func2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    int Member { 123 };
};

class Derived : public Base
{
public:
    Derived()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Func2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~Derived() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Func3()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    int DerivedMember { 456 };
};

// 64位程序，指针是uint64_t
// 以下测试基于MSVC 对于GCC等其他编译器并不一定正确

int main()
{
    typedef void (*Fun)();

    // 虚基类的模型：虚函数指针 + 所有成员变量
    {
        // 8 + 8 = 16 类的第一个位置是虚函数指针，8个字节。成员变量类型为int需要字节对齐
        std::cout << "sizeof(Base): " << sizeof(Base) << '\n';

        Base b;
        uint64_t* vptrAddress = (uint64_t*)(&b); // 虚函数指针，指向虚函数表，表中的虚函数顺序与类中声明的顺序一致
        std::cout << "vptr: " << vptrAddress << '\n';

        Fun vFunc0 = (Fun) * ((uint64_t*)*(uint64_t*)(&b)); // 类中的第0个虚函数，虚函数表中保存的是函数指针，所以需要两次uint64_t*类型转换
        vFunc0();
        Fun vFunc2 = (Fun) * ((uint64_t*)*(uint64_t*)(&b) + 2); // 类中的第2个虚函数，第1个是虚析构函数，没有类型无法获取到
        vFunc2();

        auto member = *(int*)((uint64_t*)(&b) + 1);
        std::cout << "Member: " << member << '\n'; // 成员变量
    }

    std::cout << "------------------------------------------\n";

    // 单一继承，子类也拥有自己单独的虚函数表
    // 模型：虚函数指针 + 所有成员变量（基类+子类自己）
    {
        // 8+8+8 = 24 虚表指针 + 所有成员变量（需要考虑内存对齐）
        std::cout << "sizeof(Derived): " << sizeof(Derived) << '\n';

        Derived d;

        auto func = (Fun) * (uint64_t*)*(uint64_t*)(&d); // Func()
        func();
        auto func2 = (Fun) * ((uint64_t*)*(uint64_t*)(&d) + 2); // Func2()
        func2();
        auto func3 = (Fun) * ((uint64_t*)*(uint64_t*)(&d) + 3); // Func3() 子类定义的虚函数附加到基类虚函数后边
        func3();

        auto baseMember    = *(int*)((uint64_t*)(&d) + 1); // 第1个是基类的成员变量
        auto deviredMember = *(int*)((uint64_t*)(&d) + 2); // 第2个是子类的成员变量
        std::cout << "Member: " << baseMember << '\t' << deviredMember << '\n';
    }
}

#endif // TEST11

#ifdef TEST12

#include <iostream>

class Base
{
public:
    virtual void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void BaseFunc()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual ~Base()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t Member { 123 };
};

class Base2
{
public:
    virtual void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Base2Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual ~Base2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t Member { 789 };
};

class Base3
{
public:
    virtual ~Base3()
    {
    }

    virtual void Base3Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

class Base4
{
public:
    virtual ~Base4()
    {
    }

    virtual void Base4Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

class Base5
{
public:
    virtual ~Base5()
    {
    }

    virtual void Base5Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

class Derived : public Base,
                public Base2,
                public Base3,
                public Base4,
                public Base5
{
public:
    void Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void DerivedFunc()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void BaseFunc() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Base2Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Base3Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Base4Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~Derived() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t Member { 666 };
};

// 64位程序，指针是uint64_t
/**
 * 子类继承自多个父类时，对象模型如下：
 * 虚函数表指针（子类自己独有的虚函数也会放在这个虚函数表中）
 * 成员变量指针（成员变量指针并不是必须的，如果基类没有成员变量，也就没有成员变量指针）
 * 虚函数表指针
 * 成员变量指针
 * ...
 * 子类的成员变量
 *
 * 注意：
 * overwrite时，所以基类的相同签名的函数都会被子类相同签名的函数覆盖
 * 内存布局中，父类按照声明顺序排列
 */

int main()
{
    typedef void (*Fun)();

    // 多继承
    // 模型：
    {
        std::cout << "sizeof(Derived): " << sizeof(Derived) << '\n';

        Derived d;
        uint64_t BaseMember { 0 }, Base2Member { 0 }, DerivedMember { 0 };

        {
            // 第0个父类的虚函数表
            auto vfptr = (uint64_t*)(&d);

            // 第0个父类的虚函数
            auto Func0 = (Fun) * (uint64_t*)*vfptr;
            Func0();
            auto Func1 = (Fun) * ((uint64_t*)*vfptr + 1);
            Func1();

            // 子类自己定义的虚函数
            auto Func = (Fun) * ((uint64_t*)*vfptr + 3);
            Func();

            // 第0个父类的成员变量
            BaseMember = *(uint64_t*)(vfptr + 1);
        }

        {
            // 第1个父类的虚函数表
            auto vfptr = (uint64_t*)(&d) + 2; // 加2表示第0个基类需要包含：虚函数表和成员变量两个指针

            // 第1个父类的虚函数
            auto Func0 = (Fun) * (uint64_t*)*vfptr;
            Func0();
            auto Func1 = (Fun) * ((uint64_t*)*vfptr + 1);
            Func1();

            // 第1个父类的成员变量
            Base2Member = *(uint64_t*)(vfptr + 1);
        }

        {
            // 第2个父类的虚函数表
            auto vfptr = (uint64_t*)(&d) + 4;

            // 第2个父类的虚函数，第0个虚函数是析构函数，无法获取
            auto Func1 = (Fun) * ((uint64_t*)*vfptr + 1);
            Func1();
        }

        {
            // 第3个父类的虚函数表
            auto vfptr = (uint64_t*)(&d) + 5; // 因为Base3没有成员变量，所以只需要加1

            // 第3个父类的虚函数，第0个虚函数是析构函数，无法获取
            auto Func1 = (Fun) * ((uint64_t*)*vfptr + 1);
            Func1();
        }

        {
            // 第4个父类的虚函数表
            auto vfptr = (uint64_t*)(&d) + 6;

            // 第4个父类的虚函数，第0个虚函数是析构函数，无法获取
            auto Func1 = (Fun) * ((uint64_t*)*vfptr + 1);
            Func1();
        }

        // 子类自己的成员变量
        {
            auto vptr     = (uint64_t*)(&d) + 7;
            DerivedMember = *vptr;
        }

        std::cout << "Member: " << BaseMember << '\t' << Base2Member << '\t' << DerivedMember << '\n';
    }
}

#endif // TEST11

#ifdef TEST13

#include <iostream>

class A
{
public:
    virtual void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Func2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Func3()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual ~A()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t AMember1 { 123 };
    uint64_t AMember2 { 234 };
};

class C
{
public:
    virtual void Func()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void Func2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual ~C()
    {
    }

    uint64_t CMember1 { 678 };
    uint64_t CMember2 { 789 };
};

class B : virtual public A,
          virtual public C
{
public:
    virtual void BFunc()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void BFunc2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void Func() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~B() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    uint64_t BMember { 666 };
};

int main()
{
    std::cout << "sizeof(A): " << sizeof(A) << "\nsizeof(B): " << sizeof(B) << std::endl;

    typedef void (*Fun)();

    {
        B b;
        auto ptr = (uint64_t*)(&b); // 虚函数表

        // B自己的虚函数(B可以没有自己的虚函数)
        auto B_func = (Fun) * ((uint64_t*)*(uint64_t*)(&b));
        B_func();
        auto B_func2 = (Fun) * ((uint64_t*)*(uint64_t*)(&b) + 1);
        B_func2();

        // 虚基类表指针，里面存放的是偏移值
        auto x = *(int*)*((uint64_t*)(&b) + 1); // 虚基类表指针到该类内存首地址的偏移值，如果B没有自己的虚函数此处为0，有虚函数则为-8(x64)
        auto y = *(int*)((int*)*((uint64_t*)(&b) + 1) + 1); // 最左虚继承父类的内存地址相对于虚基类表指针的偏移值
        auto z = *(int*)((int*)*((uint64_t*)(&b) + 1) + 2); // 次左虚继承父类的内存地址相对于虚基类表指针的偏移值
        // 其他的虚继承父类 ...
        std::cout << "vbptr: " << x << '\t' << y << '\t' << z << '\n';

        // B的成员变量
        auto B_member = *(uint64_t*)((uint64_t*)(&b) + 2); // 前面的uint64_t表示成员变量类型，此处可以不加
        std::cout << "B Member: " << B_member << '\n';

        // 分隔标记，好像只是用来隔离
        auto o = *((uint64_t*)(&b) + 3);
        std::cout << "offset: " << o << '\n';

        // 最左虚继承父类的虚函数（重写的虚函数不知道为啥调用不了）
        Fun fun0_1 = (Fun) * ((uint64_t*)*((uint64_t*)(&b) + 4) + 1);
        fun0_1();
        Fun fun0_2 = (Fun) * ((uint64_t*)*((uint64_t*)(&b) + 4) + 2);
        fun0_2();

        // 最左虚继承父类的成员变量
        auto m0_0 = *(uint64_t*)((uint64_t*)(&b) + 5);
        auto m0_1 = *(uint64_t*)(((uint64_t*)(&b) + 5) + 1);
        std::cout << "A Member: " << m0_0 << '\t' << m0_1 << '\n';

        // 次左虚继承父类。偏移值里边的第二个（次左）是56=7*8（8是x64指针大小），所以此处的8=1+7
        // 同样只能调用没有被子类重写的虚函数
        Fun fun1_1 = (Fun) * ((uint64_t*)*((uint64_t*)(&b) + 8) + 1);
        fun1_1();

        // 次左虚继承父类的成员变量
        auto m1_0 = *(uint64_t*)((uint64_t*)(&b) + 9);
        auto m1_1 = *(uint64_t*)(((uint64_t*)(&b) + 9) + 1);
        std::cout << "C Member: " << m1_0 << '\t' << m1_1 << '\n';
    }
}

#endif // TEST13

#ifdef TEST10

#include <iostream>
#include <string>

class Sub;

template <typename T>
class Base
{
public:
    void printInfo();
};

class Sub : public Base<Sub>
{
public:
    static std::string getClassName()
    {
        return "Sub\n";
    }

    void printA()
    {
        std::cout << "A\n";
    }

    void printB()
    {
        std::cout << "B\n";
    }
};

template <typename T>
void Base<T>::printInfo()
{
    std::cout << "Class Name:" << Sub::getClassName();

    ((T*)this)->printA();

    static_cast<T*>(this)->printB();

    // error 因为Base<Sub>不是多态基类
    // dynamic_cast<T*>(this)->printB();

    reinterpret_cast<T*>(this)->printB();
}

int main()
{
    Sub s;
    s.getClassName();
    s.printA();
    s.printB();
    s.printInfo();

    return 0;
}
#endif // TEST10

#ifdef TEST14

#include <iostream>

class Base
{
public:
    void Func1()
    {
        std::cout << "Base::Func1()\n";
    }

    void Func2()
    {
        std::cout << "Base::Func2()\n";
    }

    void Func3()
    {
        std::cout << "Base::Func3()\n";
    }

    virtual void Func4()
    {
        std::cout << "Base::Func4()\n";
    }

    virtual void Func5()
    {
        std::cout << "Base::Func5()\n";
    }

    virtual void Func6()
    {
        std::cout << "Base::Func6()\n";
    }
};

class Derived : public Base
{
public:
    // 因为基类的Func1没有virtual声明，所以子类不能使用override重写
    void Func1()
    {
        std::cout << "Derived::Func1()\n";
    }

    virtual void Func2()
    {
        std::cout << "Derived::Func2()\n";
    }

    virtual void Func3()
    {
        std::cout << "Derived::Func3()\n";
    }

    void Func4() override
    {
        std::cout << "Derived::Func4()\n";
    }

    virtual void Func5()
    {
        std::cout << "Derived::Func5()\n";
    }

    virtual void Func6() override
    {
        std::cout << "Derived::Func6()\n";
    }
};

class Derived2 : public Derived
{
public:
    // void Func1() override
    // {
    //     std::cout << "Derived2::Func1()\n";
    // }

    void Func2() override
    {
        std::cout << "Derived2::Func2()\n";
    }

    void Func3() override
    {
        std::cout << "Derived2::Func3()\n";
    }

    // Func4对应的父亲没有使用virtual，仍然可以改写
    // 因为Func4的爷爷带有virtual
    void Func4() override
    {
        std::cout << "Derived2::Func4()\n";
    }

    void Func5() override
    {
        std::cout << "Derived2::Func5()\n";
    }

    void Func6() override
    {
        std::cout << "Derived2::Func6()\n";
    }
};

int main()
{
    {
        Derived* d = new Derived();
        d->Func1();
        d->Func2();
        d->Func3();
        d->Func4();
        d->Func5();
        d->Func6();
    }
    std::cout << "---------------------------------\n";
    {
        Base* b = new Derived();
        b->Func1();
        b->Func2();
        b->Func3();
        b->Func4();
        b->Func5();
        b->Func6();
    }
    std::cout << "---------------------------------\n";
    {
        Base* b = new Derived2();
        b->Func1();
        b->Func2();
        b->Func3();
        b->Func4();
        b->Func5();
        b->Func6();
    }
    std::cout << "---------------------------------\n";
    {
        Derived* d = new Derived2();
        d->Func1();
        d->Func2();
        d->Func3();
        d->Func4();
        d->Func5();
        d->Func6();
    }

    return 0;
}

#endif // TEST14

#ifdef TEST15

// https://www.cnblogs.com/zwvista/p/9256655.html

#include <iostream>

struct A
{
    void f(int)
    {
        std::cout << "A::f(int)\n";
    }
};

struct B
{
    void f(double)
    {
        std::cout << "B::f(double)\n";
    }
};

struct S1 : A
{
    // 类S1继承了基类的f(int)函数
};

struct S2 : A
{
    // 类S2继承了基类的f(int)函数
    // 但是类S2定义了同名成员函数f(double)
    // 子类S2中的f(double)函数【覆盖】了基类同名成员函数
    void f(double)
    {
        std::cout << "S2::f(double)\n";
    }
};

struct S3 : A
{
    // 类S3继承了基类的f(int)函数
    // 类A定义了同名成员函数f(double)
    // 类S3通过using声明将基类的f(int)函数引入了类S3的作用域
    // 类S3的成员函数f(double)和基类同名成员函数f(int)形成了【重载】关系
    using A::f;

    void f(double)
    {
        std::cout << "S3::f(double)\n";
    }
};

struct S4 : A,
            B
{
    // 类S4继承了类A和类B的同名成员函数f，两者形成【竞争】关系
    // 实例调用时不能确定使用哪一个就会报错
};

struct S5 : A,
            B
{
    // 类S5同时继承了类A和类B的同名成员函数f
    // 类S5通过using声明将基类A和基类B的同名成员函数f都引入自己的作用域。
    // 基类A和基类B的同名成员函数在类S5中形成【重载】关系。
    using A::f;
    using B::f;
};

struct S6 : A,
            B
{
    // [0]和[1]是等价的 在 C++17 中多个 using 声明可以通过逗号连接起来，合成一个 using 声明。
    // using A::f; using B::f;  // [0]
    using A::f, B::f; // [1] C++17
};

int main()
{
    {
        S1 s;
        s.f(1); // A::f(int)
    }
    std::cout << "----------------------\n";
    {
        S2 s;
        s.f(1);   // S2::f(double)
        s.f(2.2); // S2::f(double)
    }
    std::cout << "----------------------\n";
    {
        S3 s;
        s.f(1);   // A::f(int)
        s.f(2.2); // S3::f(double)
    }
    std::cout << "----------------------\n";
    {
        // 调用的f函数不明确，编译器报错
        S4 s;
        // s.f(1);   // error
        // s.f(2.2); // error
    }
    std::cout << "----------------------\n";
    {
        S5 s;
        s.f(1);   // A::f(int)
        s.f(2.2); // B::f(double)
    }
    std::cout << "----------------------\n";
    {
        S6 s;
        s.f(1);   // A::f(int)
        s.f(2.2); // B::f(double)
    }

    return 0;
}

#endif // TEST15

#ifdef TEST16

#include <iostream>

namespace {
template <typename... Args>
struct MyStruct : Args...
{
    using Args::operator()...;
};

#if __cplusplus < 202002L
// 这是一个自动推断向导，用于帮助编译器根据 MyStruct 构造器【参数的类型】来推导 MyStruct 的模板参数类型。
// 这个自动推断向导告诉编译器，如果 MyStruct 构造器所有参数的类型的集合为Args，那么 MyStruct 的模板参数类型就是 Args 所包含的所有类型。
// 例如  MyStruct {1, 2.2, 3.3f} 的类型就是 MyStruct<int, double, float>
// C++20不需要这个，编译器就可以推导出模板参数类型
template <typename... Args>
MyStruct(Args...) -> MyStruct<Args...>;
#endif

// MyStruct 类的实例 s7 的构造器包含2个lambda参数，也可以看作2个各自包含一个 operator() 的函数对象。
// 根据 MyStruct 类的定义，s7 对象将继承这2个lambda（函数对象）的 operator() ，也就是说这2个lambda的 operator() 即函数体在 s7 对象内部形成重载关系。
// 根据 MyStruct 类的自动推断向导，s7 对象的类型为overloaded<T1, T2>，其中T1, T2为2个lambda参数的类型。
// MyStruct s7 { [](double) { std::cout << "lambda (double)\n"; }, [](int) { std::cout << "lambda (int)\n"; } };
} // namespace

int main()
{
    // s7定义在局部，MSVC会抛出栈损坏的异常，g++不会，具体原因未知
    MyStruct s7 { [](double) { std::cout << "lambda (double)\n"; }, [](int) { std::cout << "lambda (int)\n"; } };

    s7(1);   // lambda (int)
    s7(2.2); // lambda (double)
    // s7("abc");   // error 参数不匹配

    return 0;
}

#endif // TEST16