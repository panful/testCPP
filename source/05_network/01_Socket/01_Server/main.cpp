/*
1. 串口服务器 https://bbs.csdn.net/topics/392291851?page=1 socket: remote end point
2. 端口复用 http://www.cppblog.com/zhangyq/archive/2018/12/11/216112.html
3. tcp
4. udp
5. 
6. tcp发送二进制文件 https://my.oschina.net/u/4278643/blog/3863195
7. 自定义数组分割，发送大型char*
8. 使用stringstream分割大型char数组并发送接收
9. server同时监听多个套接字select()
*/


#define TEST5

#ifdef TEST1

#endif // TEST1

#ifdef TEST2

#include <WinSock2.h>
#include <string>

class LeoSockServer
{
public:
    LeoSockServer();
    ~LeoSockServer() = default;
public:
    void Bind(const std::string& addr, uint16_t port);
    void Listen();
    void Accept();
public:
    SOCKET m_socket;
};


LeoSockServer::LeoSockServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {

    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void LeoSockServer::Bind(const std::string& addr, uint16_t port)
{
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(addr.c_str());
    sockAddr.sin_port = htons(port);

    auto ret = bind(m_socket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockaddr));
}

void LeoSockServer::Listen()
{
    auto ret = listen(m_socket, 9);
}

void LeoSockServer::Accept()
{
    //auto ret = accept()
}


int main()
{

}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <WinSock2.h>
#include <thread>

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);

    int bin = bind(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockaddr));

    int lis = listen(sock, 10);

    int addrSize = sizeof(sockaddr);
    std::cout << "before accept\n";

    SOCKET acc = accept(sock, reinterpret_cast<sockaddr*>(&sockAddr), &addrSize);
    std::cout << "after accept\n";

    char buf[1024] = { 0 };
    int rec = recv(acc, buf, sizeof(buf), 0);
    std::cout << "recv from client:" << buf << std::endl;

    int sen = send(acc, (std::string("client input:") + std::string(buf)).c_str(), 1024, 0);

    int clo = closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string>

#define BUF_SIZE 100

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    //绑定套接字
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;  //使用IPv4地址
    //servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(1234);  //端口
    bind(sock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));

    std::cout << "Server start\n";

    //接收客户端请求
    SOCKADDR clntAddr;  //客户端地址信息
    int nSize = sizeof(SOCKADDR);
    char buffer[BUF_SIZE]{ 0 };  //缓冲区
    while (1) {
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &clntAddr, &nSize);

        std::string str = "The client send : " + std::string(buffer);

        //sendto(sock, buffer, strLen, 0, &clntAddr, nSize);
        sendto(sock, str.c_str(), str.size(), 0, &clntAddr, nSize);
        std::cout << "Message from client : " << buffer << std::endl;
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
#include <string>
#include <thread>

using namespace std;

SOCKET m_Client;
void SendFile(const std::string& srcFileName);

