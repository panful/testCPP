#include "socket.h"

#include <mutex>
#include <iostream>
#include <sstream>
#include <thread>

constexpr size_t BUFFER_SIZE = 1024;
constexpr size_t HEAD_SIZE = 8;
ClientSocket* ClientSocket::m_instance = nullptr;

ClientSocket* ClientSocket::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {m_instance = new ClientSocket(); });
    return m_instance;
}

ClientSocket::ClientSocket() :
    m_socket(0)
{
}

void ClientSocket::Run()
{
    std::thread recvThread([this]() { this->RecvBuffer(); });
    std::thread sendThread([this]() { this->SendBuffer(); });

    recvThread.detach();
    sendThread.detach();
}

void ClientSocket::AddBuffer(const std::string& buffer)
{
    m_sendBuffers.emplace(buffer);
}

std::string ClientSocket::GetBuffer()
{
    if (m_recvBuffers.empty())
    {
        return std::string();
    }

    auto str = m_recvBuffers.front();
    m_recvBuffers.pop();
    return str;
}

void ClientSocket::SendBuffer()
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
                send(m_socket, reinterpret_cast<char*>(&size), HEAD_SIZE, 0);

                // 实际buffer
                while (sendLen + BUFFER_SIZE <= size)
                {
                    // 还未发送的buffer长度大于1024
                    memcpy(sendBuf, buffer, BUFFER_SIZE);
                    buffer += BUFFER_SIZE;
                    sendLen += send(m_socket, sendBuf, BUFFER_SIZE, 0);
                }
                // 剩余长度小于1024的buffer
                sendLen += send(m_socket, buffer, static_cast<int>(size - sendLen), 0);

                std::cout << "send size : " << sendLen << std::endl;
            }
        }
    }
}

void ClientSocket::RecvBuffer()
{
    while (true)
    {
        // 前8个字节表示即将接受的数据长度
        char headBuffer[HEAD_SIZE]{ 0 };
        recv(m_socket, headBuffer, HEAD_SIZE, 0);
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
                recvSize = recv(m_socket, buffer, BUFFER_SIZE, 0);
                ss.write(buffer, recvSize);
                haveRecv += recvSize;
            }
            recvSize = recv(m_socket, buffer, bufferSize - haveRecv, 0);
            ss.write(buffer, recvSize);

            std::cout << "recv size : " << recvSize << std::endl;

            m_recvBuffers.emplace(ss.str());
        }
    }
}

void ClientSocket::InitSocket()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        throw("init socket failed!\n");
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        throw("invalid socket!\n");
    }
}

void ClientSocket::ConnectSocket()
{
    sockaddr_in serAddr{};
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    if (connect(m_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        closesocket(m_socket);
        throw("connect failed!");
    }
}

void ClientSocket::ClostSocket()
{
    closesocket(m_socket);
    WSACleanup();
}
