//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

int main()
{
    CTcpServerSocket* pclTcpServerSocket;
    CTcpClientSocket* pclTcpClientSocket;
    string strForeignAddr;
	int8 ai8RecvBuf[1024];
	int32 i32RecvLen = 0;
    
    pclTcpServerSocket = new CTcpServerSocket(6000);
    
    pclTcpClientSocket = pclTcpServerSocket->Accept(strForeignAddr);

    while(1)
    {
        sleep(3);
        i32RecvLen = pclTcpClientSocket->Recv(ai8RecvBuf, 1024);  
        
        if(i32RecvLen > 0)
            PRINT_BUF(ai8RecvBuf, i32RecvLen);
        else
            printf("recv error!\n");

        printf("%s",ai8RecvBuf);
    }
}

