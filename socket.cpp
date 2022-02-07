#include "socket.h"
#include "sockexcp.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#define CLOSE(SOCK_FD) closesocket(SOCK_FD)
typedef int socklen_t;
WSADATA net::Socket::m_wsa;
int net::Socket::startup = WSAStartup(MAKEWORD(2, 2), &net::Socket::m_wsa);

#else
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define CLOSE(SOCK_FD) close(SOCK_FD)
#endif

using namespace net;

//Constructor of the Socket class, creates socket, throws an exception if
//creating was unsuccessful.

Socket::Socket(int family, int type, int protocol){
        m_fd = socket(family, type, protocol);
        if(m_fd == INVALID_SOCKET)
            throw(SockExcp("Cannot create socket."));
}

//Destructor that uses the macro to call the OS specific function to end work
//with socket.

Socket::~Socket()
{
    CLOSE(m_fd);
}

//Recieving data from a sender, takes as a parameters pointer to a buffer 
//for a recieved data and size of the buffer, returns amount of recieved bytes.

int Socket::recv(void *dest, int size)
{
    return ::recv(m_fd, (char*)dest, size, 0);
}

//Sending data to the recipient, takes as a parameter pointer to data and it's
//size, returns amount of bytes sent.

int Socket::send(void *data, int size)
{
    return ::send(m_fd, (const char*)data, size, 0);
}



//Default constructor of the TcpSocket class, since the class inherts the 
//Socket class it creates new socket and uses it's descriptor.

TcpSocket::TcpSocket()
        : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = 0;
    m_addr.sin_addr.s_addr = 0;
}

//This contructor takes string presentation of ip adress and port number and
//binds it to the socket. Is is useful in a case of creating server's listening 
//socket.

TcpSocket::TcpSocket(const char *ip, unsigned short port) 
        : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
    m_addr.sin_family = AF_INET;
    if (ip)
        inet_pton(AF_INET, ip, &m_addr.sin_addr);
    else
        m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(port);
    bind();
}

//The method uses m_addr struct to bind adress to the socket, throws an 
//exception in a case of unsuccessful binding. The method is private and is
//called only from parameterized constructor.

void TcpSocket::bind()
{
    int res = ::bind(m_fd, (sockaddr*)&m_addr, sizeof(m_addr));
    if(res == SOCKET_ERROR){
        const char opt = 1;
        setsockopt
            (m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        res = ::bind(m_fd, (sockaddr*)&m_addr, sizeof(m_addr));
    } if (res == -SOCKET_ERROR){
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, ipstr, INET_ADDRSTRLEN);
        throw(SockExcp("Cannot attach addres:",
                       ipstr,
                       ntohs(m_addr.sin_port)));
    }
}

//Takes string form of the server's ip, and ushort port, after use m_addr
//struct will contain the server's addres. Throws an exception if connection
//was unsuccessful.

void TcpSocket::connect(const char *ip, unsigned short port)
{
    m_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &m_addr.sin_addr);
    m_addr.sin_port = htons(port);
    int res =::connect(m_fd, (sockaddr*) &m_addr, sizeof(m_addr));
    if (res == SOCKET_ERROR)
        throw(SockExcp("Cannot connect to the addres:", ip, port));
}

//Switches socket to listening, throws an exception if a case of error.

void TcpSocket::listen()
{
    if(::listen(m_fd, 16) == SOCKET_ERROR) 
        throw(SockExcp("Cannot start listening socket!"));
}

//Accepts a client on a listening server, returns null pointer in a case if no
//one was waiting or pointer to initialised client object located in DYNAMIC
//memory, it means that after use it must be FREED manually by using "delete".

TcpSocket* TcpSocket::accept()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    SOCKET fd = ::accept(m_fd, (sockaddr*) &addr, &len);
    if(fd == INVALID_SOCKET)
        return nullptr;
    return new TcpSocket(fd, addr);
}

//Returns own ip on the listening socket, server's ip on socket that used
//connect, and ip of the client on socket created with accept method.

std::string TcpSocket::getIp()
{
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, ipstr, INET_ADDRSTRLEN);
    return ipstr;
}
