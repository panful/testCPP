#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <string>
#include <queue>

class ServerSocket
{
public:
    static ServerSocket* GetInstance();

private:
    static ServerSocket* m_instance;

public:
    void AddBuffer(const std::string& buffer);
    std::string GetBuffer();

    void Run();

    void InitSocket();
    void AcceptSocket();
    void ClostSocket();

private:
    ServerSocket();
    ~ServerSocket() = default;

    void SendBuffer();
    void RecvBuffer();

private:
    SOCKET m_socket;
    SOCKET m_accept;

    std::queue<std::string> m_recvBuffers;
    std::queue<std::string> m_sendBuffers;
};

#endif // !_SERVER_SOCKET_H_
