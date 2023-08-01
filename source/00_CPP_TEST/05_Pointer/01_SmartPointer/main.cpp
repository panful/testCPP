/*
 * 101 std::unique_ptr reset() release() 容器中保存std::unique_ptr
 * 102 自定义std::unique_ptr的释放方法
 * 103 将std::unique_ptr应用于工厂模式，std::unique_ptr转换为std::shared_ptr
 *
 * 201 std::shared_ptr的大小，控制块（引用计数）的创建
 * 202 std::enable_shared_from_this 如何安全地使用this创建std::shared_ptr
 * 203 std::shared_ptr reset() 主动释放控制权 引用计数
 * 204 自定义std::shared_ptr的删除器
 * 205 std::shared_ptr 线程安全问题
 * 206 多线程执行std::shared_ptr，它管理的对象的线程安全问题
 *
 * 301 std::weak_ptr.expired() use_count() 字节大小
 * 302 std::shared_ptr在当前类中创建并保存（std::weak_ptr），在儿子类中也保存一份（std::shared_ptr）
 * 303 std::weak_ptr.lock() 线程安全问题
 * 304 std::weak_ptr解决std::shared_ptr循环引用
 * 305 std::weak_ptr解决野指针（悬空指针）的问题
 * 306 std::weak_ptr用在观察者模式
 *
 * 401 std::unique_ptr和std::shared_ptr的get()函数，智能指针和裸指针混用的问题
 * 402 智能指针离开作用域前抛出异常，也可以正常释放
 * 403 std::make_shared使用的时机 std::allocate_shared允许自定义分配器
 * 404 使用new构造std::shared_ptr造成内存泄漏
 * 405 智能指针应用于多态
 */

#define TEST405

#ifdef TEST101

#include <iostream>
#include <memory>
#include <vector>

class A
{
public:
    A()
    {
        std::cout << "construct\n";
    }

    ~A()
    {
        std::cout << "destruct\n";
    }
};

int main()
{
    std::cout << "---------------------------------------------\n";
    // release()
    {
        auto a = std::make_unique<A>();
        if (a)
        {
            std::cout << "a is not nullptr\n";
        }

        // release只是释放控制权，会将控制的裸指针返回，并不销毁
        // 因此这里也不会调用A的析构函数
        auto p = a.release();

        if (a == nullptr)
        {
            std::cout << "a is nullptr\n";
        }
        if (p)
        {
            std::cout << "p is not nullptr\n";
        }

        // 必须手动将获取的裸指针释放，不然内存泄漏
        // 正常情况下不会这样使用
        delete p;
        p = nullptr;
    }

    std::cout << "---------------------------------------------\n";

    // reset()
    {
        std::unique_ptr<A> a {}; // 这里并不调用构造函数
        // a.reset(std::make_unique<A>());  // 错误
        // a.reset(new A); 正确，但不建议这样使用，new对应一个delete

        std::cout << "++++++++++++\n";

        // unique_ptr换绑其他对象
        a.reset(std::make_unique<A>().release());

        // 释放a，会立即调用A的析构函数，不同于std::shared_ptr
        a.reset();
        std::cout << "++++++++++++\n";
    }

    std::cout << "---------------------------------------------\n";

    // 容器中保存unique_ptr
    {
        std::vector<std::unique_ptr<A>> vecA;
        auto a = std::make_unique<A>();
        // vecA.emplace_back(a);  // error：尝试引用已删除的函数

        // 正确用法
        vecA.emplace_back(a.release());           // 使用release返回裸指针，然后插入vector
        vecA.emplace_back(std::make_unique<A>()); // 直接在vector上构造unique_ptr
        vecA.emplace_back(std::move(a));          // 使用移动语义将unique_ptr插入vector
    }

    std::cout << "---------------------------------------------\n";
    return 0;
}

#endif // TEST101

#ifdef TEST102

#include <iostream>
#include <memory>

class Obj
{
public:
    Obj()
    {
        std::cout << "construct\n";
    }

    ~Obj()
    {
        std::cout << "destruct\n";
    }
};

void MyDeleterFunc(Obj* p)
{
    std::cout << "use func to delete pointer\n";
    delete p;
    p = nullptr;
}

// 可以利用函数对象的重载对多种类型的指针进行释放
struct MyDeleter
{
    void operator()(Obj* p)
    {
        std::cout << "use functor to delete Obj\n";

        // 此处如果没有调用delete p,那么智能指针释放的时候，它所管理的Obj对象就不会调用析构
        delete p;
        p = nullptr;
    }

    void operator()(int* p)
    {
        std::cout << "use functor to delete int\n";
        // 此处如果没有调用delete p,那么智能指针释放的时候，它所管理的Obj对象就不会调用析构
        delete p;
        p = nullptr;
    }
};

//----------------------------------------------------------------------------------------
// 使用函数指针定义释放方法时，std::unique_ptr的尺寸会增加一到两个字长（word）
// 使用无状态的函数对象或无捕获的lambda表达式就不会增加尺寸
// 所以lambda表达式是最好的选择

