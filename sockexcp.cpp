#include "sockexcp.h"
#include <string>
#include <sstream>
#include <cerrno>
using namespace net;


//Windows implemenation

#ifdef _WIN32

#include<WinSock2.h>

static const DWORD fflags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_FROM_SYSTEM |
                            FORMAT_MESSAGE_IGNORE_INSERTS;

static const DWORD fargs = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

SockExcp::SockExcp(const char *msg) : errCode(WSAGetLastError())
{
    std::wstringstream ss;
    wchar_t* errDescr = NULL;
    FormatMessageW(fflags, NULL, errCode, fargs,
                  (LPWSTR)&errDescr, 0, NULL);
    ss << msg << '\n' <<
          errCode << ": " <<
          errDescr << '\n';
    errLine = ss.str();
    LocalFree(errDescr);
}

SockExcp::SockExcp(const char *msg, const char *addr, 
                            unsigned short port) : errCode(WSAGetLastError())
{
    std::wstringstream ss;
    wchar_t* errDescr = NULL;
    FormatMessageW(fflags, NULL, errCode, fargs,
                    (LPWSTR)&errDescr, 0, NULL);
    ss << msg << '\n' <<
          addr << ':' << port << '\n' <<
          errCode << ": " << errDescr << '\n';
    errLine = ss.str();
    LocalFree(errDescr);
}


//Linux implementation.

#else

#include <string.h>
#include <locale>
#include <codecvt>
SockExcp::SockExcp(const char* msg) : errCode(errno)
{
    std::stringstream ss;
    ss << msg << '\n' << 
        errCode << ": " << strerror(errCode) << '\n';
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    errLine = converter.from_bytes(ss.str());
}

SockExcp::SockExcp(const char* msg, const char* addr,
    unsigned short port) : errCode(errno)
{
    std::stringstream ss;
    ss << msg << '\n' << 
        addr << ':' << port << '\n' <<
        errno << ": " << strerror_l(errCode, LC_GLOBAL_LOCALE) << '\n';
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    errLine = converter.from_bytes(ss.str());
}

#endif
