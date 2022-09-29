#ifndef _SOCKET_H_
#define _SOCKET_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <string>
#include <queue>

class ClientSocket
{
public:
    static ClientSocket* GetInstance();

    void Run();

    void AddBuffer(const std::string& buffer);
    std::string GetBuffer();

    void InitSocket();
    void ConnectSocket();
    void ClostSocket();

private:
    void SendBuffer();
    void RecvBuffer();

private:
    ClientSocket();
    ~ClientSocket() = default;
    ClientSocket& operator=(const ClientSocket&) = delete;
    ClientSocket(const ClientSocket&) = delete;

private:
    static ClientSocket* m_instance;
    SOCKET m_socket;

    std::queue<std::string> m_sendBuffers;
    std::queue<std::string> m_recvBuffers;
};

#endif // !_SOCKET_H_
