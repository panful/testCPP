#include <iostream>
#include "socket.h"

int main(int argc,char** argv)
{   
    std::clog << "   === client start ===\n";

    ClientSocket::GetInstance()->InitSocket();
    ClientSocket::GetInstance()->ConnectSocket();
    ClientSocket::GetInstance()->Run();

    ClientSocket::GetInstance()->AddBuffer("test 123 +-*/ end");
    ClientSocket::GetInstance()->AddBuffer("222\t1234\n\tsss2222");
    ClientSocket::GetInstance()->AddBuffer("333\t300\n\n\t\tssss11113333");

    system("pause");
    return 0;
}
