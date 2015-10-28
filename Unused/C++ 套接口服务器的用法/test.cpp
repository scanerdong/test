//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
#include <iostream>

uint8 g_u8PrintCtrl = 0xff;

int main()
{
    CTcpServerSocket* pclTcpServerSocket;
    CTcpClientSocket* pclTcpClientSocket;
    string strForeignAddr;
	uint8 au8RecvBuf[1024];
	int32 i32RecvLen = 0;
    fd_set fdsReadable;
    struct timeval stTimeout;
    int32 i32FdsReadableCount = 0;
    char achBuf[100] = {0};
    
    bzero(&stTimeout, sizeof(stTimeout));
    pclTcpServerSocket = new CTcpServerSocket(6000);
    
    pclTcpClientSocket = pclTcpServerSocket->Accept(strForeignAddr);

    FD_SET(pclTcpClientSocket->GetFd(), &fdsReadable);
    
    while(1)
    {
        sleep(3);
        
        select(pclTcpClientSocket->GetFd() + 1, &fdsReadable, NULL, NULL, &stTimeout);

        i32FdsReadableCount = FD_ISSET(pclTcpClientSocket->GetFd(), &fdsReadable);
        cout << i32FdsReadableCount << endl;
        if(i32FdsReadableCount > 0)
        {
        
            i32RecvLen = pclTcpClientSocket->Recv(au8RecvBuf, 1024);  
            
            if((i32RecvLen <= 0) && (errno != EINTR || errno != EWOULDBLOCK || errno != EAGAIN))//连接断开
            {
                sprintf(achBuf, "pclTcpClientSocket->Recv--errno is %d", errno);
                perror(achBuf);        
                return -1;
            }
            
            switch(i32RecvLen)
            {
                case -1:
                    printf("receive error no is %d", errno);
                    perror("receive");
                    pclTcpClientSocket->CloseFd();
                    break;

                case 0:
                    printf("receive len is %d \n", i32RecvLen);
                    break;

                default:
                    OM_INFO("receive msg: %s", au8RecvBuf);
                    break;
            }
        }
    }

}

