/*
* 1. sizeof class
* 2. 类初始化列表的顺序按成员变量在类中声明的顺序从上往下依次执行，不按初始化列表
* 3. 类成员变量析构顺序
* 4. 成员变量定义比构造函数更先执行
* 5. 子类可以继承父类那些成员
* 6. delete this，状态模式中进行状态转换时可能会用到delete this
* 7. 纯虚析构函数，抽象类的析构函数
* 8. 父类函数带virtual和不带virtual 纯虚函数，dynamic_cast正确使用方法
* 9. this指针，指针为nullptr访问成员函数
* 10.crtp 单例基类 https://mp.weixin.qq.com/s/eai6rC0V5Ym1kgfTWl4QhQ
* 11.深拷贝，浅拷贝
* 12.成员函数的存储位置 00_05_TEST17 TEST9
* 13.explicit
*/

// 02_07 构造函数、析构函数、拷贝构造函数的使用

#define TEST13

#ifdef TEST1

#include <iostream>

class A
{};
class B
{
    int n;
    A a;
};
class C :
    public A
{
};
class D
{
public:
    virtual void test() = 0;
};
class E :
    public D
{
public:
    void test() override{}
};

int main()
{
    std::cout << sizeof(A) << std::endl;
    A a;
    std::cout << sizeof(a) << std::endl;
    std::cout << sizeof(B) << std::endl;  // 内存对齐
    std::cout << sizeof(C) << std::endl;
    std::cout << sizeof(D) << std::endl;
    std::cout << sizeof(E) << std::endl;
    std::cout << "============\n";
    std::cout << sizeof(long) << std::endl;
    std::cout << sizeof(long long) << std::endl;
    std::cout << sizeof(long long int) << std::endl;
    std::cout << sizeof(long double) << std::endl; // 16 or 8?
    std::cout << sizeof(double) << std::endl;
    std::cout << sizeof(int) << std::endl;
    std::cout << sizeof(unsigned char) << std::endl;
    std::cout << sizeof(char) << std::endl;
    std::cout << sizeof(float) << std::endl;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

class A
{
public:
    A() { std::cout << "A\n"; }
    ~A() { std::cout << "~A\n"; }
};

class B
{
public:
    B() { std::cout << "B\n"; }
    ~B() { std::cout << "~B\n"; }
};

class C
{
public:
    C() { std::cout << "C\n"; }
    ~C() { std::cout << "~C\n"; }
};

class Test
{
public:
    Test() :
        b(new B()),
        c(new C()),
        a(new A())
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
    A() { std::cout << "A\n"; }
    ~A() { std::cout << "~A\n"; }
};

class B
{
public:
    B() { std::cout << "B\n"; }
    ~B() { std::cout << "~B\n"; }
};

class C
{
public:
    C() { std::cout << "C\n"; }
    ~C() { std::cout << "~C\n"; }
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
    std::unique_ptr<A> m_a{ nullptr };
    std::unique_ptr<B> m_b{ nullptr };
    std::unique_ptr<C> m_c{ nullptr };
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
    A() { std::cout << "A\n"; }
};

class Test
{
public:
    Test() { std::cout << "Test\n"; }

    A a;
};

int main()
{
    Test t; // 先A 后Test
}

#endif // TEST4

#ifdef TEST5

// operator= 返回的是本类对象可以被继承，返回其他对象或类型不能被继承
// 除过=以外的其他重载运算符都可以被子类继承
// 拷贝构造函数不能被继承
// 无参构造函数不能被继承
// 析构函数不能被继承
// 友元不会被继承

#include <iostream>

class A
{
public:
    A() { this->n = 9; std::cout << "AAA\n"; }

    A(const A& a) { this->n = a.n; std::cout << "&AAA\n"; }
    A& operator=(const A&) = delete;

    int get() { return n; }
    void set(int n_) { this->n = n_; }
protected:
    ~A() { std::cout << "~AAA\n"; }  // 基类的析构不能声明为私有的
    //A& operator=(const A& a) { this->n = a.n; std::cout << "=AAA\n"; return *this; } // 赋值操作符，注意区分和赋值操作符重载的区别
public:
    //int operator=(const int& x) { return x + 1; } // 赋值操作符重载
    //int operator+(const int& x) { return x + 1; } // 不能声明为私有和保护的，不然子类无法访问
private:
    int n{ 0 };

};

class B :
    public A
{
public:
    //B() { std::cout << "BBB\n"; }
    //~B() { std::cout << "~BBB\n"; }
    //B(const B& b) { std::cout << "&BBB\n"; }
    //B& operator=(const B& b) { std::cout << "=BBB\n"; return *this; }// 如果基类和子类都没有重载=，编译器会自动添加这个函数
    //B& operator= (const B&) = delete;
};

int main()
{
    B b1;
    b1.set(1);
    B b2;
    std::cout << b2.get() << std::endl;
    b2.set(2);
    std::cout << "===============\n";
    //b2 = b1;
    std::cout << b2.get() << std::endl; // 1

    //auto ret1 = (b2 = 1);
    //std::cout << (b2 + 1) << std::endl;  // 子类可以继承基类的operator+

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
    int m_int{ 100 };
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
        m_int = 99;
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
        A* a = new A();
        auto ret = a->Instance();
        if (a == ret)
        {
            std::cout << "Instance == this\n";
        }
        ret->PrintYes();
        std::cout << " === call Destroy\n";
        a->Destroy();
        a->PrintNo();
        //a->PrintYes();      // 输出的值是一个随机值
        //a->PrintPointer();  // 崩溃
    }
    {
        A a;
        auto ret = a.Instance();
        if (ret == &a)
        {
            std::cout << "Instance == &A\n";
        }
        a.Destroy();  // 崩溃，因为a不是new出来的，所以不能delete this
        a.PrintNo();
        a.PrintYes();
    }
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
    void funA() { std::cout << "A\n"; }
};

