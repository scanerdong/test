/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Socket.h
 * @version : 1.0.0
 * @brief : ����socketͨ���豸��
 *   
 * @author : zhangx
 * @date : 2011��3��28�� 17:12:22
 * @history : 
 *   
 */

#ifndef SOCKET_H
#define SOCKET_H

/************************���ò���*****************************/
#include "Om.h"
#include "Stream.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/
//CSocket
class CSocket : PUBLIC CStream
{
PUBLIC:
  ~CSocket();

    string GetLocalAddr();

    uint16 GetLocalPort();

    int32 SetLocalPort(uint16 u16LocalPort);

    int32 SetLocalAddrAndPort(const string &strLocalAddress, uint16 u16LocalPort = 0);  
    
    _enResult SetBlock(_enBool bBlocked);
    
    _enResult SetAliveDetect(int32 i32Idle, int32 i32Interval, int32 i32Count);    
    
    virtual void CloseFd();
    
PROTECTED:
    CSocket(uint32 u32Type, uint32 u32Protocol);

    CSocket(int32 m_i32Socket);

PRIVATE:
    CSocket(const CSocket &clSock);
  
    void operator=(const CSocket &clSock);
};

//CCommunicatingSocket
class CCommunicatingSocket : PUBLIC CSocket 
{
PUBLIC:
    int32 Connect(const string &strForeignAddr, uint16 u16ForeignPort);

    int32 Send(const void *pBuf, uint32 u32Buffer);

    int32 Recv(void *pBuf, uint32 u32Buffer);

    int32 GetForeignAddr(string& strForeignAddr);

    uint16 GetForeignPort(uint16 *pu16ForeignPort);

    void SetForeignAddr(const string& strForeignAddr);

    void SetForeignPort(uint16 u16ForeignPort);
    
PROTECTED:
    CCommunicatingSocket(uint32 u32Type, uint32 u32Protocol);

    CCommunicatingSocket(uint32 i32NewSocket);
    
    string m_strForeignAddr;
    
    uint16 m_u16ForeignPort;
};

//CTcpClientSocket
class CTcpClientSocket : PUBLIC CCommunicatingSocket 
{
PUBLIC:
    CTcpClientSocket();
    
    _enResult ResetFd();

PRIVATE:
    // CTcpServerSocket�������Ӻ�����CTcpClientSocket������˶���Ϊ��Ԫ��
    friend class CTcpServerSocket;
  
    CTcpClientSocket(uint32 i32NewSocket);
};

//CTcpServerSocket
class CTcpServerSocket : PUBLIC CSocket //����Ҫ�̳���CSocket����Ϊ���������������
{
PUBLIC:
    CTcpServerSocket(uint16 u16LocalPort, uint32 u32QueueLen = 5);

    CTcpServerSocket(const string &strLocalAddress, uint16 u16LocalPort, uint32 u32QueueLen = 5);
    
    _enResult ResetFd();

    CTcpClientSocket* Accept(string& strForeignAddr);

PRIVATE:    
    //Ϊ����ʵ����CTcpServerSocket�࣬����ʵ�������յ�˽��Send��Recv�ӿ�
    virtual int32 Send(const void *pBuf, uint32 u32BufLen){}
    
    virtual int32 Recv(void *pBuf, uint32 u32BufLen){}

    int32 Listen(uint32 u32QueueLen);

    string m_strLocalAddr;
    
    uint16 m_u16LocalPort;

    uint32 m_u32QueueLen;
};

//CUdpSocket
class CUdpSocket : PUBLIC CCommunicatingSocket
{
PUBLIC:
    CUdpSocket();

    CUdpSocket(uint16 u16LocalPort);

    CUdpSocket(const string &strLocalAddress, uint16 u16LocalPort);
    
    _enResult ResetFd();

    int32 Disconnect();

    int32 SendTo(const void *pBuf, uint32 u32BufLen, const string &strForeignAddr, uint16 u16ForeignPort);

    int32 RecvFrom(void *pBuf, uint32 u32BufLen, string &sSourceAddress, uint16 u16SourcePort);

    int32 SetMulticastTTL(uint8 u8MulticaseTTL);

    int32 JoinGroup(const string &strMulticastGroup);

    int32 LeaveGroup(const string &strMulticastGroup);

PRIVATE:
    int32 SetBroadcast();
    
    string m_strLocalAddr;
    
    uint16 m_u16LocalPort;
};


#endif

