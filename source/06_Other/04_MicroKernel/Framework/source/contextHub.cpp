#include "contextHub.h"
#include <iostream>
namespace Framework
{

ContextHub* ContextHub::m_instance = nullptr;
std::once_flag ContextHub::m_inited;

ContextHub* ContextHub::GetInstance()
{
    std::call_once(m_inited, []() {
            m_instance = new ContextHub();
        });

    return m_instance;
}

} // namespace