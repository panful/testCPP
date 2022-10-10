/*
* 1. Windows API以及C方式获取当前时间
* 2. C++方式获取当前时间
* 3. 时间类型转换
* 4. 计时，秒表
* 5. 时长，线程睡眠一定时间
* 6. ++i和i++花费时间
*/

#define TEST4


#ifdef TEST1

#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>

int main()
{
    //system("time");  //将当前时间输出到控制台（黑框），并且可以输入时间修改系统时间

    //SYSTEMTIME SysTime;     //需要包含windows.h头文件
    //GetLocalTime(&SysTime); //得到本地时间，不再需要转换，就是当前时区的时间
    //SysTime.wYear;            //年份，类型为unsigned short
    //SysTime.wMonth;            //月份
    //SysTime.wDay;            //天
    //SysTime.wHour;            //小时
    //SysTime.wMinute;        //分钟
    //SysTime.wSecond;        //秒
    //SysTime.wMilliseconds;    //毫秒
    //SysTime.wDayOfWeek;        //一周的第几天（每周的星期一是第一天）

    //time_t timer;
    //time(&timer);
    //tm* tm1 = nullptr;
    ////tm = localtime(&timer); //报错：C4996会提示不安全
    //localtime_s(tm1,&timer);

    char szTimer[MAX_PATH];  //MAX_PATH需要包含windows.h
    tm tm2;
    time_t now;
    time(&now);  //获取系统日期和时间
    localtime_s(&tm2, &now);
    strftime(szTimer, _countof(szTimer), "%Y%m%d%H%M%S", &tm2);
    //strftime使用参考：http://www.cplusplus.com/reference/ctime/strftime/
    //localtime_r运行于linux下


    //SYSTEMTIME systime;
    //GetLocalTime(&systime);
    //auto year = systime.wYear;
    //char ct[MAX_PATH] = { 0 };
    //sprintf_s(ct, MAX_PATH, "%04d-%04d-%04d", systime.wYear, systime.wMonth, systime.wDay);
    //std::string str = ct;
    //std::string str2 = "Hello";
    //std::string str3 = str2 + str + std::string("HHHH");
    //SYSTEMTIME SysTime;  //需要包含windows.h头文件
    //GetLocalTime(&SysTime);
    //char ct[MAX_PATH];
    //sprintf_s(ct, MAX_PATH, "%04d%02d%02d%02d%02d%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    //std::ofstream outfile;
    //std::string path = "C:\\Users\\Yang\\Desktop\\dump";
    //path = path + ct + std::string(".txt");
    //outfile.open(path);

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

int main()
{
    // 允许用 24h 、 1ms 、 1s 代替对应的,std::chrono::hours(24) 等
    using namespace std::literals; 

    // 当前时刻，会随着系统时间变化，比如修改Windows的系统时间，这个返回值也会跟着变化
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();

    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

    // 将时间戳转换为longlong类型
    auto number = static_cast<long long>(t_c);

    //int tm_sec;   // seconds after the minute - [0, 60] including leap second
    //int tm_min;   // minutes after the hour - [0, 59]
    //int tm_hour;  // hours since midnight - [0, 23]
    //int tm_mday;  // day of the month - [1, 31]
    //int tm_mon;   // months since January - [0, 11]
    //int tm_year;  // years since 1900
    //int tm_wday;  // days since Sunday - [0, 6]
    //int tm_yday;  // days since January 1 - [0, 365]
    //int tm_isdst; // daylight savings time flag

    auto nowTime = std::localtime(&t_c);
    auto year = nowTime->tm_year + 1900;
    auto mon = nowTime->tm_mon + 1;
    std::cout << "year:" << year << "\nmonth:" << mon << '\n';

    std::cout << std::put_time(nowTime, "%F %T.\n") << std::flush;

}
#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    auto time1 = std::chrono::seconds(5);
    auto tiem2 = std::chrono::duration_cast<std::chrono::milliseconds>(time1); //用于将time转换为毫秒

    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    {
        // 最好不要用这种方式计时，因为system_clock会随着系统时间的修改而变化
        auto start1 = std::chrono::system_clock::now();
        // do something
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto end1 = std::chrono::system_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
        std::cout << "took " << duration.count() / 1000.0 << "s" << std::endl;  //花费多少秒

        auto token1 = start1.time_since_epoch(); // 获得1970年1月1日到start1这个时间点经过的时间间隔（duration）
        auto token2 = end1.time_since_epoch();   // epoch是纪元的意思
    }

    {
        // steady_clock就相当于秒表，所以最好使用该类来计算时间消耗
        // 获取开始时间点
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        // 执行业务流程
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // 获取结束时间点
        std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
        // 计算差值
        auto dt = last - start;
        std::cout<< "total token: " << dt.count() << "ns" << std::endl; // 纳秒
    }

    {
        // high_resolution_clock其实就是steady_clock的别名，源文件中有如下定义
        // using high_resolution_clock = steady_clock;
        auto start2 = std::chrono::high_resolution_clock::now();
        // do something
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto end2 = std::chrono::high_resolution_clock::now();
        std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count() / 1000.0 << "s" << std::endl;
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <chrono>
#include <thread>

int main()
{
    using namespace std::chrono_literals;  //时间字面量
    std::this_thread::sleep_for(5s);   //5s
    std::this_thread::sleep_for(100ms);//100ms
    std::this_thread::sleep_for(std::chrono::seconds(3));  //3s
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  //1000ms = 1s

    return 0;
}
#endif // TEST5

#ifdef TEST6

#include <iostream>
#include <chrono>
#include <thread>

constexpr size_t count = 10000;
constexpr double delta = 1000000.0;

int main()
{
    auto time1 = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < count; i++)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    auto time2 = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < count; ++i)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    auto time3 = std::chrono::high_resolution_clock::now();

    auto use1 = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count();
    auto use2 = std::chrono::duration_cast<std::chrono::microseconds>(time3 - time2).count();

    std::cout << use1 / delta << '\t' << use2 / delta << '\n';
}


#endif // TEST6
