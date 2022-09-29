/*
1. atomic 多线程累加
2. atomic exchange
3. future promise获取函数返回值
4. 使用lambda表达式获取多线程函数的返回值
5. async（异步）的使用 async和thread类似，但有区别  https://www.jb51.net/article/198765.htm
6. async的使用，配合future直接获取多线程中所调用函数的返回值
7. async，future析构阻塞问题
8. packaged_task的使用，直接得到多线程调用函数的返回值
9. promise的使用，多线程中的函数所使用的参数需要其他线程返回。子线程使用主线程传入的值
10 promise的使用，多线程中的函数所使用的参数需要其他线程返回。主线程使用子线程得到的值
11.condition_variable条件变量的使用
12.线程调用成员函数
13.join阻塞演示
14.多线程求值
15.多线程读写文件，还有问题没修复
16.给线程执行的函数传参
17.线程池  dcax
18.
19.
20.
21.线程池
*/

#define TEST0


#ifdef TEST0

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

class Test
{
public:
    void Func1()
    {
        for (size_t i = 0; i < 10000; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            m_int1+=1;
        }
    }
    void Func2()
    {
        std::lock_guard<std::mutex> l(m_mutex);
        for (size_t i = 0; i < 10000; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            m_int2 += 1;
        }
    }
    void Func3()
    {
        for (size_t i = 0; i < 10000; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            m_int3 += 1;
        }
    }

    int Get1() { return m_int1; }
    int Get2() { return m_int2; }
    int Get3() { return m_int3; }

private:
    std::atomic_int m_int1{ 0 };
    int m_int2{ 0 };
    int m_int3{ 0 };

    std::mutex m_mutex;
};

int main()
{
    std::cout << "Test std::atomic\n";

    std::thread t1[10];
    std::thread t2[10];
    std::thread t3[10];

    Test test1, test2, test3;

    for (size_t i = 0; i < 10; ++i)
    {
        t1[i] = std::thread(&Test::Func1, &test1);
        t2[i] = std::thread(&Test::Func2, &test2);
        t3[i] = std::thread(&Test::Func3, &test3);
    }

    for (size_t i = 0; i < 10; ++i)
    {
        t1[i].join();
        t2[i].join();
        t3[i].join();
    }

    std::cout << test1.Get1() << '\t' << test1.Get2() << '\t' << test1.Get3() << std::endl;
    std::cout << test2.Get1() << '\t' << test2.Get2() << '\t' << test2.Get3() << std::endl;
    std::cout << test3.Get1() << '\t' << test3.Get2() << '\t' << test3.Get3() << std::endl;

    return 0;
}

#endif // TEST0


#ifdef TEST1

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>


class Test
{
public:
    void Set(int n)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        m_int = n;
    }

    int Get()
    {
        return m_int;
    }
private:
    std::atomic_int m_int;
};

int main()
{
    Test test;

    std::thread t[5];

    for (size_t i = 0; i < 5; ++i)
    {
        t[i] = std::thread(&Test::Set, test, 1);
    }

    for (size_t i = 0; i < 5; ++i)
    {
        t[i].join();
    }
}




#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<bool> ready(false);
std::atomic<bool> winner(false);

void count1m(int n)
{
    while (!ready) {} //wait for ready

    for (int i = 0; i < 1000000; i++) {} //count to 1M

    if (!winner.exchange(true)) { //只有第一个执行exchange的线程会返false,并输出下面的语句,其余线程都返回true无法进入if
        std::cout << "thread #" << n << " won!\n";
    }
}

int main()
{

    std::atomic_int num;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread(count1m, i + 1));//创建10个线程
    }
    ready.store(true);//开始
    for (auto& th : threads)
        th.join();
    return 0;
}
#endif // TEST2

#ifdef TEST3

#include<iostream>
#include<thread>
#include<mutex>
#include<atomic>
#include<future>  //std::future std::promise
#include<utility>  //std::ref模板传参的时候使用

