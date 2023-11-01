/**
 * 1. boost版本 boost::array
 * 2. boost::process 启动进程杀掉进程
 * 3. sml State Machine Libray 状态机 https://github.com/boost-ext/sml
 * 4. mpl Meta Programming 元编程
 * 5. boost::lexical_cast 数值与字符串转换，c++流也可以，不过没有错误检查
 * 6. boost::pfr 反射
 * 7. boost::type_index
 * 8. boost::coroutines2 协程
 */

#define TEST8

#ifdef TEST1

#include <boost/array.hpp>
#include <boost/version.hpp>
#include <iostream>

int main()
{
    // boost版本
    std::cout << "Using Boost " << BOOST_VERSION / 100000 << "." // major version
              << BOOST_VERSION / 100 % 1000 << "."               // minor version
              << BOOST_VERSION % 100                             // patch level
              << std::endl;

    std::cout << "-------------------\n";

    // boost::array测试
    boost::array<int, 2> test { 11, 22 };
    std::cout << "test boost::array\t" << test.at(0) << ',' << test[1] << '\n';

    return 0;
}

#endif // TEST1

#ifdef TEST2

// https://blog.csdn.net/stallion5632/article/details/126200625

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <chrono>
#include <stdint.h>
#include <string>
#include <thread>
#if defined(BOOST_WINDOWS_API)
#include <TlHelp32.h>
#include <boost/locale.hpp>

#elif defined(BOOST_POSIX_API)
#include <signal.h>
#include <sys/types.h>
#endif

namespace ProcessHelper {
static uint64_t getTimeStamp()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    return ms.count();
}

static uint32_t getCurrentProcessId()
{
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return ::getpid();
#endif
}

static std::string getProcessRunningTime(uint64_t startTimeStamp) // return minutes
{
    auto now  = getTimeStamp();
    auto diff = now - startTimeStamp;
    auto min  = double(diff) / (1000 * 60);
    return std::to_string(min);
}

#if defined(BOOST_WINDOWS_API)
static int isRunning(int pid)
{
    HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

    PROCESSENTRY32 pe = { 0 };
    pe.dwSize         = sizeof(pe);

    if (Process32First(pss, &pe))
    {
        do
        {
            // pe.szExeFile can also be useful
            if (pe.th32ProcessID == pid)
                return 0;
        } while (Process32Next(pss, &pe));
    }

    CloseHandle(pss);

    return -1;
}

#elif defined(BOOST_POSIX_API)

// 0 : running, -1 : exit, -2 : zombie
static int isRunning(int pid)
{
    if (0 == kill(pid, 0))
    {
        std::string path = std::string("/proc/") + std::to_string(pid) + "/status";
        std::ifstream fs;
        fs.open(path);
        if (!fs)
            return -1;

        std::string line;
        while (getline(fs, line))
        {
            std::size_t found = line.find("State:");
            if (found == std::string::npos)
                continue;
            else
            {
                found = line.find("zombie");
                if (found == std::string::npos)
                    return 0;
                else
                    return -2; // zombie
            }
        }
    }
    else
        return -1;
}
#endif

static std::tuple<bool, std::string> killChildProcess(int pid)
{
    std::string err;
    bool ret = false;
    try
    {
        auto id = boost::process::pid_t(pid);
        boost::process::child pros(id);
        std::error_code ec;
        pros.terminate(ec);

        if (ec.value() == 0)
            return std::make_tuple(true, err);
    }
    catch (boost::process::process_error& exc)
    {
        err = exc.what();
    }
    return std::make_tuple(false, err);
}

static std::tuple<bool, std::string> killProcess(int pid, bool isChild = true)
{
    std::string err;
    bool ret = false;
#if defined(BOOST_WINDOWS_API)
    return killChildProcess(pid);
#elif defined(BOOST_POSIX_API)
    if (isChild)
        return killChildProcess(pid);
    else // if not a child process,will not kill the process correctly
    {
        std::string cmd("kill -9 ");
        cmd += std::to_string(pid);
        auto ret = boost::process::system(cmd);
        if (ret == 0)
            return std::make_tuple(true, err);
    }
    return std::make_tuple(false, err);
#endif
}

static std::tuple<std::vector<int>, std::string> getAllProcessPidsByName(const std::string& processName)
{
    std::vector<int> pids;
    std::string err;
    try
    {
#if defined(BOOST_WINDOWS_API)

        std::wstring wName;

        std::string exe(".exe");

        wName = boost::locale::conv::to_utf<wchar_t>(processName + exe, "GBK");

        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32W entry;
        entry.dwSize = sizeof entry;
        if (!Process32FirstW(hSnapShot, &entry))
        {
            return std::make_tuple(std::move(pids), err);
        }
        do
        {
            if (std::wstring(entry.szExeFile) == wName)
            {
                pids.emplace_back(entry.th32ProcessID);
            }
        } while (Process32NextW(hSnapShot, &entry));

#elif defined(BOOST_POSIX_API)
        boost::filesystem::path path = "/proc";
        boost::filesystem::directory_iterator end;

        for (boost::filesystem::directory_iterator iter(path); iter != end; iter++)
        {
            boost::filesystem::path p = *iter;
            std::ifstream statusFile;
            statusFile.open(p.string() + std::string("/status"));
            if (!statusFile)
                continue;

            std::string statusContent;
            getline(statusFile, statusContent);
            std::vector<std::string> a;
            boost::algorithm::split(a, statusContent, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);

            if (boost::algorithm::trim_copy(a[1]) == processName)
            {
                pids.push_back(std::stoi(p.leaf().string()));
            }
            statusFile.close();
            statusFile.clear();
        }
#endif
    }
    catch (boost::process::process_error& exc)
    {
        err = exc.what();
    }
    return std::make_tuple(std::move(pids), err);
}

