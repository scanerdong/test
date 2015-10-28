//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
#include "Test.h"

#include <poll.h>

using namespace std;

typedef struct pollfd POLLFD;

const int32 i32PollFdCount = 1;

CTcpClient::CTcpClient(CTcpClientSocket* pclTcpClientSocket)
{
    m_pclSocket = pclTcpClientSocket;
}

CTcpClient::~CTcpClient()
{
    delete m_pclSocket;
    
}

CTcpClientSocket* CTcpClient::GetSocket()
{
    return m_pclSocket;
}

void CTcpClient::OnRead()
{
    return ;
}

void CTcpClient::OnWrite()
{
	int8 au8RecvBuf[1024] = "this is a test";
	int32 i32SendLen = 0;

    OM_INFO("in CTcpClient::OnWrite\n");
    i32SendLen = m_pclSocket->Send(au8RecvBuf, strlen((char*)au8RecvBuf));  
    
    switch(i32SendLen)
    {
        case -1:
            printf("send error!\n");
            //m_pclSocket->CloseFd();
    
            //重连
            //i32ReConnectRet = m_pclSocket->Connect("127.0.0.1",6000);
            //cout << i32ReConnectRet << endl;
            printf("Connect err no %d \n", errno);
            perror("Connect");
            break;
            
        case 0:
            printf("no data to be send, err no: %d\n", errno);
            perror("Send");
            break;
            
        default:
            OM_INFO("The send msg is: %s\n", au8RecvBuf);
            break;
    }
}

int main()
{
    CTcpClientSocket* pclTcpClientSocket = new CTcpClientSocket();
    CTcpClient* pclTcpClient = new CTcpClient(pclTcpClientSocket);
    
    int32 i32PollRet = 0;
    struct pollfd* pstPollFds;
    char achErrInfo[100] = {0};
    
    if(-1 == pclTcpClient->GetSocket()->Connect("127.0.0.1",6000))
    {
        sprintf(achErrInfo, "In Connect, the errno is %d", errno);
        perror(achErrInfo);

        return -1;
    }
    
    pstPollFds = new POLLFD[i32PollFdCount];
    pstPollFds->fd = pclTcpClient->GetSocket()->GetFd();
    pstPollFds->events = POLLIN | POLLOUT | POLLERR;

    for(;;)
    {
        i32PollRet = poll(pstPollFds, i32PollFdCount, 10);

        switch(i32PollRet)
        {
            case -1:
                perror("poll");
                break;

            case 0:
                OM_INFO("no fd changed\n");
                break;

            default:
                OM_INFO("have fd changed\n");
                for(int i32I = 0; i32I < i32PollFdCount; i32I++)
                {
                    if((pstPollFds->revents & POLLOUT) == POLLOUT)
                    {
                        pclTcpClient->OnWrite();
                    }

                    if((pstPollFds->revents & POLLIN) == POLLIN)
                    {
                        OM_INFO("have fd readable\n");
                    }

                    if((pstPollFds->revents & POLLERR) == POLLERR)
                    {
                        OM_INFO("have fd ERR\n");
                        OM_ERROR("the errno is %d", errno);
                        perror("POLLERR");

                        return -1;
                    }
                }
                
                break;
        }
        
        sleep(3);
    }
}
