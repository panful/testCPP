
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace MyThreadPool {
// 线程池最大容量,应尽量设小一点
#define THREADPOOL_MAX_NUM 16
// #define  THREADPOOL_AUTO_GROW

// 线程池,可以提交变参函数或拉姆达表达式的匿名函数执行,可以获取执行返回值
// 不直接支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等
class threadpool
{
    using Task = std::function<void()>; // 定义类型
    std::vector<std::thread> m_pool;    // 线程池
    std::queue<Task> m_tasks;           // 任务队列
    std::mutex m_lock;                  // 同步
    std::condition_variable m_task_cv;  // 条件阻塞
    std::atomic<bool> m_run { true };   // 线程池是否执行
    std::atomic<int> m_idlThrNum { 0 }; // 空闲线程数量

public:
    inline threadpool(unsigned short size = 4) { addThread(size); }
    inline ~threadpool()
    {
        m_run = false;
        m_task_cv.notify_all(); // 唤醒所有线程执行
        for (std::thread& thread : m_pool)
        {
            // thread.detach(); // 让线程“自生自灭”
            if (thread.joinable())
            {
                thread.join(); // 等待任务结束， 前提：线程一定会执行完
            }
        }
    }

public:
    // 提交一个任务
    // 调用.get()获取返回值会等待任务执行完,获取返回值
    // 有两种方法可以实现调用类成员，
    // 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
    // 一种是用   mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
    template <class F, class... Args>
    auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        if (!m_run) // stoped ??
            throw std::runtime_error("commit on ThreadPool is stopped.");

        using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)); // 把函数入口及参数,打包(绑定)
        std::future<RetType> future = task->get_future();
        {                                                // 添加任务到队列
            std::lock_guard<std::mutex> lock { m_lock }; // 对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
            m_tasks.emplace([task]() {                   // push(Task{...}) 放到队列后面
                (*task)();
            });
        }
#ifdef THREADPOOL_AUTO_GROW
        if (_idlThrNum < 1 && m_pool.size() < THREADPOOL_MAX_NUM)
            addThread(1);
#endif                          // !THREADPOOL_AUTO_GROW
        m_task_cv.notify_one(); // 唤醒一个线程执行

        return future;
    }

    // 空闲线程数量
    int idlCount() { return m_idlThrNum; }
    // 线程数量
    auto thrCount() { return m_pool.size(); }
#ifndef THREADPOOL_AUTO_GROW
private:
#endif // !THREADPOOL_AUTO_GROW
       // 添加指定数量的线程
    void addThread(unsigned short size)
    {
        for (; m_pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
        {                                // 增加线程数量,但不超过 预定义数量 THREADPOOL_MAX_NUM
            m_pool.emplace_back([this] { // 工作线程函数
                while (m_run)
                {
                    Task task; // 获取一个待执行的 task
                    {
                        // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                        std::unique_lock<std::mutex> lock { m_lock };
                        m_task_cv.wait(lock, [this] {
                            return !m_run || !m_tasks.empty();
                        }); // wait 直到有 task
                        if (!m_run && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front()); // 按先进先出从队列取一个 task
                        m_tasks.pop();
                    }
                    m_idlThrNum--;
                    task(); // 执行任务
                    m_idlThrNum++;
                }
            });
            m_idlThrNum++;
        }
    }
};

} // namespace MyThreadPool

//----------------------------------------------------------------------------------------------------
void MyFunc(int slp)
{
    std::cout << "MyFunc\tthread id: " << std::this_thread::get_id() << '\n';
    if (slp > 0)
    {
        std::cout << "MyFunc\tthread id: " << std::this_thread::get_id() << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(slp));
    }
}

struct MyStruct
{
    int operator()(int n)
    {
        std::cout << "MyStruct\tthread id: " << std::this_thread::get_id() << '\n';
        return 1;
    }
};

// 函数必须是 static 的才能使用线程池
class MyClass
{
public:
    static int Afun(int n = 0)
    {
        std::cout << n << "MyClass\tthread id: " << std::this_thread::get_id() << std::endl;
        return n;
    }

    static std::string Bfun(int n, std::string str, char c)
    {
        std::cout << n << "MyClass\tthread id: " << str.c_str() << "  " << (int)c << "  " << std::this_thread::get_id() << std::endl;
        return str;
    }
};

int main()
{
    try
    {
        MyThreadPool::threadpool executor { 50 };
        MyClass a;
        std::future<void> ff = executor.commit(MyFunc, 0);
        std::future<int> fg = executor.commit(MyStruct {}, 0);
        std::future<int> gg = executor.commit(a.Afun, 9999);
        std::future<std::string> gh = executor.commit(MyClass::Bfun, 9998, "mult args", 123);
        std::future<std::string> fh = executor.commit([]() -> std::string { std::cout << "hello, fh !  " << std::this_thread::get_id() << std::endl; return "hello,fh ret !"; });

        std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(900));

        for (int i = 0; i < 50; i++)
        {
            executor.commit(MyFunc, i * 100);
        }
        std::cout << " =======  commit all ========= " << std::this_thread::get_id() << " idlsize=" << executor.idlCount() << std::endl;

        std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        ff.get(); // 调用.get()获取返回值会等待线程执行完,获取返回值
        std::cout << fg.get() << "  " << fh.get().c_str() << "  " << std::this_thread::get_id() << std::endl;

        std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << " =======  fun1,55 ========= " << std::this_thread::get_id() << std::endl;
        executor.commit(MyFunc, 55).get(); // 调用.get()获取返回值会等待线程执行完

        std::cout << "end... " << std::this_thread::get_id() << std::endl;

        MyThreadPool::threadpool pool(4);
        std::vector<std::future<int>> results;

        for (int i = 0; i < 8; ++i)
        {
            results.emplace_back(
                pool.commit([i] {
                    std::cout << "hello " << i << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "world " << i << std::endl;
                    return i * i;
                }));
        }
        std::cout << " =======  commit all2 ========= " << std::this_thread::get_id() << std::endl;

        for (auto&& result : results)
            std::cout << result.get() << ' ';
        std::cout << std::endl;
        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "some unhappy happened...  " << std::this_thread::get_id() << e.what() << std::endl;
    }
}