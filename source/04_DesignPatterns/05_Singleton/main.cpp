/*
1. std::call_once 单例类
2. crtp 单例基类
*/


#define TEST1


#ifdef TEST1

#include <iostream>
#include <thread>
#include <mutex>

class Singleton
{
public:
    static Singleton* GetInstance()
    {
        static std::once_flag onceFlag;  // 此处onceFlag必须是静态的
        std::call_once(onceFlag, []()
            {
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
        std::call_once(onceFlag, []()
            {
                std::cout << "destroy Singleton\n";
                if (m_instance)
                {
                    delete m_instance;
                    m_instance = nullptr;
                }
            });
    }

    void Func()
    {
        std::cout << " === Func()\n";
    }
private:
    // 静态非const成员必须类外初始化
    static Singleton* m_instance;
};

Singleton* Singleton::m_instance = nullptr;

int main()
{
    // 多次使用GetInstance()，只会new一次
    Singleton::GetInstance()->Func();
    Singleton::GetInstance()->Func();

    Singleton::Destroy();

    if (!Singleton::GetInstance())
    {
        // 单例类成功析构
        std::cout << "Instance is nullptr\n";
    }

    // GetInstance()返回值为nullptr还可以访问成员函数，具体请看：00_CPP_TEST->07_Class->TEST9
    Singleton::GetInstance()->Func();
}

#endif // TEST1




