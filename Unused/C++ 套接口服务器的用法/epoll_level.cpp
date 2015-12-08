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

#define MAX_RECV_FD 10
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

    //���������׽ӿ�
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


    //��listen()��ķ������׽ӿڼ��뵽epoll�
    int epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(-1);
    }
    struct epoll_event event, events[MAX_RECV_FD];
    event.events = EPOLLIN;
    event.data.fd = sock;
    int epctl_ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &event);
    if(epctl_ret == -1){
        perror("epoll_ctl");
        exit(-1);
    }

    int nfds,n,conn_sock;
    struct sockaddr_in stAddrFrom;
    unsigned int u32AddrFromLen = 0;
    memset(&stAddrFrom, 0, sizeof(stAddrFrom));
    u32AddrFromLen = sizeof(stAddrFrom);

    
    sigset_t sigblockset;
    sigemptyset(&sigblockset);
    sigaddset(&sigblockset, SIGALRM);

    //����һ��40���źţ���ppoll�����ڼ���kill -s 40 [pid]��������40�źţ��ᵼ��ppoll���������һ���յ��ܶ�40�ź�
    //��ֻ���յ�һ��SIGALRM�źţ��μ�http://linux.die.net/man/7/signal "Real-time signals"
    sigaddset(&sigblockset, 40);

    for (;;) {
        //nfds = epoll_wait(epollfd, events, MAX_RECV_FD, -1);
        nfds = epoll_pwait(epollfd, events, MAX_RECV_FD, -1, &sigblockset);
        if (nfds == -1) {
            perror("epoll_pwait");
            exit(-1);
        }
    
       for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == sock) {
                conn_sock = accept(sock,
                                (struct sockaddr *) &stAddrFrom, &u32AddrFromLen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                string addr = inet_ntoa(stAddrFrom.sin_addr);
                cout << "the client addr is: " << addr << endl;    
                //setnonblocking(conn_sock);
                event.events = EPOLLIN;
                event.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                            &event) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else {
                /* We have data on the fd waiting to be read. Read and
                    display it. We must read whatever data is available
                    completely, as we are running in edge-triggered mode
                    and won't get a notification again for the same
                    data. */
                 ssize_t count;
                 char buf[512];
            
                 count = read (events[n].data.fd, buf, sizeof(buf));
                 if (count == 0 || count == -1) // 0,peer closed -1,error
                   {
                     perror ("read");
                     printf ("Closed connection on descriptor %d\n",
                             events[n].data.fd);
                     
                     /* Closing the descriptor will make epoll remove it
                        from the set of descriptors which are monitored. */
                     close (events[n].data.fd);
                   }else{
                     /* Write the buffer to standard output */
                     int s = write (1, buf, count);
                     if (s == -1)
                       {
                         perror ("write");
                         abort ();
                       }
                   }
            }
        }
    }
        
    //�������ٽ���close()
    close(epollfd);

    return 0;  
}  