void func2(int x, int y, std::promise<int>& promiseObj)
{
    promiseObj.set_value(x + y);
}

int main()
{
    //计算（a+b）/(x+y)
    //用三个线程，一个线程计算a+b，另一个线程计算x+y
    int a, b, x, y;
    a = 10, b = 8, x = 2, y = 4;

    int sum1, sum2;
    //声明一个类
    std::promise<int> promiseObj;
    //将future和promise关联
    std::future<int> futureObj = promiseObj.get_future();
    //模板传参的时候使用ref，否则传参失败
    std::thread t1(func2, a, b, ref(promiseObj));
    t1.join();
    //获取值
    sum1 = futureObj.get();
    std::cout << "sum1=" << sum1 << std::endl;

    //不能直接复用上面的future和promise
    std::promise<int> promiseObj2;
    std::future<int> futureObj2 = promiseObj2.get_future();

    std::thread t2(func2, x, y, ref(promiseObj2));
    t2.join();
    sum2 = futureObj2.get();
    std::cout << "sum2=" << sum2 << std::endl;

    std::cout << "sum1/sum2=" << sum1 / sum2 << std::endl;

    std::system("pause");
    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <thread>
#include <future>

int f(int a, int b)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
    return a + b;
}

int main()
{
    int result = 0;

    //std::thread* t;
    //t = new std::thread([&] { result = f(2, 3); });
    //t->join();

    std::future<int> retVal = std::async(f, 2, 4);
    std::cout << result << '\n';
    std::cout << retVal.get();  //执行到这行代码会阻塞
    std::cout << result << '\n';

    return 0;
}
#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <thread>
#include <future>
using namespace std::chrono_literals;
int fun(int a, int b)
{
    std::this_thread::sleep_for(1s);
    return a + b;
}

int main()
{
    std::cout << "1\n";
    std::promise<int> p1, p2;
    std::future<int> f1 = p1.get_future(), f2 = p2.get_future();
    std::cout << "2\n";
    p1.set_value(3);  //set_value必须在async之前
    p2.set_value(4);
    std::future<int> retVal = std::async(std::launch::async, fun, f1.get(), f2.get());
    std::cout << "3\n";

    std::cout << retVal.get() << std::endl;//会阻塞，即主线程需要子线程执行完从而得到返回值

    return 0;
}
#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <thread>
#include <future>

int f(int a, int b)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);  //睡眠五秒
    return a + b;
}

int main()
{
    std::future<int> retVal = std::async(f, 2, 4);
    std::cout << "start" << '\n';
    std::cout << retVal.get();  //会阻塞，即主线程需要子线程执行完从而得到返回值
    std::cout << "end" << '\n';


    auto sleep = [](int s) { std::this_thread::sleep_for(std::chrono::seconds(s)); };

    {
        std::async(std::launch::async, sleep, 5); // 临时对象被析构，阻塞 5s
        std::async(std::launch::async, sleep, 5); // 临时对象被析构，阻塞 5s

        //auto f1 = std::async( std::launch::async, sleep, 5 );
        //auto f2 = std::async( std::launch::async, sleep, 5 );
    }

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <thread>
#include <future>

int main()
{
    auto sleep = [](int s) { std::this_thread::sleep_for(std::chrono::seconds(s)); };
    clock_t start = clock();
    {
        std::async(std::launch::async, sleep, 5); // 临时对象被析构，阻塞 5s
        std::async(std::launch::async, sleep, 5); // 临时对象被析构，阻塞 5s

        //auto f1 = std::async( std::launch::async, sleep, 5 );
        //auto f2 = std::async( std::launch::async, sleep, 5 );
    }
    std::cout << (clock() - start) / CLOCKS_PER_SEC << std::endl;
    return 0;
}
#endif // TEST7

#ifdef TEST8

#include <iostream>     // std::cout
#include <utility>      // std::move
#include <future>       // std::packaged_task, std::future
#include <thread>       // std::thread

int fun(int a)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 2 * a;
}

