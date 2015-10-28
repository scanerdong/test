//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

using namespace std;

int main()
{
    CTcpClientSocket* pclTcpClientSocket;
    string strForeignAddr;
	int8 ai8RecvBuf[1024] = "this is a test ....-------a\n";
	int32 i32SendLen = 0;
    char achBuf[PRINT_BUF_LEN];
    
    pclTcpClientSocket = new CTcpClientSocket();
    
    pclTcpClientSocket->Connect("127.0.0.1",6000);
    i32SendLen = pclTcpClientSocket->Send(ai8RecvBuf, strlen((char*)ai8RecvBuf));  
    
    if(i32SendLen > 0)
        PRINT_BUF(ai8RecvBuf, i32SendLen);
    else
        printf("recv error!\n");

    //fprintf(stderr, "/bin/sh not available \n");
    //system("ls -l");
}
