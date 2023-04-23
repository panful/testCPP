/*
1. std::call_once 单例类
2. crtp 单例基类
*/

#define TEST1

#ifdef TEST1

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Singleton
{
public:
    static Singleton* GetInstance()
    {
        static std::once_flag onceFlag; // 此处onceFlag必须是静态的
        std::call_once(onceFlag, []() {
            if (m_instance == nullptr)
            {
                std::cout << "new Singleton\n";
                m_instance = new Singleton();
            }
        });
        return m_instance;
    }

    static void Destroy()
    {
        static std::once_flag onceFlag;
        std::call_once(onceFlag, []() {
            if (m_instance)
            {
                std::cout << "destroy Singleton\n";
                delete m_instance;
                m_instance = nullptr;
            }
        });
    }

    void Func()
    {
        static size_t n = 0;
        std::cout << " --- " << n++ << " Test: Func()\n";
    }

private:
    // 静态非const成员必须类外初始化（c++17之前）
    static Singleton* m_instance;
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    void operator=(const Singleton&) = delete;

    // c++17开始，可以使用inline在类内部初始化静态成员
    // inline static Singleton* m_instance { nullptr };
};

Singleton* Singleton::m_instance = nullptr;

int main()
{
    std::cout << "--- start ---\n";

    std::vector<std::thread> threads;

    constexpr size_t numOfThreads { 1000 };
    for (size_t i = 0; i < numOfThreads; i++)
    {
        // new Singleton()只会被调用一次
        std::thread t([]() { Singleton::GetInstance(); });
        threads.emplace_back(std::move(t));
    }

    for (auto&& t:threads)
    {
        t.join();
    }

    std::cout << "--- end ---\n";

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <mutex>

// crtp https://mp.weixin.qq.com/s/giSDMWLO0d7t3fX-ZC7eVg

template <typename T>
class SingletonBase
{
public:
    static T* GetInstance() noexcept
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            m_instance = new T();
        });
        return m_instance;
    }
    static void Destroy() noexcept
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            if (m_instance)
            {
                delete m_instance;
                m_instance = nullptr;
            }
        });
    };

private:
    inline static T* m_instance { nullptr };

protected:
    SingletonBase() noexcept = default;
    virtual ~SingletonBase() noexcept = default;
    void operator=(const SingletonBase&) noexcept = delete;
    SingletonBase(const SingletonBase&) noexcept = delete;

    // 不需要实现参数为右值引用的拷贝构造函数和operator=
    // void operator=(SingletonBase&&) = delete;
    // SingletonBase(SingletonBase&&) = delete;
};

class Derived final: public SingletonBase<Derived>
{
    friend class SingletonBase<Derived>;

public:
    void Test()
    {
        std::cout << m_number << '\n';
    }

private:
    int m_number { 666 };

private:
    Derived() noexcept = default;
    // 同时使用override和noexcept的时候，要保证noexcept在前。
    ~Derived() noexcept override = default;

    // 拷贝构造和operator=(const SingletonBase&)都会被继承
};

int main()
{
    Derived::GetInstance()->Test();
    Derived::Destroy();

    return 0;
}

#endif // TEST2
