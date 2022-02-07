#ifndef SOCKET_H
#define SOCKET_H
#include <string>

#ifdef _WIN32
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#else
#include <netinet/in.h>
#define SOCKET int
#endif

namespace net {

//Abstract class Socket, that initialises WSA in windows, stores a descriptor
//and provides send/revcieve methods. It has "close" call in destructor,
//so copy constructor is disabled.

    class Socket {
    protected:
#ifdef _WIN32
        static WSADATA m_wsa;
        static int startup;
#endif
        SOCKET m_fd;
        Socket(int family, int type, int protocol);
        Socket(SOCKET fd) : m_fd(fd) {}
    public:
        int send(void* dest, int size);
        int recv(void* data, int size);
        virtual void bind() = 0;
        virtual ~Socket();
    private:
        Socket(const Socket&) = delete;
    };

//The class provides easy acces to the socket functionality. A names of the
//methods are the same as names of original C functions

    class TcpSocket : public Socket {
        sockaddr_in m_addr;
    public:
        TcpSocket();
        TcpSocket(const char* ip, unsigned short port);
        void connect(const char* ip, unsigned short port);
        void listen();
        TcpSocket* accept();
        std::string getIp();
        unsigned short getPort() {
            return ntohs(m_addr.sin_port);
        }
    private:
        void bind();
        TcpSocket(SOCKET fd, sockaddr_in addr) : Socket(fd), m_addr(addr) {}
    };

}

#endif
