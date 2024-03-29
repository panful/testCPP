#pragma once

#include <chrono>
#include <iostream>

namespace MyUtility {

// 利用RAII机制来计时
template <typename TimeType = std::chrono::seconds>
class ConsumeTime
{
public:
    ConsumeTime()
        : m_startTime(std::chrono::steady_clock::now())
    {
    }
    ~ConsumeTime()
    {
        auto now = std::chrono::steady_clock::now();
        auto ut = std::chrono::duration_cast<TimeType>(now - m_startTime);

        std::cout << "consume time: " << ut.count() << '\n';
    }

    // 主动获取开始到现在花费的时间
    void ConsumeTimeByNow()
    {
        auto now = std::chrono::steady_clock::now();
        auto ut = std::chrono::duration_cast<TimeType>(now - m_startTime);

        std::cout << "consume time: " << ut.count() << '\n';
    }

private:
    std::chrono::steady_clock::time_point m_startTime;
};

} // namespace MyUility