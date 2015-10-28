/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Socket.cpp
 * @version : 1.0.0
 * @brief : 实现socket通信设备类
 *   
 * @author : zhangx
 * @date : 2011年3月29日 9:29:23
 * @history : 
 *   
 */

/************************引用部分*****************************/
#include <netinet/tcp.h> //设置socket连接检测属性

#include "Socket.h"
#include "MemMgr.h"
#include "FdResMgr.h"

/************************前向声明部分***********************/


/************************定义部分*****************************/
typedef void raw_type;
const static uint8 CONNECT_TIME_OUT_TIME = 20;
const static uint8 RW_TIME_OUT_TIME = 20;


/************************实现部分*****************************/
static _enResult FillAddr(const string &strAddress, uint16 u16Port, sockaddr_in& sockAddr) 
{
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    if(!inet_aton(strAddress.c_str(), &sockAddr.sin_addr)) return FAIL;
    sockAddr.sin_port = htons(u16Port);

    return  SUCC;
}

//CSocket
CSocket::CSocket(uint32 u32Type, uint32 u32Protocol)
{
	int32 i32Flag = 1;
    
    SOCK_OPEN(m_streamFd, AF_INET, u32Type, u32Protocol);
    
    setsockopt(m_streamFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
}

CSocket::CSocket(int32 i32Socket) 
{
	int32 i32Flag = 1;

    m_streamFd = i32Socket;
    setsockopt(m_streamFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
}

CSocket::~CSocket() 
{
    OM_INFO("CSocket::~CSocket close fd %d\n", m_streamFd);
    
    CloseFd();
}

string CSocket::GetLocalAddr()
{
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    getsockname(m_streamFd, (sockaddr *)&sockAddr, (socklen_t *) &u32AddrLen);
    return inet_ntoa(sockAddr.sin_addr);
}

uint16 CSocket::GetLocalPort()
{
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    getsockname(m_streamFd, (sockaddr *) &sockAddr, (socklen_t *)&u32AddrLen);
    return ntohs(sockAddr.sin_port);
}

int32 CSocket::SetLocalPort(uint16 u16LocalPort)
{
    sockaddr_in localAddr;

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(u16LocalPort);

    return bind(m_streamFd, (sockaddr *) &localAddr, sizeof(sockaddr_in));;
}

int32 CSocket::SetLocalAddrAndPort(const string &strLocalAddress, uint16 u16LocalPort)
{
    sockaddr_in localAddr;

    FillAddr(strLocalAddress, u16LocalPort, localAddr);
    return bind(m_streamFd, (sockaddr *) &localAddr, sizeof(sockaddr_in));
}

_enResult CSocket::SetBlock(_enBool bBlocked) 
{
    ulint32 ul = bBlocked?0:1;
    ioctl(m_streamFd, FIONBIO, &ul);  //1:设置为非阻塞模式 0:为阻塞模式

    return SUCC;
}

_enResult CSocket::SetAliveDetect(int32 i32Idle, int32 i32Interval, int32 i32Count)
{
    //时间单位0.5秒
	int i32KeepAlive = 1; // 开启keepalive属性
	int i32KeepIdle = (i32Idle>0)?i32Idle:1; // 没有任何数据往来则进行探测 间隔时间
	int i32KeepInterval = (i32Interval>0)? i32Interval:1; // 探测时发包的时间间隔
	int i32KeepCount = (i32Count>0)? i32Count:3; // 探测尝试的次数

    //设置socket的keep alive属性
    if(0 == setsockopt(m_streamFd, SOL_SOCKET, SO_KEEPALIVE, (void *)&i32KeepAlive, sizeof(int32))
       && 0 == setsockopt(m_streamFd, SOL_TCP, TCP_KEEPIDLE, (void *)&i32KeepIdle, sizeof(int32))
       && 0 == setsockopt(m_streamFd, SOL_TCP, TCP_KEEPINTVL, (void *)&i32KeepInterval, sizeof(int32))
       && 0 == setsockopt(m_streamFd, SOL_TCP, TCP_KEEPCNT, (void *)&i32KeepCount, sizeof(int32)))
    {
        return SUCC;
    }
	
	return FAIL;
}

void CSocket::CloseFd()
{
    SOCK_CLOSE(m_streamFd);
    m_streamFd = INVALID_SOCKET;
}

//CComunicatingSocket
CCommunicatingSocket::CCommunicatingSocket(uint32 u32Type, uint32 u32Protocol) 
    : CSocket(u32Type, u32Protocol) 
{    
    timeval timeout = {RW_TIME_OUT_TIME, 0};
    
    //设置发送超时
    setsockopt(m_streamFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(struct timeval));
    
    //设置接收超时
    setsockopt(m_streamFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
}

CCommunicatingSocket::CCommunicatingSocket(uint32 i32NewSocket)
     : CSocket(i32NewSocket) 
{    
    timeval timeout = {RW_TIME_OUT_TIME, 0};
    
    //设置发送超时
    setsockopt(m_streamFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(struct timeval));
    
    //设置接收超时
    setsockopt(m_streamFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
}

int32 CCommunicatingSocket::Connect(const string &strForeignAddr, uint16 u16ForeignPort)
{
    sockaddr_in destAddr;
    int32 i32Ret = 0;
    int32 i32Error = 0;
    int32 i32Len = 0;
    timeval tm;
    fd_set set;
    
    m_strForeignAddr = strForeignAddr;
    m_u16ForeignPort = u16ForeignPort;
    
    FillAddr(strForeignAddr, u16ForeignPort, destAddr);
    
    SetBlock(FALSE);
    if(connect(m_streamFd, (struct sockaddr *)&destAddr, sizeof(destAddr)) == -1)
    {
        tm.tv_sec = CONNECT_TIME_OUT_TIME;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(m_streamFd, &set);
        if(select(m_streamFd+1, NULL, &set, NULL, &tm) > 0)
        {
            i32Len = sizeof(int32);
            getsockopt(m_streamFd, SOL_SOCKET, SO_ERROR, &i32Error, (socklen_t *)&i32Len);
            if(i32Error == 0)
            {
                i32Ret = 0;
            }
            else
            {
                i32Ret = -1;
            }
        } 
        else
        {
            i32Ret = -1;
        }
    }    
    SetBlock(TRUE);
    
    return i32Ret;
}

int32 CCommunicatingSocket::Send(const void *pBuf, uint32 u32BufLen) 
{
    return send(m_streamFd, (raw_type *) pBuf, u32BufLen, 0);
}

int32 CCommunicatingSocket::Recv(void *pBuf, uint32 u32BufLen)
{
    return recv(m_streamFd, (raw_type *) pBuf, u32BufLen, 0);
}

int32 CCommunicatingSocket::GetForeignAddr(string& strForeignAddr)
{
    int32 i32Ret;
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    i32Ret = getpeername(m_streamFd, (sockaddr *) &sockAddr,(socklen_t *) &u32AddrLen);
    strForeignAddr = inet_ntoa(sockAddr.sin_addr);
    
    return i32Ret;
}

uint16 CCommunicatingSocket::GetForeignPort(uint16 *pu16ForeignPort)
{
    int32 i32Ret;
    sockaddr_in sockAddr;
    uint32 u32AddrLen = sizeof(sockAddr);

    i32Ret = getpeername(m_streamFd, (sockaddr *) &sockAddr, (socklen_t *) &u32AddrLen);
    *pu16ForeignPort = ntohs(sockAddr.sin_port);

    return i32Ret;
}

void CCommunicatingSocket::SetForeignAddr(const string& strForeignAddr)
{
    m_strForeignAddr = strForeignAddr;
    
    return;
}

void CCommunicatingSocket::SetForeignPort(uint16 u16ForeignPort)
{
    m_u16ForeignPort = u16ForeignPort;
    return;
}

//CTcpClientSocket
CTcpClientSocket::CTcpClientSocket() 
    : CCommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) 
{    
    SetAliveDetect(20, 5, 3);
}

CTcpClientSocket::CTcpClientSocket(uint32 i32NewSocket) 
    : CCommunicatingSocket(i32NewSocket) 
{    
    SetAliveDetect(20, 5, 3);
}
_enResult CTcpClientSocket::ResetFd()
{
	int32 i32Flag = 1;

    //关闭现有的FD
    CloseFd();

    //生产新的FD
    SOCK_OPEN(m_streamFd, AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置属性
    setsockopt(m_streamFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
    SetAliveDetect(20, 5, 3);
    
    //连接对方
    if(!m_strForeignAddr.empty() && m_u16ForeignPort != 0)
    {
        Connect(m_strForeignAddr, m_u16ForeignPort);
    }
}

//CTcpServerSocket
CTcpServerSocket::CTcpServerSocket(uint16 u16LocalPort, uint32 u32QueueLen) 
    : CSocket(SOCK_STREAM, IPPROTO_TCP) 
{
    m_strLocalAddr = "";
    m_u16LocalPort = u16LocalPort;
    m_u32QueueLen = u32QueueLen;
    
    SetLocalPort(u16LocalPort);
    Listen(u32QueueLen);
}

CTcpServerSocket::CTcpServerSocket(const string &strLocalAddress, uint16 u16LocalPort, uint32 u32QueueLen) 
    : CSocket(SOCK_STREAM, IPPROTO_TCP) 
{
    m_strLocalAddr = strLocalAddress;
    m_u16LocalPort = u16LocalPort;
    m_u32QueueLen = u32QueueLen;

    SetLocalAddrAndPort(strLocalAddress, u16LocalPort);
    Listen(u32QueueLen);
}

_enResult CTcpServerSocket::ResetFd()
{
	int32 i32Flag = 1;

    //关闭现有的FD
    CloseFd();
 
    //生产新的FD
    SOCK_OPEN(m_streamFd, AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置属性
    setsockopt(m_streamFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
    
    if(m_strLocalAddr.length() == 0)
    {        
        SetLocalPort(m_u16LocalPort);
    }
    else
    {
        SetLocalAddrAndPort(m_strLocalAddr, m_u16LocalPort);
    }

    //侦听
    Listen(m_u32QueueLen);
}

int32 CTcpServerSocket::Listen(uint32 u32QueueLen)
{
    return listen(m_streamFd, u32QueueLen);
}

CTcpClientSocket* CTcpServerSocket::Accept(string& strForeignAddr)
{
    uint32 i32NewSocket;
    CTcpClientSocket* pclTcpClientSocket;    
    struct sockaddr_in stAddrFrom;
    uint32 u32AddrFromLen = 0;

    memset(&stAddrFrom, 0, sizeof(stAddrFrom));
    u32AddrFromLen = sizeof(stAddrFrom);

    i32NewSocket = accept(m_streamFd, (struct sockaddr*)&stAddrFrom, &u32AddrFromLen);
    if(i32NewSocket == INVALID_SOCKET)
    {
        return NULL;
    }
    strForeignAddr = inet_ntoa(stAddrFrom.sin_addr);    
    NEW(pclTcpClientSocket, CTcpClientSocket, i32NewSocket);    
    
    return pclTcpClientSocket;
}

//CUdpSocket
CUdpSocket::CUdpSocket()
    : CCommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) 
{
    m_strLocalAddr = "";
    m_u16LocalPort = 0;

    SetBroadcast();
}

CUdpSocket::CUdpSocket(uint16 u16LocalPort)
    : CCommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
    m_strLocalAddr = "";
    m_u16LocalPort = u16LocalPort;

    SetLocalPort(u16LocalPort);
    SetBroadcast();
}

CUdpSocket::CUdpSocket(const string &strLocalAddress, uint16 u16LocalPort) 
    : CCommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
    m_strLocalAddr = strLocalAddress;
    m_u16LocalPort = u16LocalPort;

    SetLocalAddrAndPort(strLocalAddress, u16LocalPort);
    SetBroadcast();
}

_enResult CUdpSocket::ResetFd()
{
	int32 i32Flag = 1;

    //关闭现有的FD
    CloseFd();

    //生产新的FD
    SOCK_OPEN(m_streamFd, AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //设置属性
    setsockopt(m_streamFd, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));
    
    if((m_strLocalAddr.length() != 0) && (m_u16LocalPort != 0))
    {        
        SetLocalAddrAndPort(m_strLocalAddr, m_u16LocalPort);
    }
    else if(m_u16LocalPort != 0)
    {
        SetLocalPort(m_u16LocalPort);
    }    
    SetBroadcast();
}

int32 CUdpSocket::SetBroadcast() 
{
    uint32 broadcastPermission = 1;

    return setsockopt(m_streamFd, SOL_SOCKET, SO_BROADCAST, 
             (raw_type *) &broadcastPermission, sizeof(broadcastPermission));
}

int32 CUdpSocket::Disconnect()
{
    sockaddr_in sockNullAddr;

    memset(&sockNullAddr, 0, sizeof(sockNullAddr));
    sockNullAddr.sin_family = AF_UNSPEC;

    return connect(m_streamFd, (sockaddr *) &sockNullAddr, sizeof(sockNullAddr));
}

int32 CUdpSocket::SendTo(const void *pBuf, uint32 u32BufLen, 
    const string &strForeignAddr, uint16 u16ForeignPort)
{
    sockaddr_in destAddr;

    FillAddr(strForeignAddr, u16ForeignPort, destAddr);
    return sendto(m_streamFd, (raw_type *) pBuf, u32BufLen, 0,
        (sockaddr *) &destAddr, sizeof(destAddr));
}

int32 CUdpSocket::RecvFrom(void *pBuf, uint32 u32BufLen, 
    string &strSourceAddress, uint16 u16SourcePort)
{    
    int32 i32Ret;
    sockaddr_in clntAddr;
    socklen_t addrLen = sizeof(clntAddr);

    i32Ret = recvfrom(m_streamFd, (raw_type *) pBuf, u32BufLen, 
        0, (sockaddr *) &clntAddr, (socklen_t *) &addrLen);
    strSourceAddress = inet_ntoa(clntAddr.sin_addr);
    u16SourcePort = ntohs(clntAddr.sin_port);

    return i32Ret;
}

int32 CUdpSocket::SetMulticastTTL(uint8 u8MulticaseTTL)
{
   return setsockopt(m_streamFd, IPPROTO_IP, IP_MULTICAST_TTL,
    (raw_type *) &u8MulticaseTTL, sizeof(u8MulticaseTTL));
}

int32 CUdpSocket::JoinGroup(const string &strMulticastGroup)
{
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(strMulticastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  return setsockopt(m_streamFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
            (raw_type *) &multicastRequest, sizeof(multicastRequest));
}

int32 CUdpSocket::LeaveGroup(const string &strMulticastGroup)
{
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(strMulticastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  return setsockopt(m_streamFd, IPPROTO_IP, IP_DROP_MEMBERSHIP,     
            (raw_type *) &multicastRequest, sizeof(multicastRequest));
}

