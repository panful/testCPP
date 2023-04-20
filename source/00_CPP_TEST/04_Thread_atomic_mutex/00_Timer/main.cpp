/*
 * 1. 自定义一个定时器
 * 2. std::promise实现定时器 查看 00_04_01_TEST18
 * 3. C++20 信号量
 */

#define TEST3

#ifdef TEST1

#include <chrono>
#include <condition_variable>
#include <functional>
#include <type_traits>

#include <iostream>

// 用来限制Timer的第一个参数必须是时间类型 std::chrono::duration
namespace std {
template <typename T, template <typename...> class U>
struct is_specialization_of : std::false_type
{
};

template <template <typename...> class U, typename... Ts>
struct is_specialization_of<U<Ts...>, U> : std::true_type
{
};

} // namespace std

template <typename T>
class Timer
{
public:
    Timer(T&& ti, std::function<void()>&& cb)
        : m_timeInterval(ti)
        , m_callBack(std::move(cb))
    {
        static_assert(std::is_specialization_of<T, std::chrono::duration>::value, "type is error");

        Run();
    }

private:
    void Run()
    {
        std::thread([*this]() {
            while (true)
            {
                std::this_thread::sleep_for(m_timeInterval);
                this->m_callBack();
            }
        }).detach();
    }

private:
    T m_timeInterval { 0 }; // 时间间隔
    std::function<void()> m_callBack;
};

int main()
{
    Timer timer(std::chrono::seconds(1), []() { std::cout << "timer\n"; });

    // 静态断言失败
    // Timer timer2(1, []() { std::cout << "timer\n"; });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}

#endif // TEST1

#ifdef TEST3

#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>
using namespace std::literals;

// 信号量（信号计数量）简单理解就是允许几个线程执行某个任务
// 互斥锁可以理解为信号量为1
 
// 全局二元信号量实例
// 设置对象计数为零
// 对象在未被发信状态
std::binary_semaphore smphSignal(0);
 
void ThreadProc()
{
    // 通过尝试减少信号量的计数等待来自主程序的信号
    smphSignal.acquire();
 
    // 此调用阻塞直至信号量的计数被从主程序增加
 
    std::cout << "[thread] Got the signal" << std::endl; // 回应消息
 
    // 等待 3 秒以模仿某种线程正在进行的工作
    std::this_thread::sleep_for(3s);
 
    std::cout << "[thread] Send the signal\n"; // 消息
 
    // 对主程序回复发信
    smphSignal.release();
}
 
int main()
{
    // 创建某个背景工作线程，它将长期存在
    std::jthread thrWorker(ThreadProc);
 
    std::cout << "[main] Send the signal\n"; // 消息
 
    // 通过增加信号量的计数对工作线程发信以开始工作
    smphSignal.release();
 
    // release() 后随 acquire() 可以阻止工作线程获取信号量，所以添加延迟：
    std::this_thread::sleep_for(50ms);
 
    // 通过试图减少信号量的计数等待直至工作线程完成工作
    smphSignal.acquire();
 
    std::cout << "[main] Got the signal\n"; // 回应消息
}

#endif // TEST3
