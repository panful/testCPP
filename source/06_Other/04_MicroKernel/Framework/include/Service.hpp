///-------------------------------------------------------
///  @file      Service.hpp
///  @brief     内核通信机制服务类 
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.11
///-------------------------------------------------------

#ifndef _SERVICE_HPP_
#define _SERVICE_HPP_

#include <iostream> // std::clog
#include <mutex>
#include <future>
#include <unordered_map>

#include "threadPool.h"

namespace Framework
{

template <typename Func>
class Service
{
public:
    Service() = default;
    ~Service() = default;
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;
public:
    template <typename _Func, typename ..._Args>
    bool RegisterMethod(const std::string& interfaceName, _Func&& func, _Args&& ...args)
    {
        return _RegisterMethod(std::is_convertible<_Func, Func>::type(), interfaceName,
            std::forward<_Func>(func), std::forward<_Args>(args)...);
    }

    /// @brief 同步调用
    template <typename ...Args>
    typename std::invoke_result_t<Func, Args...>
        SyncInvoke(const std::string& interfaceName, Args&& ...args)
    {
        auto func = GetFunctor(interfaceName);
        if (func)
        {
            std::clog << interfaceName << " invoked synchronously.\n";
            return func(std::forward<Args>(args)...);
        }
        else
        {
            throw std::runtime_error(interfaceName + " not found!\n");
        }
    }

    /// @brief 使用线程池异步调用
    template <typename ...Args>
    std::future< typename std::invoke_result_t<Func, Args...>>
        AsyncInvoke(const std::string& interfaceName, Args&& ...args)
    {
        auto func = GetFunctor(interfaceName);
        if (func)
        {
            std::clog << interfaceName << " invoked asynchronously.\n";
            auto fn = std::bind(func, std::forward<Args>(args)...);
            common::ThreadPool::GetInstance()->Submit(fn);
        }
        else
        {
            throw std::runtime_error(interfaceName + " not found!\n");
        }
    }

private:
    template <typename _Func, typename ..._Args>
    bool _RegisterMethod(std::true_type, const std::string& interfaceName, _Func&& func, _Args&& ...args)
    {
        return ImpRegisterMethod(interfaceName, std::forward<_Func>(func));
    }

    bool ImpRegisterMethod(const std::string& interfaceName, Func func)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_funcTable.emplace(interfaceName, std::move(func)).second)
        {
            std::clog << interfaceName << " registered\n";
            return true;
        }
        return false;
    }

    Func GetFunctor(const std::string& interfaceName)
    {
        Func func;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            if (m_funcTable.count(interfaceName))
            {
                func = m_funcTable.at(interfaceName);
            }
        }
        return func;
    }

private:
    std::mutex m_mutex;
    std::unordered_map<std::string, Func> m_funcTable;
};

} // namespace

#endif // !_SERVICE_HPP_
