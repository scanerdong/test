#include "Common.h"

class CSocket
{
PUBLIC:
    virtual ~CSocket();

	CSocket();

	int32 GetFd(){return m_i32SockFd;}

    int32 SetLocalPort(uint16 u16LocalPort);
    
    _enResult SetAliveDetect(int32 i32Idle, int32 i32Interval, int32 i32Count);    
	
     void CloseFd(void );
	 
PROTECTED:
    CSocket(uint32 u32Type, uint32 u32Protocol);

    CSocket(int32 m_i32Socket);
	
PROTECTED:
	int32 m_i32SockFd;

PRIVATE:
    //CSocket(const CSocket &clSock);
  
    void operator=(const CSocket &clSock);
};

class CTcpClientSocket : PUBLIC CSocket
{
PUBLIC:
    CTcpClientSocket();
	
	int32 Connect(const string &strForeignAddr, uint16 u16ForeignPort);

	int32 Send(const void *pBuf, uint32 u32Buffer);

	int32 Recv(void *pBuf, uint32 u32Buffer);

	int32 GetForeignAddress(string& strForeignAddr);

	uint16 GetForeignPort(uint16& u16ForeignPort);
	
PRIVATE:
    // CTcpServerSocket接受连接后生成CTcpClientSocket对象，因此定义为友元。
    friend class CTcpServerSocket;
  
    CTcpClientSocket(int32 i32NewSocket);
};

//CTcpServerSocket
class CTcpServerSocket : PUBLIC CSocket //仅需要继承至CSocket，因为仅需用其接受连接
{
PUBLIC:
    CTcpServerSocket(uint16 u16LocalPort, uint32 u32QueueLen = 5);

    CTcpClientSocket* Accept(string& strForeignAddr);

PRIVATE:    
    int32 Listen(uint32 u32QueueLen);

    string m_strLocalAddress;
    
    uint16 m_u16LocalPort;

    uint32 m_u32QueueLen;
};