int main()
{
    std::cout << "---------------------------------------------\n";

    {
        auto p = std::make_unique<int>(66);
        std::unique_ptr<int, MyDeleter> p1(new int());              // 调用自定义释放函数
        std::unique_ptr<int, MyDeleter> p2(new int(), MyDeleter()); // 调用自定义释放函数

        std::cout << sizeof(p1) << '\t' << sizeof(p2) << '\n';

        // 使用自定义释放方法不能使用std::make_unique???
        // auto p3 = std::make_unique<int, MyDeleter>(1,MyDeleter()); // error
        // auto p4 = std::make_unique<int>(2, MyDeleter()); // error
        // std::unique_ptr<int, MyDeleter> p5 = std::make_unique<int, MyDeleter>(3, MyDeleter()); // error
    }
    std::cout << "---------------------------------------------\n";

    {
        auto lambdaDel = [](int* p)
        {
            std::cout << "use lambda to delete int\n";
            delete p;
            p = nullptr;
        };

        auto p1 = std::unique_ptr<int, decltype(lambdaDel)>(new int(), lambdaDel);
        // auto p2 = std::make_unique<int, decltype(lambdaDel)>(1, lambdaDel); //error
        auto p3 = std::unique_ptr<int, decltype(lambdaDel)>(new int());

        std::cout << sizeof(p1) << '\t' << sizeof(p3) << '\n';
    }

    std::cout << "---------------------------------------------\n";

    {
        // std::unique_ptr<Obj, decltype(MyDeleterFunc)> p1(new Obj(), MyDeleterFunc); // error
        std::unique_ptr<Obj, void (*)(Obj*)> p2(new Obj(), MyDeleterFunc);

        std::cout << sizeof(p2) << '\n';
    }

    std::cout << "---------------------------------------------\n";

    return 0;
}

#endif // TEST102

#ifdef TEST103

#include <iostream>
#include <memory>

class Base
{
public:
    virtual ~Base() = default;
};

class Derived1 : public Base
{
};

class Derived2 : public Base
{
};

// 创建实例的工厂
auto MakeObject(int type)
{
    std::unique_ptr<Base> p(nullptr);

    if (1 == type)
    {
        p.reset(std::make_unique<Derived1>().release());
    }
    else if (2 == type)
    {
        p.reset(std::make_unique<Derived2>().release());
    }

    return p;
}

int main()
{
    // obj的类型是std::unique_ptr<Base>
    auto obj = MakeObject(1);

    // 将std::unique_ptr型别转换为std::shared_ptr型别
    std::shared_ptr<Base> shared_obj = MakeObject(2);
}

#endif // TEST103

//---------------------------------------------------------
#ifdef TEST201

#include <iostream>
#include <memory>

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

