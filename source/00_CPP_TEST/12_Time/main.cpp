/*
 * 1. Windows API以及C方式获取当前时间
 * 2. C++方式获取当前时间
 * 3. 时间类型转换
 * 4. 计时，秒表
 * 5. 时间字面量
 */

#define TEST2

#ifdef TEST1

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    // system("time");  //将当前时间输出到控制台（黑框），并且可以输入时间修改系统时间

    // SYSTEMTIME SysTime;     //需要包含windows.h头文件
    // GetLocalTime(&SysTime); //得到本地时间，不再需要转换，就是当前时区的时间
    // SysTime.wYear;            //年份，类型为unsigned short
    // SysTime.wMonth;            //月份
    // SysTime.wDay;            //天
    // SysTime.wHour;            //小时
    // SysTime.wMinute;        //分钟
    // SysTime.wSecond;        //秒
    // SysTime.wMilliseconds;    //毫秒
    // SysTime.wDayOfWeek;        //一周的第几天（每周的星期一是第一天）

    // time_t timer;
    // time(&timer);
    // tm* tm1 = nullptr;
    ////tm = localtime(&timer); //报错：C4996会提示不安全
    // localtime_s(tm1,&timer);

    char szTimer[MAX_PATH]; // MAX_PATH需要包含windows.h
    tm tm2;
    time_t now;
    time(&now); // 获取系统日期和时间
    localtime_s(&tm2, &now);
    strftime(szTimer, _countof(szTimer), "%Y%m%d%H%M%S", &tm2);
    // strftime使用参考：http://www.cplusplus.com/reference/ctime/strftime/
    // localtime_r运行于linux下

    // SYSTEMTIME systime;
    // GetLocalTime(&systime);
    // auto year = systime.wYear;
    // char ct[MAX_PATH] = { 0 };
    // sprintf_s(ct, MAX_PATH, "%04d-%04d-%04d", systime.wYear, systime.wMonth, systime.wDay);
    // std::string str = ct;
    // std::string str2 = "Hello";
    // std::string str3 = str2 + str + std::string("HHHH");
    // SYSTEMTIME SysTime;  //需要包含windows.h头文件
    // GetLocalTime(&SysTime);
    // char ct[MAX_PATH];
    // sprintf_s(ct, MAX_PATH, "%04d%02d%02d%02d%02d%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute,
    // SysTime.wSecond); std::ofstream outfile; std::string path = "C:\\Users\\Yang\\Desktop\\dump"; path = path + ct + std::string(".txt");
    // outfile.open(path);

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <algorithm>
#include <chrono>
#include <iostream>

int main()
{
    // 当前时间点，会随着系统时间变化，比如修改Windows的系统时间，这个返回值也会跟着变化
    // const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    // 获取当前时间的年月日（注意时区导致的误差）
    {
        auto now = std::chrono::system_clock::now();
        auto tp  = std::chrono::floor<std::chrono::days>(now); // 将当前时间点转换为天数时间点
        auto ymd = std::chrono::year_month_day(tp);            // C++20
        auto y   = ymd.year();
        auto m   = ymd.month(); // 重载了类型转换 unsigned int
        auto d   = ymd.day();

        std::cout << std::format("Year: {} Month: {} Day: {}\n", y, static_cast<uint32_t>(m), d);
    }

    std::cout << "-----------------------------------------\n";

    // 格式化时间
    {
        auto now = std::chrono::system_clock::now();
        std::cout << std::format("{}\n", now);
        std::cout << std::format("{:%Y-%m-%d %H:%M:%S}\n", now); // %Y-%m-%d:年月日 %H:%M:%S:时分秒
        std::cout << std::format("{:%F %T %Z}\n", now);          // %F:年月日 %T:时分秒 %Z:时区缩写
        std::cout << std::format("{:%OS}\n", now);               // 可以控制秒的精度
    }

    std::cout << "-----------------------------------------\n";

    // 时区
    {
        auto now               = std::chrono::system_clock::now();
        auto default_time_zone = std::chrono::current_zone();                     // 默认时区
        auto local_time        = std::chrono::zoned_time(default_time_zone, now); // 根据时区获取当前时间（本地时间）

        std::cout << std::format("Current local time: {:%Y-%m-%d %H:%M:%S %Z}\n", local_time);
        std::cout << std::format("time zone name: {}\n", local_time.get_time_zone()->name());
    }

    return 0;
}
#endif // TEST2

#ifdef TEST3

#include <chrono>
#include <format>
#include <iostream>
#include <thread>

int main()
{
    // 从一个时长转换到另一个时长，比如：时长1小时转为60分钟
    {
        auto time1 = std::chrono::seconds(5);
        auto tiem2 = std::chrono::duration_cast<std::chrono::milliseconds>(time1); // 将 5s 转换为 毫秒
        std::cout << std::format("Before: {}\t After: {}\n", time1, tiem2);
    }

    // 将时间点转换为同一时钟上拥有不同时长的另一个时间点
    {
        auto tp1 = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>(std::chrono::seconds(5));
        auto tp2 = std::chrono::time_point_cast<std::chrono::milliseconds>(tp1);
        std::cout << std::format("Before: {}\t After: {}\n", tp1, tp2);
    }

    // 转换一个时钟的时间点为另一个 C++20
    {
        auto tp1 = std::chrono::utc_clock::now();
        auto tp2 = std::chrono::clock_cast<std::chrono::gps_clock>(tp1);
        std::cout << std::format("Before: {}\t After: {}\n", tp1, tp2);
    }

    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    // 最好不要用这种方式计时，因为 std::system_clock 会随着系统时间的修改而变化
    {
        auto start1 = std::chrono::system_clock::now();
        // do something
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto end1           = std::chrono::system_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
        std::cout << "took " << duration.count() / 1000.0 << "s" << std::endl; // 花费多少秒

        auto token1 = start1.time_since_epoch(); // 获得1970年1月1日到start1这个时间点经过的时间间隔（duration）
        auto token2 = end1.time_since_epoch();   // epoch是纪元的意思
    }

    // std::steady_clock 就相当于秒表，所以最好使用该类来计算时间消耗
    {
        // 获取开始时间点
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        // 执行业务流程
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // 获取结束时间点
        std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
        // 计算差值
        auto dt = last - start;
        std::cout << "total token: " << dt.count() << "ns" << std::endl; // 纳秒
    }

    // std::high_resolution_clock 一般为别名，定义为 std::system_clock 或 std::steady_clock 两者之一
    // 不推荐使用
    {
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
#include <format>
#include <iostream>

int main()
{
    using namespace std::chrono_literals;

    std::chrono::local_days ymd { 2023y / 12 / 17 };
    std::cout << std::format("Y-M-D: {}\n", ymd);

    auto hms = 14h + 20min + 10s;
    std::cout << std::format("H:M:S: {:%T}\tSecond: {}", hms, hms);
}

#endif // TEST5
