#include "ContextHub.h"
#include <iostream>

void func()
{
    std::cout << "invoke\n";
}

int main(int argc, char** argv)
{
    // 注册服务
    {
        auto pService = Framework::ContextHub::GetInstance()->GetService<void()>();
        pService->RegisterMethod("func_name", func);
    }

    // 调用服务
    {
        auto pService = Framework::ContextHub::GetInstance()->GetService<void()>();
        pService->SyncInvoke("func_name");
    }

    return 0;
}