int main()
{
    std::cout << "------------------------------------------------------\n";

    {
        // std::shared_ptr的大小是裸指针的两倍
        // 一个指向数据的裸指针
        // 一个指向控制块的裸指针
        // 控制块包含引用计数、弱计数、删除器、内存分配器等
        std::shared_ptr<Helper> helper {};
        std::cout << sizeof(Helper) << '\t' << sizeof(helper) << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    {
        Helper* p1                 = new Helper();
        std::unique_ptr<Helper> p2 = std::make_unique<Helper>();

        // 以下3种方式都会创建一个控制块
        std::shared_ptr<Helper> sp1 = std::shared_ptr<Helper>(p1);
        std::shared_ptr<Helper> sp2 = std::shared_ptr<Helper>(p2.release());
        std::shared_ptr<Helper> sp3 = std::make_shared<Helper>();
    }

    std::cout << "------------------------------------------------------\n";

    {
        // 不要使用裸指针创建一个std::shared_ptr
        // 因为使用裸指针作为实参创建std::shared_ptr时，会创建一次控制块
        // 多次使用同一个裸指针创建std::shared_ptr就会导致同一个裸指针有多个控制块
        // 即有多个引用计数，引用计数变为0就会释放一次，导致多次释放
        Helper* p = new Helper();
        std::shared_ptr<Helper> h1(p);
        // std::shared_ptr<Helper> h2(p);
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST201

#ifdef TEST202

#include <iostream>
#include <memory>

class Test
{
public:
    Test()
    {
        std::cout << "construct\n";
    }

    ~Test()
    {
        std::cout << "destruct\n";
    }

public:
    std::shared_ptr<Test> GetSharedPtr()
    {
        // 返回的std::shared_ptr是通过裸指针this创建的，因此会创建一次控制块
        // 可以查看TEST201
        return std::shared_ptr<Test>(this);
    }
};

class Test2 : public std::enable_shared_from_this<Test2>
{
public:
    std::shared_ptr<Test2> GetSharedPtr()
    {
        return shared_from_this();
    }
};

class Test3 : public std::enable_shared_from_this<Test3>
{
public:
    std::shared_ptr<Test3> GetSharedPtr()
    {
        return shared_from_this();
    }

    static std::shared_ptr<Test3> Create()
    {
        return std::shared_ptr<Test3>(new Test3());
    }

private:
    Test3() = default;
};

// std::enable_shared_from_this 使用场景：
// 当你只有this指针的时候，还想要获得关于this的std::shared_ptr。
int main()
{
    std::cout << "------------------------------------------------------\n";

    // 以下代码会崩溃，因为会释放两次
    // {
    //     Test* t  = new Test();
    //     auto sp1 = t->GetSharedPtr();

    //     std::cout << "+++++\n";

    //     delete t;
    //     t = nullptr;

    //     std::cout << "+++++\n";
    // }

    std::cout << "------------------------------------------------------\n";

    // 以下代码会崩溃，因为会释放两次
    // {
    //     auto t   = std::make_shared<Test>();
    //     auto sp1 = t->GetSharedPtr();

    //     std::cout << "+++++\n";
    // }

    std::cout << "------------------------------------------------------\n";

    // 会崩溃，抛出一个std::bad_weak_ptr异常
    // {
    //     auto t   = new Test2();
    //     auto sp1 = t->GetSharedPtr();

    //     delete t;
    //     t = nullptr;
    // }

    std::cout << "------------------------------------------------------\n";

    // 不会崩溃
    {
        auto t   = std::make_shared<Test2>();
        auto sp1 = t->GetSharedPtr();
    }

    std::cout << "------------------------------------------------------\n";

    // 不会崩溃
    {
        // 因为Test3的构造函数被声明为私有的，所以只能通过静态函数Create()创建Test3
        // 这样可以杜绝使用new创建Test3从而导致崩溃的情况
        auto t   = Test3::Create();
        auto sp1 = t->GetSharedPtr();
        auto sp2 = t->GetSharedPtr();
    }
    std::cout << "------------------------------------------------------\n";

    return 0;
}

#endif // TEST202

#ifdef TEST203

#include <iostream>
#include <memory>
#include <vector>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

int main()
{
    std::cout << "------------------------------------------------------\n";

    // reset()
    {
        auto p = std::make_shared<Helper>();
        std::cout << p.use_count() << '\n';

        // 使用一个空的std::shared_ptr替换被p管理的对象
        // 相当于使p的引用计数减1
        p.reset();

        std::cout << "use count: " << p.use_count() << '\n';
        std::cout << "address: " << p << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    {
        std::vector<std::shared_ptr<Helper>> vec;

        auto p = std::make_shared<Helper>();

        std::cout << "use count:\t" << p.use_count() << '\n';
        vec.emplace_back(p);
        std::cout << "use count:\t" << p.use_count() << '\t' << vec.front().use_count() << '\n';

        // 此处执行完reset后，不会立即调用Helper的析构函数
        // 这里的reset只是把p对std::shared_ptr的控制权释放掉，vec的控制权还在
        // std::shared_ptr的引用计数减1
        p.reset();
        std::cout << "use count:\t" << p.use_count() << '\t' << vec.front().use_count() << '\n';
        std::cout << "address:\t" << p << '\t' << vec.front() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    // std::weak_ptr
    {
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();
        std::weak_ptr<Helper> wp   = sp;
        std::cout << wp.use_count() << '\t' << sp.use_count() << '\n';
        sp.reset(); // 立即调用析构
        std::cout << wp.use_count() << '\t' << sp.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    {
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();
        std::weak_ptr<Helper> wp   = sp;
        std::cout << wp.use_count() << '\t' << sp.use_count() << '\n';
        wp.lock().reset(); // 不会调用析构函数，原因看TEST403
        std::cout << wp.use_count() << '\t' << sp.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    // 将std::shared_ptr赋值为nullptr
    {
        std::shared_ptr<Helper> sp  = std::make_shared<Helper>();
        std::shared_ptr<Helper> sp2 = sp;

        std::cout << sp.use_count() << '\t' << sp2.use_count() << '\n';

        sp = nullptr;
        std::cout << sp.use_count() << '\t' << sp2.use_count() << '\n';

        sp2 = nullptr; // 立即调用析构，类似上面std::vector的示例
        std::cout << sp.use_count() << '\t' << sp2.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    // std::move(std::shared_ptr)
    {
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();
        std::cout << sp.use_count() << '\n';
        auto sp1 = sp;
        std::cout << sp.use_count() << '\t' << sp1.use_count() << '\n';
        auto sp2(sp);
        std::cout << sp.use_count() << '\t' << sp1.use_count() << '\t' << sp2.use_count() << '\n';

        // 移动不会增加引用计数，移动之后原来的std::shared_ptr引用计数将会变为0
        auto sp3 = std::move(sp);
        std::cout << sp.use_count() << '\t' << sp1.use_count() << '\t' << sp2.use_count() << '\t' << sp3.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    // std::shared_ptr.unique() 如果引用计数为1则返回true，不为1都是false
    {
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();
        std::cout << sp.use_count() << '\t' << sp.unique() << '\n';
        auto sp2 = sp;
        std::cout << sp.use_count() << '\t' << sp.unique() << '\t' << sp2.use_count() << '\t' << sp2.unique() << '\n';
        sp.reset();
        std::cout << sp.use_count() << '\t' << sp.unique() << '\t' << sp2.use_count() << '\t' << sp2.unique() << '\n';
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST203

#ifdef TEST204

#include <iostream>
#include <memory>

class Obj
{
public:
    Obj()
    {
        std::cout << "construct\n";
    }

    ~Obj()
    {
        std::cout << "destruct\n";
    }
};

// 自定义释放规则
void deleteInt(int* p)
{
    std::cout << "use func pointer delete\n";
    delete[] p;
}

// 自定义的释放规则
struct myDel
{
    void operator()(int* p)
    {
        std::cout << "use functor delete int\n";
        delete p;
    }

    void operator()(Obj* p)
    {
        std::cout << "use functor delete Obj\n";
        // 此处如果没有调用delete p,那么智能指针释放的时候，它所管理的Obj对象就不会调用析构
    }
};

int main()
{
    std::cout << "------------------------------------------------------\n";

    {
        // 指定 default_delete 作为释放规则
        std::shared_ptr<int> p(new int[10](), std::default_delete<int[]>());
    }

    std::cout << "------------------------------------------------------\n";

    {
        // 初始化智能指针，并自定义释放规则
        std::shared_ptr<int> p(new int[10](), deleteInt);
        std::shared_ptr<int> p2(new int[10](), myDel());

        // 删除器的型别对std::shared_ptr的型别没有影响
        // std::shared_ptr<int, decltype(deleteInt)> p3(new int[10](), deleteInt); // error
    }

    std::cout << "------------------------------------------------------\n";

    {
        // 使用lambda指定智能指针释放规则
        std::shared_ptr<int> p8(new int[8](),
            [](int* p)
            {
                delete[] p;
                std::cout << "use lambda delete\n";
            });
    }

    std::cout << "------------------------------------------------------\n";

    {
        std::shared_ptr<Obj> p1(new Obj(),
            [](Obj* p)
            {
                delete p;
                p = nullptr;
                std::cout << "use lambda delete Obj\n";
            });

        std::shared_ptr<Obj> p2(new Obj());
    }

    std::cout << "------------------------------------------------------\n";

    {
        // 使用std::make_shared不支持设置删除器
        // std::shared_ptr<int> p = std::make_shared<int>(new int(), [](int* p) {}); // error
        // auto p2                = std::make_shared<int>(new int(), [](int* p) {}); // error
    }

    std::cout << "------------------------------------------------------\n";

    return 0;
}

#endif // TEST204

#ifdef TEST205

// std::shared_ptr的引用计数是原子的，但是st::shared_ptr本身不是线程安全的
// 所以对std::shared_ptr读写还是需要加锁，构造、拷贝构造、析构
// c++20 std::atomic<std::shared_ptr> https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic2

// https://blog.csdn.net/solstice/article/details/8547547

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

static auto now = std::chrono::steady_clock::now();

class Helper
{
public:
    constexpr Helper() : m_number(new int(66))
    {
        std::cout << "Helper construct\n";
        std::cout << *m_number << '\t' << m_number << '\n';
    }

    ~Helper()
    {
        std::cout << "Helper destruct\n";

        delete m_number;
        m_number = nullptr;
    }

    void Print() const
    {
        std::cout << *m_number << '\t' << m_number << '\n';
    }

private:
    int* m_number { nullptr };
};

class Test
{
public:
    void Release()
    {
        std::this_thread::sleep_until(now + std::chrono::seconds(2));

        std::cout << "-- release\n";
        m_helper = nullptr;
    }

    void GoodUse() const
    {
        std::this_thread::sleep_until(now + std::chrono::seconds(1));

        std::cout << "-- use\n";

        // 拷贝赋值，引用计数加1，在其他地方对std::shared_ptr引用计数减一，并不会导致析构
        if (auto p = m_helper; p)
        {
            std::cout << p.use_count() << '\n';
            std::this_thread::sleep_until(now + std::chrono::seconds(3));
            std::cout << p.use_count() << '\n';

            std::cout << "print\n";
            p->Print();
        }

        std::cout << m_helper.use_count() << '\n';
    }

    void BadUse() const
    {
        std::this_thread::sleep_until(now + std::chrono::seconds(1));

        std::cout << "-- use\n";

        if (m_helper)
        {
            std::cout << m_helper.use_count() << '\n';
            std::this_thread::sleep_until(now + std::chrono::seconds(3));
            std::cout << m_helper.use_count() << '\n';

            // m_helper引用计数为0，管理的对象已经被析构，程序中断
            std::cout << "print\n";
            m_helper->Print();
        }

        std::cout << m_helper.use_count() << '\n';
    }

private:
    std::shared_ptr<Helper> m_helper { std::make_shared<Helper>() };
};

int main()
{
    std::cout << "------------------------------------------------------\n";

    std::vector<std::thread> threads;
    Test test;
    {
        // threads.emplace_back(std::thread(&Test::BadUse, &test));
        threads.emplace_back(std::thread(&Test::GoodUse, &test));
        threads.emplace_back(std::thread(&Test::Release, &test));
    }

    std::cout << "------------------------------------------------------\n";

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "------------------------------------------------------\n";
}

#endif // TEST205

#ifdef TEST206

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

class Accumulation
{
public:
    void Worker()
    {
        for (size_t i = 0; i < m_count; i++)
        {
            m_number++;
        }
    }

    void PrintResult()
    {
        std::cout << "Result of a thread: " << m_number << '\n';
    }

private:
    int m_number { 0 };
    size_t m_count { 100000 };
};

class Test
{
public:
    void Worker() const
    {
        // 如果不使用锁累加的结果就是错误的
        // std::lock_guard<std::mutex> lock(m_mutex);
        m_accumulation->Worker();
    }

    void PrintResult() const
    {
        m_accumulation->PrintResult();
    }

private:
    std::shared_ptr<Accumulation> m_accumulation { std::make_shared<Accumulation>() };
    mutable std::mutex m_mutex;
};

// 将Accumulation中的m_number声明为 std::atomic<int> 不需要加锁就可以正确累加

int main()
{
    // 多个线程执行累加任务
    {
        std::vector<std::thread> threads;
        Accumulation a;

        for (size_t i = 0; i < 10; i++)
        {
            threads.emplace_back(std::thread(&Accumulation::Worker, &a));
        }

        for (auto& t : threads)
        {
            t.join();
        }

        a.PrintResult();
    }

    // 使用std::shared_ptr调用累加任务
    {
        std::vector<std::thread> threads;
        Test a;

        for (size_t i = 0; i < 10; i++)
        {
            threads.emplace_back(std::thread(&Test::Worker, &a));
        }

        for (auto& t : threads)
        {
            t.join();
        }

        a.PrintResult();
    }

    return 0;
}

#endif // TEST206

//---------------------------------------------------------
#ifdef TEST301

#include <iostream>
#include <memory>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

int main()
{
    std::cout << "------------------------------------------------------\n";

    // expired() use_count()
    {
        std::weak_ptr<Helper> wp;
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();

        // expired(): 当std::weak_ptr引用的std::shared_ptr无效时返回true
        // std::weak_ptr和std::shared_ptr的use_count()不是同一个东西
        std::cout << wp.expired() << '\t' << wp.use_count() << '\t' << sp.use_count() << '\n';
        wp = sp;
        std::cout << wp.expired() << '\t' << wp.use_count() << '\t' << sp.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    // 字节大小
    {
        // std::weak_ptr和std::shared_ptr的尺寸相同，和std::shared_ptr使用相同的控制块
        // 构造、析构、赋值操作都包含了对引用计数的原子操作
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();
        std::weak_ptr<Helper> wp   = sp;
        std::cout << sizeof(sp) << '\t' << sizeof(wp) << '\n';
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST301

#ifdef TEST302

#include <iostream>
#include <memory>

class A
{
public:
    A()
    {
        std::cout << "A()\n";
    }

    ~A()
    {
        std::cout << "~A()\n";
    }
};

class B
{
public:
    B()
    {
        std::cout << "B()\n";
    }

    ~B()
    {
        std::cout << "~B()\n";
        std::cout << "A count in B: " << m_a.use_count() << "\n";
    }

    void SetA(const std::shared_ptr<A>& a)
    {
        m_a = a;
    }

    std::shared_ptr<A> m_a { nullptr };
};

class C
{
public:
    C()
    {
        std::cout << "C()\n";
        m_b = std::make_shared<B>();
        // m_a = std::make_shared<A>();
        auto pa = std::make_shared<A>();
        m_a     = pa;
        m_b->SetA(pa);
    }

    ~C()
    {
        std::cout << "~C()\n";
        std::cout << "A count in C: " << m_a.use_count() << '\n';
        std::cout << "B count in C: " << m_b.use_count() << '\n';
    }

private:
    // std::shared_ptr<A> m_a和std::shared_ptr<B> m_b这两个成员的声明顺序不一样，打印的结果也不一样（即使都会成功析构）
    // 合理的用法是，此处的m_a应该声明为std::weak_ptr，且std::weak_ptr应该在类C，而不是类B
    // 因为如果将std::shared_ptr<A>放在C中，B中的是std::weak_ptr<A>，且c中的m_a声明在m_b之后，就会先析构A，导致B的m_a无效
    // 如果还需要在B中的析构函数中使用m_a就会出问题

    // std::shared_ptr<A> m_a{};
    std::shared_ptr<B> m_b {};

    // m_a在类C中的声明顺序无所谓，它只是弱指针，析构时不会使A的引用计数减一也不会调用A的析构
    std::weak_ptr<A> m_a {};
};

int main()
{
    std::cout << "------------------------------------------------------\n";

    {
        std::cout << "=========\n";
        std::unique_ptr<C> c = std::make_unique<C>();
        std::cout << "=========\n";
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST302

#ifdef TEST303

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

static auto now = std::chrono::steady_clock::now();

class Helper
{
public:
    Helper() : m_number(new int(66))
    {
        std::cout << "Helper construct\n";

        printf("%d, %p\n", *m_number, m_number);
    }

    ~Helper()
    {
        std::cout << "Helper destruct\n";

        delete m_number;
        m_number = nullptr;
    }

    void Print()
    {
        printf("%d, %p\n", *m_number, m_number);
    }

private:
    int* m_number { nullptr };
};

class Worker
{
public:
    Worker()
    {
        std::cout << "Worker->Helper expired: " << m_helper.expired() << '\n';
    }

    void SetPtr(const std::shared_ptr<Helper>& sp)
    {
        m_helper = sp;
        std::cout << "Worker->Helper expired: " << m_helper.expired() << '\n';
    }

public:
    void Run()
    {
        std::cout << "------------------ Run ------------------\n";

        if (!m_helper.expired())
        {
            std::cout << "Worker->Helper expired: " << m_helper.expired() << '\n';

            // 睡眠到程序启动后经过3s的时刻
            std::this_thread::sleep_until(now + std::chrono::seconds(3));
            std::cout << "Worker->Helper expired: " << m_helper.expired() << '\n';

            // 使用if判断std::weak_ptr的时候，它还是有效的
            // 但是经过一定时间后，lock()获取的std::shared_ptr已经为空
            // 因此为了线程安全，必须在使用lock()返回的std::shared_ptr时再判断一次
            // C++标准规定：lock()是原子操作
            if (auto sp = m_helper.lock())
            {
                std::cout << "lock() is valid ptr\t" << sp << '\n';
                sp->Print();
            }
            else
            {
                std::cout << "lock() is nullptr\n";
            }
        }
    }

private:
    std::weak_ptr<Helper> m_helper;
};

int main()
{
    std::thread t {};
    Worker w {};

    {
        std::shared_ptr<Helper> sp = std::make_shared<Helper>();

        w.SetPtr(sp);
        t = std::thread(&Worker::Run, &w);

        std::this_thread::sleep_until(now + std::chrono::seconds(1));
    }

    std::cout << "---------------------------------\n";

    t.join();

    return 0;
}

#endif // TEST303

#ifdef TEST304

#include <iostream>
#include <memory>

class B;

class A
{
public:
    A()
    {
        std::cout << "A()\n";
    }

    ~A()
    {
        std::cout << "~A()\n";
    }

    // 用std::weak_ptr替换std::share_ptr，避免循环引用造成内存泄漏
    std::weak_ptr<B> m_b;

    // std::shared_ptr<B> m_b;
};

class B
{
public:
    B()
    {
        std::cout << "B()\n";
    }

    ~B()
    {
        std::cout << "~B()\n";
    }

    // std::weak_ptr替换std::shared_ptr
    std::weak_ptr<A> m_a;

    // std::shared_ptr<A> m_a;
};

int main()
{
    std::cout << "---------------------------------------------\n";

    {
        auto pa = std::make_shared<A>();
        auto pb = std::make_shared<B>();

        pa->m_b = pb;
        pb->m_a = pa;

        // use pointer
        // pa->m_b.lock();
        // pb->m_a.lock();

        // 如果类A和类B中都定义了一个对方的shared_ptr，此处引用计数就会输出为2，
        // 离开作用域时不会调用析构函数，导致内存泄漏
        std::cout << "A use count:" << pa.use_count() << std::endl;
        std::cout << "B use count:" << pb.use_count() << std::endl;
    }

    std::cout << "---------------------------------------------\n";
    return 0;
}

#endif // TEST304

#ifdef TEST305

#include <iostream>
#include <memory>

// 野指针（悬空指针）：delete之后没有置为nullptr

int main()
{

    std::cout << "------------------------------------------------------\n";

    // 野指针示例
    {
        auto p = new int(6);
        std::cout << *p << '\t' << p << '\n';

        delete p;
        // p = nullptr;

        // 如果不将p置为nullptr，即p是一个野指针
        // 那么对p解引用就会打印一个随机值
        if (p)
        {
            std::cout << *p << '\t' << p << '\n';
        }
    }

    std::cout << "------------------------------------------------------\n";

    // 使用std::weak_ptr解决野指针的问题
    {
        std::shared_ptr<int> sp = std::make_shared<int>(3);
        std::weak_ptr<int> wp   = sp;

        if (!wp.expired())
        {
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            if (auto p = wp.lock())
            {
                std::cout << *p << '\n';
            }
        }
        else
        {
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            std::cout << "nullptr\n";
        }

        // 给std::shared_ptr换绑其他对象
        sp = std::make_shared<int>(4);
        std::cout << "-----------------------------\n";

        if (!wp.expired())
        {
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            if (auto p = wp.lock())
            {
                std::cout << *p << '\n';
            }
        }
        else
        {
            // std::weak_ptr弱引用的std::shared_ptr如果指向了其他地方，原来的std::weak_ptr就会失效
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            std::cout << "nullptr\n";
        }

        // 将新的std::shared_ptr重新绑定到std::weak_ptr就会使std::weak_ptr生效
        wp = sp;
        std::cout << "-----------------------------\n";

        if (!wp.expired())
        {
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            if (auto p = wp.lock())
            {
                std::cout << *p << '\n';
            }
        }
        else
        {
            std::cout << "wp.expired()\t" << wp.expired() << '\n';
            std::cout << "nullptr\n";
        }
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST305

//---------------------------------------------------------
#ifdef TEST401

#include <iostream>
#include <memory>

class A
{
public:
    A()
    {
        std::cout << "A::construct\n";
    }

    ~A()
    {
        std::cout << "A::destruct\n";
    }

    void Set(int a)
    {
        m_int = a;
    }

    int Get() const
    {
        return m_int;
    }

private:
    int m_int { 0 };
};

class B
{
public:
    void SetPointerA(A* a)
    {
        m_a = a;
    }

    A* GetPointerA() const
    {
        return m_a;
    }

private:
    A* m_a { nullptr };
};

class C
{
public:
    void SetPointerA(std::shared_ptr<A> a)
    {
        m_a = a;
    }

    std::shared_ptr<A> GetPointerA() const
    {
        return m_a;
    }

private:
    std::shared_ptr<A> m_a { nullptr };
};

// 智能指针和裸指针最好不要混合使用
// 智能指针的get()函数只是获取智能指针管理的裸指针，并不会使引用计数加1，
// 所以如果智能指针已经释放，之前使用get()返回的裸指针也会被释放

int main()
{
    std::cout << "------------------------------------------------------\n";

    // std::unique_ptr.get()
    // 将std::unique_ptr管理的裸指针传给函数实参
    {
        B b;
        {
            auto pa = std::make_unique<A>();
            pa->Set(9);
            b.SetPointerA(pa.get());
            std::cout << b.GetPointerA()->Get() << "\n";
        } // 离开作用域pa就会被释放

        // a对应的智能指针已经被析构，所以打印的是随机值
        std::cout << b.GetPointerA() << '\t' << b.GetPointerA()->Get() << std::endl;
    }

    std::cout << "------------------------------------------------------\n";

    // std::shared_ptr.get()
    // 将std::shared_ptr管理的裸指针传给函数实参
    {
        B b;
        {
            auto pa = std::make_shared<A>();
            pa->Set(9);
            b.SetPointerA(pa.get());
            std::cout << b.GetPointerA()->Get() << "\n";
        } // 离开作用域pa就会被释放，调用A的析构函数

        // a对应的智能指针已经被析构，所以打印的是随机值
        std::cout << b.GetPointerA() << '\t' << b.GetPointerA()->Get() << std::endl;
    }

    std::cout << "------------------------------------------------------\n";

    // 直接把std::shared_ptr对象传递给函数实参
    {
        C c;
        {
            auto pa = std::make_shared<A>();
            pa->Set(8);
            c.SetPointerA(pa);
            std::cout << c.GetPointerA()->Get() << "\n";
        } // 离开作用域并不会释放pa std::shared_ptr ，因为C会控制pa

        std::cout << c.GetPointerA() << '\t' << c.GetPointerA()->Get() << std::endl;
    }

    std::cout << "------------------------------------------------------\n";

    // 不要释放get返回的裸指针
    {
        // auto p = std::make_shared<int>(66);
        // std::cout << p.use_count() << '\n';
        // auto p2 = p.get();
        // std::cout << p.use_count() << '\n';
        // // 崩溃，提示两次释放
        // delete p2;
        // p2 = nullptr;
        // std::cout << p.use_count() << '\n';
    }

    std::cout << "------------------------------------------------------\n";

    return 0;
}

#endif // TEST401

#ifdef TEST402

#include <iostream>
#include <memory>
#include <vector>

class Helper
{
public:
    Helper()
    {
        std::cout << "Helper construct\n";
    }

    ~Helper()
    {
        std::cout << "Helper destruct\n";
    }
};

void TestUnique()
{
    auto p = std::make_unique<Helper>();

    std::cout << "Test unique start\n";
    throw std::runtime_error("throw unique");
    std::cout << "Test unique end\n";
}

void TestShared()
{
    auto p = std::make_shared<Helper>();

    std::cout << "Test shared start\n";
    throw std::runtime_error("throw shared");
    std::cout << "Test shared end\n";
}

int main()
{
    std::cout << "------------------------------------------------------\n";

    try
    {
        TestUnique();
    }
    catch (...)
    {
        std::cout << "catch an error\n";
    }

    std::cout << "------------------------------------------------------\n";

    try
    {
        TestShared();
    }
    catch (...)
    {
        std::cout << "catch an error\n";
    }

    std::cout << "------------------------------------------------------\n";
    return 0;
}

#endif // TEST402

#ifdef TEST403

#include <iostream>
#include <memory>

class Helper
{
public:
    Helper()
    {
        std::cout << "Helper construct\n";
    }

    ~Helper()
    {
        std::cout << "Helper destruct\n";
    }
};

void* operator new(size_t size)
{
    std::cout << "new size: " << size << '\n';
    return malloc(size);
}

void operator delete(void* p)
{
    std::cout << "delete\n";
    free(p);
}

int main()
{
    {
        // 会调用两次new，一次是new int 一次是new控制块
        auto p = std::shared_ptr<int>(new int());
    }

    std::cout << "----------------------------------------------------\n";

    {
        // 只会new一次
        // 管理的对象和控制块将在同一块内存上分配
        auto p = std::make_shared<int>();
    }

    std::cout << "----------------------------------------------------\n";

    // 因为被管理的对象和控制块在同一块内存，控制块还包含着第二个引用计数“弱计数”
    // std::weak_ptr指向了某个控制块
    // std::weak_ptr没有被析构前，它指向的控制块就会一直存在，也就不会调用delete
    {
        std::weak_ptr<Helper> wp;
        {
            std::cout << "+++++\n";
            auto sp = std::make_shared<Helper>();
            wp      = sp;
            std::cout << "+++++\n";
        } // 这里并不会调用delete，即不会释放内存，但是会调用Helper的析构函数
        std::cout << wp.use_count() << '\n';
    }     // std::weak_ptr析构时才会调用delete释放内存
          // 所以如果std::shared_ptr的型别特别大且被std::weak_ptr弱引用，不建议使用std::make_shared

    std::cout << "----------------------------------------------------\n";
    return 0;
}

#endif // TEST403

#ifdef TEST404

#include <iostream>
#include <memory>

void* operator new(size_t size)
{
    std::cout << "new\n";
    return malloc(size);
}

void operator delete(void* p)
{
    std::cout << "delete\n";
    free(p);
}

class Throw
{
public:
    Throw()
    {
        throw std::runtime_error("throw");
    }
};

void FuncTest(std::shared_ptr<int>, Throw)
{
}

int main()
{
    std::cout << "-----------------------------------------------\n";
    {
        try
        {
            // 可能会存在内存泄漏，调用FuncTest之前会执行以下3个步骤：
            // 1. new int()
            // 2. 构造std::shared_ptr
            // 3. 构造Throw
            // 这三个步骤的顺序并不是固定的，有可能步骤3会在1和2之间进行，从而导致内存泄漏
            // 但是使用std::make_shared就不会出现这种情况
            FuncTest(std::shared_ptr<int>(new int()), Throw());
        }
        catch (...)
        {
            std::cout << "catch throw\n";
        }
    }
    std::cout << "-----------------------------------------------\n";
}

#endif // TEST404

#ifdef TEST405

#include <iostream>
#include <memory>

class A
{
public:
    A()
    {
        std::cout << "A()\n";
    }

    virtual ~A()
    {
        std::cout << "~A()\n";
    }

    virtual void Func()
    {
        std::cout << "A::Func()\n";
    }

    void Func_A()
    {
        std::cout << "A::Func_A()\n";
    }
};

class B : public A
{
public:
    B()
    {
        std::cout << "B()\n";
    }

    virtual ~B() override
    {
        std::cout << "~B()\n";
    }

    virtual void Func() override
    {
        std::cout << "B::Func()\n";
    }

    void Func_B()
    {
        std::cout << "B::Func_B()\n";
    }
};

int main()
{
    {
        std::unique_ptr<A> ptr {};
        ptr = std::make_unique<B>();
        ptr->Func();
    }

    std::cout << "--------------------------\n";

    {
        std::unique_ptr<A> ptr {};
        ptr = std::make_unique<B>();
        ptr->Func();

        std::unique_ptr<B> ptr_b {};
        ptr_b.reset(dynamic_cast<B*>(ptr.release()));
        if (ptr_b)
        {
            ptr_b->Func_B();
        }
    }

    std::cout << "--------------------------\n";

    {
        std::unique_ptr<A> ptr {};
        ptr = std::make_unique<B>();
        ptr->Func();

        std::unique_ptr<B> ptr_b = std::move(std::unique_ptr<B>(dynamic_cast<B*>(ptr.release())));

        if (ptr_b)
        {
            ptr_b->Func_B();
        }
    }

    std::cout << "--------------------------\n";

    {
        std::unique_ptr<A> ptr {};
        ptr = std::make_unique<A>();
        ptr->Func();

        auto p = ptr.release();

        // 此处不要使用 static_cast 应该使用 dynamic_cast
        // 多态更建议使用std::shared_ptr，转换指针时使用 dynamic_pointer_cast
        std::unique_ptr<B> ptr_b = std::move(std::unique_ptr<B>(dynamic_cast<B*>(p)));

        if (ptr_b)
        {
            ptr_b->Func_B();
        }
        else
        {
            // 需要保证ptr正确释放
            ptr = std::unique_ptr<A>(p);
        }
    }

    std::cout << "--------------------------\n";

    {
        std::shared_ptr<A> ptr {};
        ptr = std::make_shared<B>();

        auto ptr_b = std::dynamic_pointer_cast<B>(ptr);

        if (ptr_b)
        {
            ptr_b->Func_B();
        }
        if (ptr)
        {
            ptr->Func_A();
        }
    }

    std::cout << "--------------------------\n";

    {
        std::shared_ptr<A> ptr {};
        ptr = std::make_shared<A>();

        // 转换失败，ptr_b为空
        auto ptr_b = std::dynamic_pointer_cast<B>(ptr);

        if (ptr_b)
        {
            ptr_b->Func_B();
        }
        if (ptr)
        {
            ptr->Func_A();
        }
    }
}

#endif // TEST405