/*
1. 串口服务器 https://bbs.csdn.net/topics/392291851?page=1 socket: remote end point
2. 端口复用 http://www.cppblog.com/zhangyq/archive/2018/12/11/216112.html
3. tcp
4. udp
5. 接收多个文件，TEST6的增强版
6. tcp接收二进制文件 https://my.oschina.net/u/4278643/blog/3863195
7. 自定义数组分割，发送大型char*
8. 使用stringstream分割大型char数组并发送接收
9. server同时监听多个套接字select()
*/

#define TEST5

#ifdef TEST1

#endif // TEST1

#ifdef TEST2

#endif // TEST2

#ifdef TEST3

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 100
int main() {
    //初始化DLL
    WSADATA wsaData;
    auto start = WSAStartup(MAKEWORD(2, 2), &wsaData);

    //向服务器发起请求
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);

    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //获取用户输入的字符串并发送给服务器
    char bufSend[BUF_SIZE] = { 0 };
    printf("Input a string: ");
    gets_s(bufSend);
    for (int i = 0; i < 3; i++) {
        send(sock, bufSend, strlen(bufSend), 0);
    }

    //接收服务器传回的数据
    char bufRecv[BUF_SIZE] = { 0 };
    recv(sock, bufRecv, BUF_SIZE, 0);

    //输出接收到的数据
    printf("Message form server: %s\n", bufRecv);

    closesocket(sock);  //关闭套接字
    WSACleanup();  //终止使用 DLL

    system("pause");
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <stdio.h>
#include <WinSock2.h>

#define BUF_SIZE 100

int main() 
{
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);

    //服务器地址信息
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(1234);

    //不断获取用户输入并发送给服务器，然后接受服务器数据
    struct sockaddr fromAddr;
    int addrLen = sizeof(fromAddr);
    while (1) {
        char buffer[BUF_SIZE] = { 0 };
        printf("Input a string: ");
        gets_s(buffer);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
        buffer[strLen] = 0;
        printf("Message form server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include <sstream>

SOCKET m_Client;
void RecvFile(long long fileSize);

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }

    //while(true){
        m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_Client == INVALID_SOCKET)
        {
            printf("invalid socket!");
            return 0;
        }

        sockaddr_in serAddr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(8888);
        serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

        if (connect(m_Client, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(m_Client);
            return 0;
        }

        const int bufferSize = 10;
        char buffer[bufferSize] = { 0 };
        while(recv(m_Client, buffer, 10, 0) > 0)
        {
            //接收文件
            RecvFile(std::stoll(std::string(buffer)));
        }

        closesocket(m_Client);
    //}

    WSACleanup();

    system("pause");
    return 0;

}

void RecvFile(long long fileSize)
{
    static int index = 0;
    std::cout << "receive the " << index << " image." << std::endl;

    const int bufferSize = 1024;
    char buffer[bufferSize] = { 0 };
    int readLen = 0;
    int haveRecv = 0;

    std::string desFileName = "recv_" + std::to_string(index++) + ".jpg";
    std::ofstream desFile;
    desFile.open(desFileName.c_str(), std::ios::binary);

    if (!desFile)
    {
        return;
    }

    do
    {
        if (haveRecv + 1024 <= fileSize)
        {
            readLen = recv(m_Client, buffer, bufferSize, 0);
        }
        else
        {
            readLen = recv(m_Client, buffer, fileSize - haveRecv, 0);
        }

        if (readLen == 0)
        {
            break;
        }

        desFile.write(buffer, readLen);
        haveRecv += readLen;

        if (haveRecv >= fileSize)
        {
            break;
        }
        
    } while (true);

    desFile.close();
}
#endif // TEST5

#ifdef TEST6

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>

using namespace std;

SOCKET m_Client;
void RecvFile();

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }
    //while(true){
    m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Client == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(m_Client, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败 
        printf("connect error !");
        closesocket(m_Client);
        return 0;
    }
    RecvFile();
    closesocket(m_Client);
    //}

    WSACleanup();

    system("pause");
    return 0;

}