class B
{
public:
    // 使用 =0 函数必须带virtual关键字
    virtual    void funB() = 0;
};

class C
{
public:
    virtual void funC() { std::cout << "C\n"; }
    virtual ~C() = default;
};

class Test :
    public A,
    public B,
    public C
{
public:
    void funT() { std::cout << "Test\n"; }

    void funA() { std::cout << "TA\n"; };
    //void funA() override { std::cout << "TA\n"; }; //不能用override 因为基类B没有将函数funB声明为virtual

    // 子类必须定义父类的纯虚函数
    virtual void funB() { std::cout << "TB\n"; }; // 重写父类的函数
    //void funB() override { std::cout << "TB\n"; } // 可以使用override父类对应的函数必然是virtual，所以子类可以不再使用virtual

    // 子类可以不实现父类的非纯虚函数，即可以不定义funC()
    void funC() override { std::cout << "TC\n"; }
};

// 如果父类中的函数没有声明为virtual，调用父类还是子类的函数由调用对象的类型决定
// 如果父类中的函数声明为virtual，调用子类还是父类的函数由对象创建时的类型决定

int main()
{
    Test* t = new Test();
    t->funA();  //子类
    t->funB();  //子类
    t->funC();  //子类

    A* a = new Test();
    a->funA();  //父类

    B* b = new Test();
    b->funB();  //子类，父类中的funB()为纯虚函数

    C* c = new Test();
    c->funC();  //子类，因为父类函数是virtual，子类会重写该函数

    C* c1 = dynamic_cast<C*>(new Test()); //子类转父类没必要使用dynamic_cast，直接转换就行，因为类型本来就是安全的
    c1->funC(); //子类

    //Test* t1 = new C(); //语法错误，编译器报错，不能用父类指针初始化子类指针
    //Test* t1 = new B(); //语法错误，因为类B是抽象类，不能实例化
    //Test* t1 = new A(); //语法错误，编译器报错，不能用父类指针初始化子类指针
    //Test* t1 = dynamic_cast<Test*>(new A()); //语法错误，因为类A不是多态基类，即没有使用virtual
    Test* t1 = dynamic_cast<Test*>(new C());   //正确，但此处的t1为nullptr，如果转换引用此处会抛出异常，因为没有空引用
    t1->funT(); // 正确，原因可以查看this指针博客，如果funT调用了成员变量就会报错

    // dynamic_cast的正确用法：
    C* cc = new Test();
    Test* t2 = dynamic_cast<Test*>(cc);  //此处的t2不为nullptr
    t2->funT(); // 正确，无论是否调用成员变量

    return 0;
}

#endif // TEST8

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

        //std::cout << x << std::endl; 
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
    int x{ 0 };
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

class B :
    public A
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
        p->f1();  // 正确
        p->f2();  // 正确
        //p->f3();  // 相当于调用 std::cout << nullptr->x << std::endl; 错误
        p->f4();  // 正确
        //p->f5(5); // 错误
        p->f6();  // 正确
    }

    // 含有继承关系的类
    {
        B* b = nullptr;
        //b->Func1(); // error 因为Func1()是虚函数，而b是nullptr没有虚表，所以调用失败
        b->Func2();   // Func2()不是虚函数，不需要虚表
        b->Func3();

        A* a = nullptr;
        //a->Func1();  // error
        a->Func2();
    }

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <string>
#include <iostream>

class Sub;

template<typename T>
class Base
{
public:
    void printInfo();
};

class Sub :public Base<Sub>
{
public:
    static std::string getClassName() { return "Sub\n"; }
    void printA() { std::cout << "A\n"; }
    void printB() { std::cout << "B\n"; }
};

template<typename T>
void Base<T>::printInfo()
{
    std::cout << "Class Name:" << Sub::getClassName();

    ((T*)this)->printA();

    static_cast<T*>(this)->printB();

    // error 因为Base<Sub>不是多态基类
    //dynamic_cast<T*>(this)->printB();

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

#ifdef TEST11
#include <iostream>

class Object
{
public:
    Object() :pValue(new int(0)) { std::cout << "construct\n"; }
    //Object(const Object& o) :pValue(o.pValue) {std::cout << "Shallow copy\n"; }  // 浅拷贝，类如果含有指针，调用析构函数会崩溃
    Object(const Object& obj) :pValue(new int(*obj.pValue)) { std::cout << "Deep copy\n"; } // 深拷贝
    ~Object() {
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
    Test1(int x) :
        m_num1(x),
        m_num2(0)
    {}

    Test1(const Test1& t):
        m_num1(0),
        m_num2(0)
    {}
private:
    int m_num1;
    int m_num2;
};

class Test2
{
public:
    explicit Test2(int x) :
        m_num1(x),
        m_num2(0)
    {}

    explicit Test2(const Test2& t):
        m_num1(0),
        m_num2(0)
    {}
private:
    int m_num1;
    int m_num2;
};

int main()
{
    {
        Test1 t1 = 1;  // 会执行隐式转换
        Test1 t2 = t1; // 调用的是拷贝构造函数，不是operator =
    }

    {
        //Test2 t1 = 1; // error，使用了explicit之后，不支持隐式转换
        Test2 t2(1); // ok
    }

    {
        Test2 t1(1);
        //Test2 t2 = t1; // error，拷贝构造函数被声明为explicit，不能使用=调用拷贝构造函数
        Test2 t3(t1);    // ok，只能使用这种方式调用拷贝构造函数
    }

    return 0;
}

#endif // TEST13
