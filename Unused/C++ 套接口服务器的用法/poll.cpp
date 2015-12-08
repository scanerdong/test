#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

#include <signal.h>
#include <sys/time.h>

void procAlarmSig(int sigNo)
{
    if(SIGALRM == sigNo)
        printf("receive alarm signal\n");
}

void procUserDefSig(int sigNo)
{
    if(40 == sigNo)
        printf("receive NO.40 signal\n");
}

#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/epoll.h>
#include <poll.h>

int main(int argc, char* argv[])  
{
    struct itimerval stITimerVal;
    int i32SleepRet;
    
    //����������ض�ʱ��
    signal(SIGALRM, procAlarmSig);
    stITimerVal.it_value.tv_sec = 1;
    stITimerVal.it_value.tv_usec = 0;
    stITimerVal.it_interval = stITimerVal.it_value;
    setitimer(ITIMER_REAL, &stITimerVal, NULL);

    //ע��һ��RT�ź� (35<n<64) 
    signal(40, procUserDefSig);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);     
    int i32Flag = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i32Flag, sizeof(i32Flag));

    sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(6000);
    bind(sock, (sockaddr *) &localAddr, sizeof(sockaddr_in));

    listen(sock, 5);

    //poll
    struct pollfd fds[1];
    memset(fds, sizeof(fds), 0x0);
    fds[0].fd = sock;
    fds[0].events = POLLIN;

#ifdef POLL_TEST
    int ret = poll(fds, 1, 10000/*ms*/);

#else
    struct timespec timeout;
    timeout.tv_sec = 60; //s
    timeout.tv_nsec = 0; //ns

    //�鿴ϵͳĿǰ��������Щ�ź�
    sigset_t testset;
    sigemptyset(&testset);
    sigprocmask(0,NULL,&testset);
    if(sigisemptyset(&testset) == 1)
    {
        printf("before ignore SIGALRM, the default set is empty\n");
    }
    
    //���ε�SIGALRM�źţ������д˲���ppoll��ֱ�ӱ���ϲ�����-1��
    sigset_t sigblockset;
    sigemptyset(&sigblockset);
    sigaddset(&sigblockset, SIGALRM);

    //����һ��40���źţ���ppoll�����ڼ���kill -s 40 [pid]��������40�źţ��ᵼ��ppoll���������һ���յ��ܶ�40�ź�
    //��ֻ���յ�һ��SIGALRM�źţ��μ�http://linux.die.net/man/7/signal "Real-time signals"
    sigaddset(&sigblockset, 40);
    int ret = ppoll(fds, 1,
                   &timeout, &sigblockset); //���һ������ΪNULL�Ļ�����pollһ��
#endif

    switch(ret)
    {
        case 0:
            printf("wait client over time\n");
            break;
        case -1:
            perror("after listen");
            break;

        default:
            if(fds[0].revents&POLLIN)
            {
                printf("now, i can accept client without blocking\n");
            }
            break;
    }
    
    int i32NewSocket;
    struct sockaddr_in stAddrFrom;
    unsigned int u32AddrFromLen = 0;

    memset(&stAddrFrom, 0, sizeof(stAddrFrom));
    u32AddrFromLen = sizeof(stAddrFrom);

    i32NewSocket = accept(sock, (struct sockaddr*)&stAddrFrom, &u32AddrFromLen);
    if(i32NewSocket == -1)
    {
        return NULL;
    }
    string strForeignAddr = inet_ntoa(stAddrFrom.sin_addr);    

    cout << "the client addr is: " << strForeignAddr << endl;    

    i32SleepRet = 6;
    while(1) 
    {
        i32SleepRet = sleep(i32SleepRet);
        if(i32SleepRet != 0)
        {
            printf("sleep has been breaked, remain %d\n", i32SleepRet);
        }else break;
    }

    return 0;  
}  