int main()
{
    std::packaged_task<int(int)> foo(fun);
    std::future<int> ret = foo.get_future();

    std::thread t(std::move(foo), 19);
    t.join();  //阻塞，调用fun函数得到返回值

    std::cout << ret.get();

    return 0;
}
#endif // TEST8

#ifdef TEST9

#include <thread>
#include <future>
#include <iostream>

void task(/*std::future<int> i*/std::promise<int>& i)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << i.get_future().get();
    //std::cout << i.get() ; // 阻塞，直到 p.set_value() 被调用
}

int main()
{
    //lambda表达式和函数效果一样
    //auto task = [](std::future<int> i) 
    //{
    //    std::this_thread::sleep_for(std::chrono::seconds(3));
    //    std::cout << i.get() << std::flush; // 阻塞，直到 p.set_value() 被调用
    //};

    std::promise<int> p;
    //std::thread t(task, p.get_future());
    std::thread t(task, std::ref(p));

    p.set_value(5);
    t.join();  //调用完join后执行task()，因此在这里会阻塞

    return 0;
}
#endif // TEST9

#ifdef TEST10

#include <iostream>
#include <future>
#include <thread>

void task(std::promise<int>& i)
{
    //dosomething
    int value = 8;
    i.set_value(value);
}

int main()
{
    std::promise<int> pro;
    std::future<int> ret = pro.get_future();

    std::thread t(task, std::ref(pro));
    t.join();

    std::cout << "get value:" << ret.get() << std::endl;

    system("pause");
    return 0;
}
#endif // TEST10

#ifdef TEST11

#include <iostream>
#include <thread>
#include <list>
#include <mutex> //condition_variable mutex

//定义一个list，一个函数用来插入元素，一个函数用来取出元素
//取出元素的前提就是list不为空。因此list插入成功后就可以使用notify_one通知取出函数可以取元素了
//取出函数中的wait()在list为空的时候一直处于阻塞状态，

std::condition_variable cv;
std::list<int> myList;
std::mutex m;

void pushList()
{
    for (int i = 0; i < 1000; i++)
    {
        std::unique_lock<std::mutex> lock(m);  //这里的锁需要放在循环内部，不然所有push执行完之后才会pop

        std::cout << std::this_thread::get_id();
        std::cout << "push:" << i << std::endl;

        myList.emplace_back(i);
        cv.notify_one();
    }
}

void popList()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m);
        std::cout << std::this_thread::get_id();
        cv.wait(lock, []() {return !myList.empty(); });  //list为空的时候一直阻塞，即后面的代码不执行

        std::cout << "pop:" << myList.front() << std::endl;
        myList.pop_front();
        lock.unlock();//锁在循环内外不影响pop，但是如果死循环且unlock()在循环外部就不会执行unlobk()
    }
}


int main()
{
    std::thread t1(pushList);
    std::thread t2(popList);
    t1.join();
    t2.join();

    system("pause");
}
#endif // TEST11

#ifdef TEST12

#include <iostream>
#include <thread>

class A
{
public:
    void display(int a) { std::cout << a << '\n'; }
};

int main()
{
    A a;
    std::thread t(&A::display, a, 3);  //第一个参数必须带&，第二个可带可不带，第二个参数之后是调用函数的实参
    t.join();
}

#endif // TEST12

#ifdef TEST13

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

void work()
{
    this_thread::sleep_for(chrono::seconds(5));
}

int main()
{
    clock_t start = clock();
    vector<thread> t;
    for (int i = 0; i < 5; i++)
    {
        t.emplace_back(thread(work));
        //t[i].join();
    }
    for (int i = 0; i < 5; i++)
    {
        t[i].join();  //join会阻塞住，因此需要放在创建线程的循环外边才能达到多线程的效果
    }
    cout << clock() - start;
}

#endif // TEST13

#ifdef TEST14

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

using namespace std;

