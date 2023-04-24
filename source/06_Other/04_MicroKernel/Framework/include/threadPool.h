

//#include <functional>
#include <type_traits> // std::is_invocable c++17 std::invoke_result_t
#include <future>
#include <vector>
//#include <atomic>
//#include <queue>

#include <iostream>

namespace common
{
    // 线程安全队列
    template<typename T>
    class queue
    {
#ifndef LOCK_FREE

    public:
        constexpr static bool is_lock_free()
        {
            return false;
        }

        queue() :m_head(std::make_unique<Node>()),
            m_tail(m_head.get())
        {}
        queue(const queue&) = delete;
        queue& operator = (const queue&) = delete;
        ~queue()
        {}

        void push(T newValue)
        {
            auto new_data = std::make_shared<T>(std::move(newValue));

            std::unique_ptr<Node> newNode = std::make_unique<Node>();
            {
                std::lock_guard<std::mutex> lk(tail_mutex);
                m_tail->data.swap(new_data);
                auto newTail = newNode.get();
                m_tail->next.swap(newNode);
                m_tail = newTail;
            }
        }


        std::shared_ptr<T> pop()
        {
            auto oldHead = popHead();
            return oldHead ? oldHead->data : nullptr;
        }

        bool empty() const
        {
            //std::lock(head_mutex, tail_mutex);
            //std::lock_guard<std::mutex> headLk(head_mutex, std::adopt_lock);
            //std::lock_guard<std::mutex> tailLk(tail_mutex, std::adopt_lock);

            return m_head.get() == m_tail;
        }

    private:
        struct Node
        {
            std::shared_ptr<T> data;
            std::unique_ptr<Node> next;
        };

        std::unique_ptr<Node> popHead()
        {
            // MSVC这里会报错：c26110:unlock of unowned mutex
            // https://www.jb51.net/article/241314.htm
            //std::lock(head_mutex, tail_mutex);
            //std::lock_guard<std::mutex> headLk(head_mutex, std::adopt_lock);
            //std::lock_guard<std::mutex> tailLk(head_mutex, std::adopt_lock);

            //if (m_head.get() == m_tail)
            //    return nullptr;

            auto oldHead = std::move(m_head);
            m_head = std::move(oldHead->next);
            return oldHead;
        }

        mutable std::mutex head_mutex;
        mutable std::mutex tail_mutex;
        std::unique_ptr<Node> m_head;
        Node* m_tail;
#else
    public:
        constexpr static bool is_lock_free()
        {
            return true;
        }
#endif
    };
    
class ThreadPool
{
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;
    ~ThreadPool();

    static ThreadPool* getInstance();
    static void destroy();

    // 提交任务
    template<typename Func>
    typename std::enable_if_t<std::is_invocable<Func>::value,
        std::future<typename std::invoke_result_t<Func>>>
        submit(Func f)
    {
        using result_type = typename std::invoke_result_t<Func>;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res = task.get_future();
        m_taskQueue.push(std::move(task));
        m_conVar.notify_one();

        //return std::move(res);
        return res;
    }
private:
    class TaskWrapper
    {

    public:
        template<typename Func>
        TaskWrapper(Func&& f) :
            m_imp(new impl_type<Func>(std::move(f)))
        {}

        TaskWrapper() = default;
        TaskWrapper(const TaskWrapper&) = delete;
        TaskWrapper& operator=(const TaskWrapper&) = delete;
        TaskWrapper(TaskWrapper&& other) noexcept :
            m_imp(std::move(other.m_imp))
        {}
        TaskWrapper& operator=(TaskWrapper&& other) noexcept
        {
            m_imp.swap(other.m_imp);
            return*this;
        }

        void operator()()
        {
            m_imp->call();
        }

    private:
        struct impl_base
        {
            virtual void call() = 0;
            virtual ~impl_base() {}
        };

        template<typename Func>
        struct impl_type :public impl_base
        {
            impl_type(Func&& f) :
                m_functor(std::move(f))
            {}
            void call() override
            {
                m_functor();
            }
            Func m_functor;
        };
    private:
        std::unique_ptr<impl_base> m_imp;
    };

    void execute();
    ThreadPool();

private:
    static std::once_flag m_inited;
    static ThreadPool* m_pInstance;

    std::atomic_bool m_done;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_conVar;
    common::queue<TaskWrapper> m_taskQueue;
};

constexpr int CORE_NUM = 20;

std::once_flag ThreadPool::m_inited;
ThreadPool* ThreadPool::m_pInstance = nullptr;

ThreadPool* ThreadPool::getInstance()
{
    std::call_once(m_inited, []() {
        m_pInstance = new ThreadPool;
        });

    return m_pInstance;
}

void ThreadPool::destroy()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void ThreadPool::execute()
{
    // 工作线程，从队列中获取任务然后执行
    std::shared_ptr<TaskWrapper> task;

    while (!m_done)
    {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_conVar.wait(lk, [this]() {return m_done || !m_taskQueue.empty(); });
            task = m_taskQueue.pop();
        }
        if (task)
            (*task)();
    }
}

ThreadPool::ThreadPool() :
    m_done(false)
{
    // 根据硬件资源启动一定数量线程放入集合中
    const auto core = std::thread::hardware_concurrency() == 0 ? CORE_NUM : std::thread::hardware_concurrency();

    try
    {
        for (auto i = 0U; i < core; i++)
        {
            m_threads.emplace_back(&ThreadPool::execute, this);
        }
    }
    catch (...)
    {
        m_done = true;
        m_conVar.notify_all();
        throw;
    }
}

ThreadPool::~ThreadPool()
{
    m_done = true;
    m_conVar.notify_all();
    for (auto& t : m_threads)
    {
        if (t.joinable())
            t.join();
    }
}
}




//void fun()
//{
//    std::cout << "submit 1\n";
//}
//
//void fun2()
//{
//    std::cout << "submit 2\n";
//}
//
//int main()
//{
//
//    auto pTheadPool = ThreadPool::getInstance();
//
//    pTheadPool->submit(std::bind(fun));
//    pTheadPool->submit(std::bind(fun2));
//
//    std::cout << "good\n";
//    return 0;
//}
