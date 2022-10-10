///-------------------------------------------------------
///  @file      contextHub.h
///  @brief     内核通信机制容器类头文件 
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.10
///-------------------------------------------------------

#ifndef _CONTEXT_HUB_H_
#define _CONTEXT_HUB_H_

#include "Service.hpp"
#include <unordered_map>
#include <memory>
#include <any> // C++17
#include <mutex>
#include <functional>

namespace Framework
{
    template <typename Func>
    using ServicePtr = std::shared_ptr<Service<std::function<Func>>>;

    class __declspec(dllexport) ContextHub
    {
    public:
        static ContextHub* GetInstance();
        //static void Destory() ;
        ContextHub(const ContextHub&) = delete;
        ContextHub& operator=(const ContextHub&) = delete;
        ~ContextHub() = default;
    private:
        ContextHub() = default;
    public:
        /// @brief 获取服务实例
        template<typename Func>
        typename std::enable_if<std::is_function<Func>::value, ServicePtr<Func>>::type
            GetService()
        {
            auto typeName = typeid(Func).name();
            if (m_services.emplace(typeName, ServicePtr<Func>()).second)
            {
                m_services[typeName] = std::make_shared<Service<std::function<Func>>>();
            }
            return std::any_cast<ServicePtr<Func>>(m_services[typeName]);
        }

    private:
        static ContextHub* m_instance;
        static std::once_flag m_inited;

        std::unordered_map<std::string, std::any> m_services;
    };

} // namespace Framework

#endif // !_CONTEXT_HUB_H_