static std::tuple<int, std::string> startProcess(const std::string& processName, const std::string& processArgv)
{
    int pid = -1;
    std::string err;

    try
    {
#if defined(BOOST_WINDOWS_API)
        auto p = processName + ".exe";
        if (!boost::filesystem::exists(p))
        {
            p = boost::filesystem::current_path().string() + "/" + p;
            if (!boost::filesystem::exists(p))
            {
                err = "process not exist";
                return std::make_tuple(pid, err);
            }
        }
        boost::process::child c(
            p, processArgv, boost::process::extend::on_setup = [](auto& exec) { exec.creation_flags |= boost::winapi::CREATE_NEW_CONSOLE_; });

#elif defined(BOOST_POSIX_API)

        auto p = processName;
        p      = boost::filesystem::current_path().string() + "/" + p;
        if (!boost::filesystem::exists(p))
        {
            err = "process not exist";
            return std::make_tuple(pid, err);
        }
        p = p + " " + processArgv;
        boost::process::child c(p);
#endif
        pid = c.id();
        // detach as a single process
        c.detach();
    }
    catch (boost::process::process_error& exc)
    {
        err = exc.what();
        pid = -1;
    }
    return std::make_tuple(pid, err);
}
} // namespace ProcessHelper

// using namespace ProcessHelper;
int main()
{
    std::string processArgv = "test.txt";
    std::string processName = "00_01_test";
    // std::string processName = "notepad";

    // 启动进程
    auto newtup = ProcessHelper::startProcess(processName, processArgv);
    auto pid    = std::get<0>(newtup);

    // 查询进程是否在运行
    if (0 != ProcessHelper::isRunning(pid))
    {
        std::cerr << "process " << pid << " is not running!" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "process " << pid << " is running!" << std::endl;
    }

    // 根据进程名查询所有的pids
    auto tup  = ProcessHelper::getAllProcessPidsByName(processName);
    auto pids = std::get<0>(tup);

    // 杀掉所有的同名进程
    for (auto pid : pids)
    {
        // std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "killing " << pid << std::endl;
        ProcessHelper::killProcess(pid, false);
        if (0 != ProcessHelper::isRunning(pid))
            std::cout << "process " << pid << " killed!" << std::endl;
    }
    return 0;
}

#endif // TEST2

#ifdef TEST3

// https ://zhuanlan.zhihu.com/p/430383570
// https://zhuanlan.zhihu.com/p/26732643

#include <boost/sml.hpp>
#include <cassert>

namespace sml = boost::sml;

struct release
{
};

struct ack
{
};

struct fin
{
};

struct timeout
{
};

const auto is_ack_valid = [](const ack&) { return true; };
const auto is_fin_valid = [](const fin&) { return true; };

const auto send_fin = [] {};
const auto send_ack = [] {};

class established;
class fin_wait_1;
class fin_wait_2;
class timed_wait;

struct hello_world
{
    auto operator()() const
    {
        using namespace sml;
        return make_transition_table(*state<established> + event<release> / send_fin = state<fin_wait_1>,
            state<fin_wait_1> + event<ack>[is_ack_valid]                             = state<fin_wait_2>,
            state<fin_wait_2> + event<fin>[is_fin_valid] / send_ack = state<timed_wait>, state<timed_wait> + event<timeout> / send_ack = X);
    }
};

int main()
{
    using namespace sml;

    sm<hello_world> sm;
    assert(sm.is(state<established>));

    sm.process_event(release {});
    assert(sm.is(state<fin_wait_1>));

    sm.process_event(ack {});
    assert(sm.is(state<fin_wait_2>));

    sm.process_event(fin {});
    assert(sm.is(state<timed_wait>));

    sm.process_event(timeout {});
    assert(sm.is(X)); // released
}
#endif // TEST3

#ifdef TEST4

#include <boost/mpl/apply.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <iostream>
#include <typeinfo>

template <class Param1, class Param2, class Func1, class Func2>
struct coalesce
{
    // 对Func1(Param1) 进行元运算以后的结果类型
    using type1 = typename boost::mpl::apply<Func1, Param1>::type;
    // 对Func2(Param2) 进行元运算以后的结果类型
    using type2 = typename boost::mpl::apply<Func2, Param2>::type;

    // 如果type1的类型和 mpl::false_类型相等，就取 type2，
    // 否则取 type1
    using type = typename boost::mpl::if_<boost::is_same<boost::mpl::false_, type1>, type2, type1>::type;
};

