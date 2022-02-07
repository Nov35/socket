#ifndef SOCKETEXCP_H
#define SOCKETEXCP_H
#include <string>
#include <exception>

namespace net {
    
//A class is used to create excepions with OS specific error messages.
//For now it has only a wchar implementation;

    class SockExcp : public std::exception {
    protected:
        int errCode;
        std::wstring errLine;
    public:
        SockExcp(const char* msg);
        SockExcp(const char* msg, const char* addr, unsigned short port);
        SockExcp(const SockExcp& oth) 
                                 : errCode(oth.errCode), errLine(oth.errLine) {}
        const wchar_t* what() {
            return errLine.c_str();
        }
        int getCode() {
            return errCode;
        }
    };

}

#endif
