/*
1. atomic 多线程累加
2. atomic exchange
3. future promise获取函数返回值
4. 使用lambda表达式获取多线程函数的返回值
5. std::async（异步）的使用 async和thread类似，但有区别  https://www.jb51.net/article/198765.htm
6. std::async的使用，配合future直接获取多线程中所调用函数的返回值，std::async返回对象析构阻塞，std::future.get()也会阻塞
7. std::future std::async
8. packaged_task的使用，直接得到多线程调用函数的返回值
9. promise的使用，多线程中的函数所使用的参数需要其他线程返回。子线程使用主线程传入的值
10 promise的使用，多线程中的函数所使用的参数需要其他线程返回。主线程使用子线程得到的值
11.condition_variable条件变量的使用
12.线程调用成员函数
13.join阻塞演示
14.多线程求值
15.多线程读写文件，还有问题没修复
16.给线程执行的函数传参
17.
18.
19.
20.
21.线程池
22.pthread
*/

/*
* 201 std::mutex 互斥锁
* 202 std::shared_lock 共享锁
* 203 std::timed_mutex 超时锁
* 204 std::recursive_mutex 递归锁
*/

#define TEST201


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

auto start = std::chrono::high_resolution_clock::now();
#define PRINT_NOW std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>\
((std::chrono::high_resolution_clock::now() - start)).count() << "ms\n";

int fun(int a, int b)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3s);
    return a + b;
}

int main()
{
    PRINT_NOW
    std::promise<int> p1, p2;
    std::future<int> f1 = p1.get_future(), f2 = p2.get_future();
    PRINT_NOW
    p1.set_value(3);  //set_value必须在async之前
    p2.set_value(4);
    std::future<int> retVal = std::async(std::launch::async, fun, f1.get(), f2.get());
    PRINT_NOW
    std::cout << retVal.get() << std::endl;//会阻塞，即主线程需要子线程执行完从而得到返回值
    PRINT_NOW

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

#define NOW std::chrono::high_resolution_clock::now();
#define TOOK(a,b) std::cout << "took " << std::chrono::duration_cast<std::chrono::seconds>(a - b).count() << "s\n";

int main()
{
    auto time1 = NOW;
    std::future<int> retVal = std::async(f, 2, 4);
    auto time2 = NOW;
    std::cout << "result = " << retVal.get() << '\n';  // 会阻塞，即主线程需要子线程执行完从而得到返回值
    auto time3 = NOW;
    //-----------------------------------------------------
    auto sleep = [](int s) { std::this_thread::sleep_for(std::chrono::seconds(s)); };
    auto time4 = NOW;
    {
        std::async(std::launch::async, sleep, 5); // 返回值没有被接收，临时对象被析构，阻塞 5s
        std::async(std::launch::async, sleep, 5); // 阻塞 5s

        auto f1 = std::async( std::launch::async, sleep, 5 ); // 返回值被接收，所以不会析构，这句代码也就不会阻塞，{}结束的时候才会析构f1，那个时候才会阻塞
        auto f2 = std::async( std::launch::async, sleep, 5 ); // f1,f2,f3并行析构，这三句代码析构共总只需要5s
        auto f3 = std::async( std::launch::async, sleep, 5 ); 
    }
    auto time5 = NOW;

    TOOK(time2, time1);
    TOOK(time3, time2);
    TOOK(time4, time3);
    TOOK(time5, time4);

    return 0;
}

#endif // TEST6

#ifdef TEST7

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
    //数据在future<std::string>对象中可获取之前，将一直【阻塞】
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







#ifdef TEST20

#include <functional>
#include <future>
#include <iostream>
#include <thread>

int func(int in)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return in * 2;
}

int main() {
    auto res = std::async(func, 5);

    std::future<int> ret = std::async(func, 6);
    //res.wait();
    std::cout << res.get() << std::endl; // 阻塞直到函数返回
    std::cout << ret.get() << std::endl;
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

#ifdef TEST22

#ifdef __GNUC__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX 50
// 全局变量
int number;

// 线程处理函数
void* funcA_num(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        int cur = number;
        cur++;
        usleep(10);
        number = cur;
        printf("Thread A, id = %lu, number = %d\n", pthread_self(), number);
    }

    return NULL;
}

void* funcB_num(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        int cur = number;
        cur++;
        number = cur;
        printf("Thread B, id = %lu, number = %d\n", pthread_self(), number);
        usleep(5);
    }

    return NULL;
}
# endif

int main(int argc, const char* argv[])
{
#ifdef __GNUC__
    pthread_t p1, p2;

    // 创建两个子线程
    pthread_create(&p1, NULL, funcA_num, NULL);
    pthread_create(&p2, NULL, funcB_num, NULL);

    // 阻塞，资源回收
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
#endif
    return 0;
}


// 作者: 苏丙榅
// 链接: https://subingwen.cn/linux/thread-sync/
// 来源: 爱编程的大丙
// 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
#endif

// -------------------------------------------

#ifdef TEST201

int main()
{
}

#endif // TEST201

#ifdef TEST202

