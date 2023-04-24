#include "contextHub.h"
#include <iostream>
#include <string>

void func()
{
    std::cout << "invoke\n";
}

int main(int argc, char** argv)
{
    // 注册服务
    {
        auto pService = Framework::ContextHub::GetInstance()->GetService<void()>();
        auto str = std::string("func_name");
        pService->RegisterMethod(str, func);
    }

    // 调用服务
    {
        auto pService = Framework::ContextHub::GetInstance()->GetService<void()>();
        pService->SyncInvoke("func_name");
    }

    return 0;
}