// 引入boost::mpl参数占位符
using boost::mpl::_1;
using boost::mpl::_2;

// 此处结果分析一下
// Func1(Param1)的结果是
// boost::mpl::not_<boost::mpl::true_> == boost::mpl::false_
// 所以应该取 type2的类型
// 就是  Func2(Param2)
// boost::mpl::not_<boost::mpl::true_> == boost::mpl::false_
// 所以res1_t::value == false
// BOOST_STATIC_ASSERT(!false) 成功
using res1_t = coalesce<boost::mpl::true_, boost::mpl::true_, boost::mpl::not_<_1>, boost::mpl::not_<_1>>::type;

BOOST_STATIC_ASSERT(!(res1_t::value));

// 分析方法同上面一样
using res2_t = coalesce<boost::mpl::true_, boost::mpl::false_, boost::mpl::not_<_1>, boost::mpl::not_<_1>>::type;

BOOST_STATIC_ASSERT((res2_t::value));

// 分析方法同上面一样
using res3_t = coalesce<boost::mpl::false_, boost::mpl::false_, boost::mpl::not_<_1>, boost::mpl::not_<boost::mpl::not_<_1>>>::type;

BOOST_STATIC_ASSERT((res3_t::value));

using res4_t = coalesce<boost::mpl::false_, boost::mpl::true_, boost::mpl::not_<_1>, boost::mpl::not_<boost::mpl::not_<_1>>>::type;

BOOST_STATIC_ASSERT((res4_t::value));

int main(int argc, char* argv[])
{
    std::cerr << typeid(res2_t).name() << std::endl;
    std::cerr << typeid((res2_t::value)).name() << std::endl;
}

#endif // TEST4

#ifdef TEST5

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace boost;

int main()
{
    // string 转 数值
    // lexical_cast依赖于字符流std::stringstream，其原理相当简单：把源类型读入到字符流中，再写到目标类型中。
    using boost::lexical_cast;
    int nInt = lexical_cast<int>("123");
    // 等价
    int d;
    std::stringstream s;
    s << "123";
    s >> d;

    double dLong = lexical_cast<double>("3.14");
    cout << nInt << endl;
    cout << dLong << endl;

    cout << lexical_cast<string>(0x10) << endl; // 16
    cout << lexical_cast<bool>(1) << endl;      // 1

    // 异常
    try
    {
        nInt = boost::lexical_cast<int>("abcd");
    }
    catch (boost::bad_lexical_cast& e)
    {
        cout << e.what() << endl;
    }

    ////数值 转 string。 boost1.6.7版本，无浮点数的精度问题， 可输完整字符串
    std::string strf = boost::lexical_cast<std::string>(123.123456789);
    cout << strf << endl; // 123.123456789

    return EXIT_SUCCESS;
}

#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <string>

#include "boost/pfr.hpp"

struct some_person
{
    std::string name;
    unsigned birth_year;
};

int main()
{
    some_person val { "Edgar Allan Poe", 1809 };

    std::cout << boost::pfr::get<0>(val)                                  // No macro!
              << " was born in " << boost::pfr::get<1>(val) << std::endl; // Works with any aggregate initializables!

    std::cout << boost::pfr::io(val) << std::endl; // Outputs: {"Edgar Allan Poe", 1809}
}

#endif // TEST6

#ifdef TEST7

#include <boost/type_index.hpp>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
void PrintType()
{
    using boost::typeindex::type_id;
    using boost::typeindex::type_id_with_cvr;
    std::cout << "boost pretty name:\t" << type_id_with_cvr<T>().pretty_name() << '\n';
    std::cout << "boost name:\t" << type_id<T>().name() << '\n';
    // std::cout << "boost raw name"type_id<T>().raw_name() << '\n';
    std::cout << "std name:\t" << typeid(T).name() << '\n';
#if defined __WIN32
    // gcc没有这个函数
    std::cout << "std raw name:\t" << typeid(T).raw_name() << '\n';
#endif
    std::cout << "-----------------------------------------\n";
}

int main()
{
    PrintType<std::vector<int>>();
    PrintType<std::string>();
    PrintType<int>();

    const int& a = 1;
    PrintType<decltype(a)>();

    return 0;
}
#endif // TEST7

#ifdef TEST8

#include <boost/coroutine2/all.hpp>
#include <iostream>

void coroutine_function(boost::coroutines2::coroutine<void>::pull_type& coro_back)
{
    std::cout << "a ";
    coro_back(); // 锚点，中断当前执行返回
    std::cout << "b ";
    coro_back(); // 锚点，返回
    std::cout << "c ";
}

int main()
{
    boost::coroutines2::coroutine<void>::push_type coroutine_object(coroutine_function); // 创建协程
    std::cout << "1 ";
    coroutine_object(); // 运行协程
    std::cout << "2 ";
    coroutine_object(); // 返回锚点，从上次中断的地方继续执行
    std::cout << "3 ";
    coroutine_object(); // 返回锚点，继续运行协程
    return 0;
}

#endif // TEST8
