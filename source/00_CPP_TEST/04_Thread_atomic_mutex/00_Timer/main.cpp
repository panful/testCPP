// 自定义一个定时器

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
