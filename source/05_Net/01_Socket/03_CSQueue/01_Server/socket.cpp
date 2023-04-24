#include "socket.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>

constexpr size_t BUFFER_SIZE = 1024;
constexpr size_t HEAD_SIZE = 8;
ServerSocket* ServerSocket::m_instance = nullptr;

ServerSocket* ServerSocket::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() 
        {
            m_instance = new ServerSocket();
        });

    return m_instance;
}

ServerSocket::ServerSocket():
    m_socket(0),
    m_accept(0)
{
}

void ServerSocket::Run()
{
    std::thread recvThread([this]() { this->RecvBuffer(); });
    std::thread sendThread([this]() { this->SendBuffer(); });

    recvThread.detach();
    sendThread.detach();
}

void ServerSocket::AddBuffer(const std::string& buffer)
{
    m_sendBuffers.emplace(buffer);
}

std::string ServerSocket::GetBuffer()
{
    if (m_recvBuffers.empty())
    {
        return std::string();
    }

    auto str = m_recvBuffers.front();
    m_recvBuffers.pop();
    return str;
}

void ServerSocket::RecvBuffer()
{
    while (true)
    {
        // 前8个字节表示即将接受的数据长度
        char headBuffer[HEAD_SIZE]{ 0 };
        recv(m_accept, headBuffer, HEAD_SIZE, 0);
        auto bufferSize = *reinterpret_cast<size_t*>(headBuffer);

        // 一组数据
        if (bufferSize > 0)
        {
            std::stringstream ss;

            char buffer[BUFFER_SIZE]{ 0 };
            int recvSize = 0;
            int haveRecv = 0;

            while (haveRecv + BUFFER_SIZE <= bufferSize)
            {
                recvSize = recv(m_accept, buffer, BUFFER_SIZE, 0);
                ss.write(buffer, recvSize);
                haveRecv += recvSize;
            }
            recvSize = recv(m_accept, buffer, bufferSize - haveRecv, 0);
            haveRecv += recvSize;
            ss.write(buffer, recvSize);

            std::cout << "recv size : " << haveRecv << std::endl;
            m_recvBuffers.emplace(ss.str());
        }
    }
}

void ServerSocket::SendBuffer()
{
    while (true)
    {
        if (!m_sendBuffers.empty())
        {
            auto strBuffer = m_sendBuffers.front();
            m_sendBuffers.pop();

            auto buffer = strBuffer.c_str();
            size_t size = strBuffer.size();

            size_t  sendLen = 0;
            char sendBuf[BUFFER_SIZE + 1]{ 0 };

            if (size > 0)
            {
                // 先发送buffer大小，size_t为8个字节
                send(m_accept, reinterpret_cast<char*>(&size), HEAD_SIZE, 0);

                // 实际buffer
                while (sendLen + BUFFER_SIZE <= size)
                {
                    // 还未发送的buffer长度大于1024
                    memcpy(sendBuf, buffer, BUFFER_SIZE);
                    buffer += BUFFER_SIZE;
                    sendLen += send(m_accept, sendBuf, BUFFER_SIZE, 0);
                }
                // 剩余长度小于1024的buffer
                sendLen += send(m_accept, buffer, static_cast<int>(size - sendLen), 0);

                std::cout << "send size : " << sendLen << std::endl;
            }
        }
    }
}

void ServerSocket::InitSocket()
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        throw("init winsock failed!\n");
    }

    //创建套接字  
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        throw("create socket failed!\n");
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(m_socket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        throw("bind failed!\n");
    }

    //开始监听  
    if (listen(m_socket, 5) == SOCKET_ERROR)
    {
        throw("listen failed!\n");
    }
}

void ServerSocket::AcceptSocket()
{
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    std::cout << "waiting for connection...\n";

    m_accept = accept(m_socket, (SOCKADDR*)&remoteAddr, &nAddrlen);
    if (m_socket == INVALID_SOCKET)
    {
        throw("accept failied!\n");
    }

    std::cout << "Connection successful : " << inet_ntoa(remoteAddr.sin_addr) << '\n';
}

void ServerSocket::ClostSocket()
{
    closesocket(m_accept);
    closesocket(m_socket);
    WSACleanup();
}