static const int MAX = 10e8;
static double sum = 0;
static mutex exclusive;

void concurrent_worker(int min, int max)
{
    double temp = 0;
    for (int i = min; i <= max; i++)
    {
        temp += sqrt(i);
    }
    exclusive.lock();
    sum += temp;
    exclusive.unlock();
}

void concurrent_task(int min, int max)
{
    auto start_time = chrono::steady_clock::now();
    unsigned concurrent_count = thread::hardware_concurrency();
    cout << "hardware_concurrency: " << concurrent_count << endl;
    vector<thread> threads;
    min = 0;
    sum = 0;
    for (int t = 0; t < concurrent_count; t++)
    {
        //对所求的数据范围进行分段，每一个线程得到一段数据的计算结果，最终加起来。
        int range = max / concurrent_count * (t + 1);
        threads.push_back(thread(concurrent_worker, min, range));
        min = range + 1;
    }

    for (int i = 0; i < threads.size(); i++)
    {   //join不能和创建线程放在一个循环内部，join会阻塞。
        threads[i].join();
    }

    auto end_time = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

int main()
{
    concurrent_task(sum, MAX);
}

#endif // TEST14

#ifdef TEST15

#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
using namespace std;


void thread_read_file(int tid, const string& file_path, string& str)
{
    ifstream file(file_path, ios::in);
    if (!file.good()) {
        stringstream ss;
        ss << "Thread " << tid << " failed to open file: " << file_path << "\n";
        cout << ss.str();
        return;
    }

    int pos;
    if (tid == 0) pos = 0;
    else pos = tid * 12;

    file.seekg(pos, ios::beg);
    string line;
    getline(file, line);
    stringstream ss;
    ss << "Thread " << tid << ", pos=" << pos << ": " << line << "\n";
    //cout << ss.str();
    str += ss.str();
}


void test_join(const string& file_path)
{
    vector<std::thread> vec_threads;
    string s;
    for (int i = 0; i < 8; ++i)
    {

        std::thread  th(thread_read_file, i, file_path, ref(s));
        vec_threads.emplace_back(std::move(th));

    }

    auto it = vec_threads.begin();
    for (; it != vec_threads.end(); ++it) {
        (*it).join();
    }
    cout << s;
}


int main()
{
    string file_path = "C:/Users/yangpan/Desktop/file.txt";

    test_join(file_path);

    return 0;
}

#endif // TEST15

#ifdef TEST16

#include <iostream>
#include <utility>
#include <thread>
#include <chrono>

void f1(int n)
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread 1 executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void f2(int& n)
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread 2 executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

class foo
{
public:
    void bar()
    {
        for (int i = 0; i < 5; ++i) {
            std::cout << "Thread 3 executing\n";
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    int n = 0;
};

class baz
{
public:
    void operator()()
    {
        for (int i = 0; i < 5; ++i) {
            std::cout << "Thread 4 executing\n";
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    int n = 0;
};

int main()
{
    int n = 0;
    foo f;
    baz b;
    std::thread t1; // t1 不是线程
    std::thread t2(f1, n + 1); // 按值传递
    std::thread t3(f2, std::ref(n)); // 按引用传递
    std::thread t4(std::move(t3)); // t4 现在运行 f2() 。 t3 不再是线程
    std::thread t5(&foo::bar, &f); // t5 在对象 f 上运行 foo::bar()
    std::thread t6(b); // t6 在对象 b 的副本上运行 baz::operator()
    t2.join();
    t4.join();
    t5.join();
    t6.join();
    std::cout << "Final value of n is " << n << '\n';
    std::cout << "Final value of f.n (foo::n) is " << f.n << '\n';
    std::cout << "Final value of b.n (baz::n) is " << b.n << '\n';


    int test = 0;
}
#endif // TEST16

#ifdef TEST17

//#include <functional>
//#include <type_traits> // std::is_invocable c++17 std::invoke_result_t
#include <future>
//#include <thread>
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
            std::lock(head_mutex, tail_mutex);
            std::lock_guard<std::mutex> headLk(head_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> tailLk(tail_mutex, std::adopt_lock);

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
            std::lock(head_mutex, tail_mutex);
            std::lock_guard<std::mutex> headLk(head_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> tailLk(head_mutex, std::adopt_lock);

            if (m_head.get() == m_tail)
                return nullptr;

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
}


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
    typename std::enable_if_t<std::is_invocable<Func>::value, std::future<typename std::invoke_result_t<Func>>>
        submit(Func f)
    {
        using result_type = typename std::invoke_result_t<Func>;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res = task.get_future();
        m_taskQueue.push(std::move(task));
        m_conVar.notify_one();

        return std::move(res);
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

void fun()
{
    std::cout << "submit 1\n";
}

void fun2()
{
    std::cout << "submit 2\n";
}

int main()
{

    auto pTheadPool = ThreadPool::getInstance();

    pTheadPool->submit(std::bind(fun));
    pTheadPool->submit(std::bind(fun2));

    std::cout << "good\n";
    return 0;
}

#endif // TEST17

#ifdef TEST18

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

using namespace std::chrono;

std::string fetchDataFromDB(std::string recvData) {
    //确保函数要5秒才能执行完成
    std::this_thread::sleep_for(seconds(5));

    //处理创建数据库连接、获取数据等事情
    return "DB_" + recvData;
}

std::string fetchDataFromFile(std::string recvData) {
    //确保函数要5秒才能执行完成
    std::this_thread::sleep_for(seconds(5));

    //处理获取文件数据
    return "File_" + recvData;
}

int main() {
    //获取开始时间
    system_clock::time_point start = system_clock::now();

    std::future<std::string> resultFromDB = std::async(std::launch::async, fetchDataFromDB, "Data");

    //从文件获取数据
    std::string fileData = fetchDataFromFile("Data");

    //从DB获取数据
    //数据在future<std::string>对象中可获取之前，将一直阻塞
    std::string dbData = resultFromDB.get();

    //获取结束时间
    auto end = system_clock::now();

    auto diff = duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "Total Time taken= " << diff << "Seconds" << std::endl;

    //组装数据
    std::string data = dbData + " :: " + fileData;

    //输出组装的数据
    std::cout << "Data = " << data << std::endl;

    return 0;
}

#endif // TEST18

#ifdef TEST19

#include <thread>
#include <iostream>
#include <chrono>


void fun(int time, int& num)  //参数为int&
{
    std::this_thread::sleep_for(std::chrono::seconds(time));
    num = 20;
}

int main()
{
    auto start = std::chrono::system_clock::now();
    int num1 = 0;
    int num2 = 0;
    std::thread t1(fun, 5, std::ref(num1));
    std::thread t2(fun, 10, std::ref(num2));

    t1.join();
    t2.join();

    std::cout << "\nnum:" << num1 + num2;
    auto end = std::chrono::system_clock::now();


    std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    return 0;
}
#endif // TEST19

#ifdef TEST20

#include <functional>
#include <future>
#include <iostream>
#include <thread>

using namespace std;

int func(int in)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return in * 2;
}

int main() {
    auto res = std::async(func, 5);

    std::future<int> ret = std::async(func, 6);
    //res.wait();
    cout << res.get() << endl; // 阻塞直到函数返回
    cout << ret.get() << endl;
    return 0;
}
#endif // TEST20

#ifdef TEST21

#include <iostream>
#include <vector>
#include <chrono>

#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        ->std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
            );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

// 测试线程池
int main()
{

    ThreadPool pool(4);
    std::vector< std::future<int> > results;

    for (int i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool.enqueue([i] {
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "world " << i << std::endl;
                return i * i;
                })
        );
    }

    for (auto&& result : results)
        std::cout << result.get() << ' ';

    std::cout << std::endl;

    return 0;
}

#endif // TEST21