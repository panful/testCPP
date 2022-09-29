#include <iostream>
#include "socket.h"

int main(int c,char** v)
{
    std::clog << "   === server start ===\n";

    ServerSocket::GetInstance()->InitSocket();
    ServerSocket::GetInstance()->AcceptSocket();
    ServerSocket::GetInstance()->Run();
    while (1)
    {
        auto ret = ServerSocket::GetInstance()->GetBuffer();
        if (ret.empty())
        {
            continue;
        }
        std::cout << ServerSocket::GetInstance()->GetBuffer() << std::endl;
    }

    system("pause");
    return 0;
}