int main(int argc, char* argv[])
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字  
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听  
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据  
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    static int index = 0;

    while (true)
    {
        printf("等待连接...\n");
        m_Client = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (m_Client == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

        SendFile("p1.jpg");
        std::this_thread::sleep_for(std::chrono::seconds(10));

        SendFile("p2.jpg");
        std::this_thread::sleep_for(std::chrono::seconds(10));

        SendFile("p3.jpg");

        closesocket(m_Client);
    }

    closesocket(slisten);
    WSACleanup();

    system("pause");
    return 0;
}
void SendFile(const std::string& srcFileName)
{
    int haveSend = 0;
    const int bufferSize = 1024;
    char buffer[bufferSize] = { 0 };
    int readLen = 0;

    ifstream srcFile;
    srcFile.open(srcFileName.c_str(), ios::binary);
    if (!srcFile) {
        return;
    }

    // 获取文件长度
    srcFile.seekg(0, srcFile.end);
    long long length = srcFile.tellg();
    srcFile.seekg(0, srcFile.beg);

    // 发送文件长度，10个字节长度
    send(m_Client, std::to_string(length).c_str(), 10, 0);

    while (!srcFile.eof())
    {
        srcFile.read(buffer, bufferSize);
        readLen = srcFile.gcount();
        send(m_Client, buffer, readLen, 0);
        haveSend += readLen;
    }
    srcFile.close();
    cout << "send: " << haveSend << "B" << endl;
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
void SendFile();

int main(int argc, char* argv[])
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字  
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听  
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据  
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    while (true)
    {
        printf("等待连接...\n");
        m_Client = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (m_Client == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
        SendFile();
        closesocket(m_Client);
    }

    closesocket(slisten);
    WSACleanup();

    system("pause");
    return 0;
}
void SendFile() 
{
    int haveSend = 0;
    const int bufferSize = 1024;
    char buffer[bufferSize] = { 0 };
    int readLen = 0;

    string srcFileName = "test.bmp";
    ifstream srcFile;
    srcFile.open(srcFileName.c_str(), ios::binary);
    if (!srcFile) {
        return;
    }

    while (!srcFile.eof()) 
    {
        srcFile.read(buffer, bufferSize);
        readLen = srcFile.gcount();
        send(m_Client, buffer, readLen, 0);
        haveSend += readLen;
    }
    srcFile.close();
    cout << "send: " << haveSend << "B" << endl;
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
void SendFile();

int main(int argc, char* argv[])
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字  
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听  
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据  
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255];
    while (true)
    {
        printf("等待连接...\n");
        m_Client = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (m_Client == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
        SendFile();
        closesocket(m_Client);
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}
void SendFile()
{
    string srcFileName = "C:\\Users\\yangpan\\Desktop\\test.bmp";
    ifstream srcFile;
    srcFile.open(srcFileName.c_str(), ios::binary);

    if (!srcFile) {
        return;
    }

    srcFile.seekg(0, srcFile.end);
    int fileSize = srcFile.tellg();
    srcFile.seekg(0, srcFile.beg);

    char* buffer = new char[fileSize]();

    srcFile.read(buffer, fileSize);
    int haveSend = 0;

    auto size = fileSize;
    while (size > 0)
    {
        int sendLen = 1024;
        char* sendBuf = new char[1024 + 1](); // 多申请一个
        if (size < 1024)
        {
            sendLen = size;
            size = 0;
        }
        else
        {
            size -= 1024;
        }

        memcpy(sendBuf, buffer, 1024);
        buffer += 1024;
        send(m_Client, sendBuf, sendLen, 0);

        delete[] sendBuf;
    }
    //delete[] buffer;  //会崩溃
    srcFile.close();
    cout << "send: " << haveSend << "B" << endl;
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
void SendFile();

int main(int argc, char* argv[])
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字  
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听  
    if (listen(slisten, 9) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据  
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255];
    while (true)
    {
        printf("等待连接...\n");
        m_Client = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (m_Client == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
        SendFile();
        closesocket(m_Client);
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}
void SendFile()
{
    ifstream ifs("C:\\Users\\yangpan\\Desktop\\test.bmp", std::ios::binary);
    if (!ifs)
    {
        std::cout << "read file error!\n";
    }
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    std::cout << "file length = " << length << '\n';
    stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();

    while (length > 0)
    {
        if (length > 1024)
        {
            auto buffer = new char[1025]();
            ss.read(buffer, 1024);
            send(m_Client, buffer, 1024, 0);
            length -= 1024;
        }
        else
        {
            auto buffer = new char[length + 1];
            ss.read(buffer, length);
            send(m_Client, buffer, length, 0);
            length = 0;
        }
    }
    std::cout << "发送完毕\n";
}
#endif // TEST8

#ifdef TEST9

#include <WinSock2.h>
#include <iostream>

SOCKET initSock(sockaddr* sockAddr)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int bin = bind(sock, sockAddr, sizeof(sockaddr));
    int lis = listen(sock, 20);

    return sock;
}

int main()
{
    std::cout << "start\n";

    WSADATA wsaData;
    auto wsa = WSAStartup(MAKEWORD(2, 2), &wsaData);

    sockaddr_in sockAddr1, sockAddr2;
    memset(&sockAddr1, 0, sizeof(sockaddr));
    sockAddr1.sin_family = AF_INET;
    sockAddr1.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr1.sin_port = htons(1234);

    memset(&sockAddr2, 0, sizeof(sockaddr));
    sockAddr2.sin_family = AF_INET;
    sockAddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr2.sin_port = htons(2345);

    SOCKET sock1 = initSock(reinterpret_cast<sockaddr*>(&sockAddr1));
    SOCKET sock2 = initSock(reinterpret_cast<sockaddr*>(&sockAddr2));

    fd_set fds;

    timeval tv;  //设置timeout
    tv.tv_sec = 2;  //秒    s
    tv.tv_usec = 0; //毫秒  ms

    int maxfds = sock1 > sock2 ? sock1 : sock2;

    while (true)
    {
        FD_ZERO(&fds);

        FD_SET(sock1, &fds);  //必须放在while内部，即每次都需要设置
        FD_SET(sock2, &fds);

        //连接是双向的，所以对第二个和第三个以及第四个参数没必要定义多个，一个连接成功那么另外两个也肯定连接成功
        //第一个参数记得加1
        int sel = select(maxfds + 1, &fds, &fds, 0, &tv);
        switch (sel)
        {
        case -1:
        {
            //failed
            break;
        }
        case 0:
        {
            //timeout
            break;
        }
        default:
        {   //success
            if (FD_ISSET(sock1, &fds))
            {
                int len = sizeof(sockaddr);
                int acc = accept(sock1, reinterpret_cast<sockaddr*>(&sockAddr1), &len);

                char buf[1024] = { 0 };
                int rec = recv(acc, buf, sizeof(buf), 0);
                std::cout << "1234 recv:" << buf << '\n';

                char buf_send[] = "WWWWW";
                int sen = send(acc, buf_send, sizeof(buf_send), 0);
                std::cout << "1234 send:" << buf_send << '\n';

            }

            if (FD_ISSET(sock2, &fds))
            {
                int len = sizeof(sockaddr);
                int acc = accept(sock2, reinterpret_cast<sockaddr*>(&sockAddr2), &len);

                char buf[1024] = { 0 };
                int rec = recv(acc, buf, sizeof(buf), 0);
                std::cout << "2345 recv:" << buf << '\n';

                char buf_send[] = "AAAAA";
                int sen = send(acc, buf_send, sizeof(buf_send), 0);

                std::cout << "2345 send:" << buf_send << '\n';
            }
        }
        }
    }

    std::cout << "end\n";
    system("pause");
    return 0;
}
#endif // TEST9