void RecvFile() 
{
    cout << "start recv!" << endl;
    const int bufferSize = 1024;
    char buffer[bufferSize] = { 0 };
    int readLen = 0;
    string desFileName = "recv.bmp";
    ofstream desFile;
    desFile.open(desFileName.c_str(), ios::binary);
    if (!desFile)
    {
        return;
    }
    do
    {
        readLen = recv(m_Client, buffer, bufferSize, 0);
        if (readLen == 0)
        {
            break;
        }
        else
        {
            desFile.write(buffer, readLen);
        }
    } while (true);

    desFile.close();
}
#endif // TEST6

#ifdef TEST7

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET m_Client;

void RecvFile();

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }
    //while(true){
    m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Client == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(m_Client, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败 
        printf("connect error !");
        closesocket(m_Client);
        return 0;
    }
    RecvFile();
    closesocket(m_Client);
    //}

    std::cout << "接收完毕\n";
    WSACleanup();
    return 0;

}

void RecvFile() {
    cout << "start recv!" << endl;
    const int bufferSize = 1024;
    char buffer[bufferSize] = { 0 };
    int readLen = 0;
    string desFileName = "C:\\Users\\yangpan\\Desktop\\out.bmp";
    ofstream desFile;
    desFile.open(desFileName.c_str(), ios::binary);
    if (!desFile)
    {
        return;
    }
    int recvSize = 0;
    while (true)
    {
        if (recvSize + 1024 > 1440054)
        {
            int size1 = 1440054 - recvSize;
            char* recvBuffer = new char[size1]();
            int rec = recv(m_Client, recvBuffer, size1, 0);
            //strcat(recvBuf, recvBuffer);
            desFile.write(recvBuffer, size1);

            delete[] recvBuffer;
            break;
        }
        else
        {
            char* recvBuffer = new char[1024]();
            int rec = recv(m_Client, recvBuffer, 1024, 0);
            //strcat(recvBuf, recvBuffer);

            desFile.write(recvBuffer, 1024);

            recvSize += 1024;
            delete[] recvBuffer;
        }
    }

    desFile.close();
}
#endif // TEST7

#ifdef TEST8

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET m_Client;

void RecvFile();

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }
    //while(true){
    m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Client == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(m_Client, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败 
        printf("connect error !");
        closesocket(m_Client);
        return 0;
    }
    RecvFile();
    closesocket(m_Client);
    //}

    std::cout << "接收完毕\n";
    WSACleanup();
    return 0;

}

void RecvFile() {
    cout << "start recv!" << endl;

    std::ofstream ofs("C:\\Users\\yangpan\\Desktop\\out.bmp", std::ios::binary);
    std::stringstream ss;

    int recvSize = 0;
    while (true)
    {
        if (recvSize + 1024 < 1440054)
        {
            char* buffer = new char[1025]();
            recv(m_Client, buffer, 1024, 0);
            ss.write(buffer, 1024);
            recvSize += 1024;
        }
        else
        {
            int lastSize = 1440054 - recvSize;
            char* buffer = new char[lastSize + 1];
            recv(m_Client, buffer, lastSize, 0);
            ss.write(buffer, lastSize);
            break;
        }
    }

    auto str = ss.str();
    auto len = str.length();
    auto sz = str.c_str();
    std::cout << "recv length = " << len << '\n';

    ofs.write(sz, len);
    ofs.close();

    std::cout << "写文件完毕\n";
}
#endif // TEST8

#ifdef TEST9

#include <WinSock2.h>
#include <iostream>

int main()
{
    WSADATA wsaData;
    int wsa = WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);

    int con = connect(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockaddr));

    char buf[] = "DDDD";
    int sen = send(sock, buf, sizeof(buf), 0);

    std::cout << "recv start:\n";
    char buf_rec[1024] = { 0 };
    int rec = recv(sock, buf_rec, sizeof(buf_rec), 0);
    std::cout << buf_rec << '\n';
    std::cout << "recv end:\n";

    int clo = closesocket(sock);
    WSACleanup();
    system("pause");

}

#endif // TEST9
