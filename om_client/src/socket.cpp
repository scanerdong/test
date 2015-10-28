//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
#include <netinet/tcp.h>

using namespace std;

uint8 g_u8PrintCtrl = 0xff;

static _enResult FillAddr(const string &strAddress, uint16 u16Port, sockaddr_in& sockAddr) 
{
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    if(!inet_aton(strAddress.c_str(), &sockAddr.sin_addr)) return FAIL;
    sockAddr.sin_port = htons(u16Port);

    return  SUCC;
}

CSocket::CSocket()
{
    
}

CSocket::CSocket(uint32 u32Type, uint32 u32Protocol)
{
	int32 i32Flag = 1;
    m_i32SockFd = socket(AF_INET, u32Type, u32Protocol);    
    SetAliveDetect(1, 1, 3);
    setsockopt(m_i32SockFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
}

CSocket::CSocket(int32 i32Socket) 
{
	int32 i32Flag = 1;

    m_i32SockFd = i32Socket;
    setsockopt(m_i32SockFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
}


CSocket::~CSocket() 
{
    OM_INFO("CSocket::~CSocket close fd %d\n", m_i32SockFd);
    
    CloseFd();
}

void CSocket::CloseFd(void )
{
    close(m_i32SockFd);
    m_i32SockFd = INVALID_SOCKET;
}

int32 CSocket::SetLocalPort(uint16 u16LocalPort)
{
    sockaddr_in localAddr;

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(u16LocalPort);

    return bind(m_i32SockFd, (sockaddr *) &localAddr, sizeof(sockaddr_in));;
}

_enResult CSocket::SetAliveDetect(int32 i32Idle, int32 i32Interval, int32 i32Count)
{
	int i32KeepAlive = 1; // 开启keepalive属性
	int i32KeepIdle = (i32Idle>0)?i32Idle:1; // 没有任何数据往来则进行探测 间隔时间
	int i32KeepInterval = (i32Interval>0)? i32Interval:1; // 探测时发包的时间间隔
	int i32KeepCount = (i32Count>0)? i32Count:3; // 探测尝试的次数

    //设置socket的keep alive属性
    if(0 == setsockopt(m_i32SockFd, SOL_SOCKET, SO_KEEPALIVE, (void *)&i32KeepAlive, sizeof(int32))
       && 0 == setsockopt(m_i32SockFd, SOL_TCP, TCP_KEEPIDLE, (void *)&i32KeepIdle, sizeof(int32))
       && 0 == setsockopt(m_i32SockFd, SOL_TCP, TCP_KEEPINTVL, (void *)&i32KeepInterval, sizeof(int32))
       && 0 == setsockopt(m_i32SockFd, SOL_TCP, TCP_KEEPCNT, (void *)&i32KeepCount, sizeof(int32)))
    {
        return SUCC;
    }
	
	return FAIL;
}
/*
CTcpClientSocket::CTcpClientSocket() 
    : CSocket(SOCK_STREAM, IPPROTO_TCP) 
{    
}
*/

CTcpClientSocket::CTcpClientSocket(int32 i32NewSocket) : CSocket(i32NewSocket) 
{
}

int32 CTcpClientSocket::Connect(const string &strForeignAddr, uint16 u16ForeignPort)
{
    sockaddr_in destAddr;

    FillAddr(strForeignAddr, u16ForeignPort, destAddr);
    return connect(m_i32SockFd, (struct sockaddr *)&destAddr, sizeof(destAddr));
}

int32 CTcpClientSocket::Send(const void *pBuf, uint32 u32BufLen) 
{
    return send(m_i32SockFd, (void *) pBuf, u32BufLen, 0);
}

int32 CTcpClientSocket::Recv(void *pBuf, uint32 u32BufLen)
{
    return recv(m_i32SockFd, (void *) pBuf, u32BufLen, 0);
}

int32 CTcpClientSocket::GetForeignAddress(string& strForeignAddr)
{
    int32 i32Ret;
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    i32Ret = getpeername(m_i32SockFd, (sockaddr *) &sockAddr,(socklen_t *) &u32AddrLen);
    strForeignAddr = inet_ntoa(sockAddr.sin_addr);
    
    return i32Ret;
}

uint16 CTcpClientSocket::GetForeignPort(uint16& u16ForeignPort)
{
    int32 i32Ret;
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    i32Ret = getpeername(m_i32SockFd, (sockaddr *) &sockAddr, (socklen_t *) &u32AddrLen);
    u16ForeignPort = ntohs(sockAddr.sin_port);

    return i32Ret;
}

//CTcpServerSocket
CTcpServerSocket::CTcpServerSocket(uint16 u16LocalPort, uint32 u32QueueLen) 
    : CSocket(SOCK_STREAM, IPPROTO_TCP) 
{
    m_strLocalAddress = "";
    m_u16LocalPort = u16LocalPort;
    m_u32QueueLen = u32QueueLen;
    
    SetAliveDetect(1, 1, 3);
    SetLocalPort(u16LocalPort);
    Listen(u32QueueLen);
}

int32 CTcpServerSocket::Listen(uint32 u32QueueLen)
{
    return listen(m_i32SockFd, u32QueueLen);
}

CTcpClientSocket* CTcpServerSocket::Accept(string& strForeignAddr)
{
    int32 i32NewSocket;
    CTcpClientSocket* pclTcpClientSocket;    
    struct sockaddr_in stAddrFrom;
    uint32 u32AddrFromLen = 0;

    memset(&stAddrFrom, 0, sizeof(stAddrFrom));
    u32AddrFromLen = sizeof(stAddrFrom);

    i32NewSocket = accept(m_i32SockFd, (struct sockaddr*)&stAddrFrom, &u32AddrFromLen);
    if(i32NewSocket == INVALID_SOCKET)
    {
        return NULL;
    }
    strForeignAddr = inet_ntoa(stAddrFrom.sin_addr);    
    pclTcpClientSocket = new CTcpClientSocket(i32NewSocket);    
    
    return pclTcpClientSocket;
}