/*
一个或多个读线程同时读取共享资源，且只有一个写线程来修改这个资源，这种情况下才能从shared_mutex获取性能优势
shared_lock是read lock。搭配std::shared_mutex使用，被锁后仍允许其他线程执行同样被shared_lock的代码。
lock_guard和unique_lock是write lock。被锁后不允许其他线程执行被shared_lock或unique_lock的代码。
https://www.jianshu.com/p/86abf93a05e7/
*/

/*
当有一个全局变量被多个线程高频调用读写时需要锁功能
1.当变量没有写入需求时可以支持多个线程同时读取(所有读取线程公用一个共享锁)
2.当有一个线程有写入需求时, 需要先等所有读取锁释放锁资源之后锁上互斥锁.此时不允许读取了, 需要等带写入的互斥锁解锁后才能读取
*/

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <list>
#include <iostream>
#include <vector>

#define READ_THREAD_COUNT 8  
#define LOOP_COUNT 5000000  

typedef std::shared_lock<std::shared_mutex> ReadLock;
typedef std::lock_guard<std::shared_mutex> WriteLock;

typedef std::lock_guard<std::mutex> NormalLock;

class shared_mutex_counter {
public:
    shared_mutex_counter() = default;

    unsigned int get() const {
        ReadLock lock(mutex);
        return value;
    }

    void increment() {
        WriteLock lock(mutex);
        value++;
    }

private:
    mutable std::shared_mutex mutex;
    unsigned int value = 0;
};

class mutex_counter {
public:
    mutex_counter() = default;

    unsigned int get() const {
        NormalLock lock(mutex);
        return value;
    }

    void increment() {
        NormalLock lock(mutex);
        value++;
    }

private:
    mutable std::mutex mutex;
    unsigned int value = 0;
};

class timers
{
public:
    timers()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    ~timers()
    {
        m_end = std::chrono::high_resolution_clock::now();
        Consuming();
    }

    void Consuming()
    {
        std::cout << "Time-consuming:" << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_end - m_begin).count() << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point m_begin;
    std::chrono::high_resolution_clock::time_point m_end;
};


void test_shared_mutex()
{
    shared_mutex_counter counter;
    unsigned int temp;

    auto writer = [&counter]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    std::cout << "----- shared mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        timers timer;

        for (int i = 0; i < READ_THREAD_COUNT; i++)
        {
            threadlist.push_back(std::make_shared<std::thread>(reader));
        }
        std::shared_ptr<std::thread> pw = std::make_shared<std::thread>(writer);

        for (auto& it : threadlist)
        {
            it->join();
        }
        pw->join();
    }
    // 因为shared_lock允许多个线程同时进行【读操作】，所以temp的值不一定是LOOP_COUNT
    std::cout << "count:" << counter.get() << ", temp:" << temp << std::endl;
}

void test_mutex()
{
    mutex_counter counter;
    unsigned int temp;

    auto writer = [&counter]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    auto reader = [&counter, &temp]() {
        for (unsigned int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    std::cout << "----- mutex test ------" << std::endl;
    std::list<std::shared_ptr<std::thread>> threadlist;
    {
        timers timer;

        for (int i = 0; i < READ_THREAD_COUNT; i++)
        {
            threadlist.push_back(std::make_shared<std::thread>(reader));
        }

        std::shared_ptr<std::thread> pw = std::make_shared<std::thread>(writer);

        for (auto& it : threadlist)
        {
            it->join();
        }
        pw->join();
    }
    std::cout << "count:" << counter.get() << ", temp:" << temp << std::endl;
}



int main()
{
    test_shared_mutex();
    test_mutex();
    return 0;
}

#endif // TEST202

#ifdef TEST203

// 尝试获取锁的时间过长时就会触发超时, 超时的情况下不让线程继续阻塞

#include <mutex>
#include <iostream>
#include <chrono>

int i = 0;
std::timed_mutex m;

using namespace std::chrono_literals;

void test() {
    for (;;)
    {
        if (!m.try_lock_for(std::chrono::milliseconds(100))) {//超时时间为100ms
            std::cout << "尝试上锁 已超过100ms 仍未成功 触发超时" << std::endl;
            continue;
        }
        i = 10;
        std::cout << "成功上锁" << std::endl;
        std::this_thread::sleep_for(1s);//阻塞一下测试超时锁效果
        m.unlock();
        std::this_thread::sleep_for(1ms);
    }
}
int main()
{
    for (int i = 0; i < 3; i++)
    {
        std::thread t1(test);
        t1.detach();
    }

    while (1);
}

#endif // TEST203

#ifdef TEST204

// 同线程同一把锁可以上锁多次, 避免死锁
// lock()一次锁计数器加一, unlock()一次锁计数器减一
// 当锁计数器归零时才会真正解锁

#include <iostream>
#include <mutex>

using namespace std;

recursive_mutex rm;

void test()
{
    rm.lock();
    cout << "Do something" << endl;
    rm.unlock();
}

int main()
{
    for (int i = 0; i < 3; i++)
    {
        rm.lock();
        test();
        rm.unlock();//锁几次就要解锁几次
        this_thread::sleep_for(1ms);
    }
    
}
#endif // TEST204
