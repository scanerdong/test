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
#include <unistd.h>
#include <fcntl.h>

#define MAX_RECV_FD 10


int setNonBlocking(int fd)
{
    int flags, s;
    
    //�õ��ļ�״̬��־
    flags = fcntl (fd, F_GETFL, 0);
    if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }
    
    //�����ļ�״̬��־
    flags |= O_NONBLOCK;
    s = fcntl (fd, F_SETFL, flags);
    if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }
    
    return 0;

}

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

    //���������׽ӿ����óɷ�����
    setNonBlocking(sock);
    
    //��listen()��ķ������׽ӿڼ��뵽epoll�
    int epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(-1);
    }
    struct epoll_event event, events[MAX_RECV_FD];
    event.events = EPOLLIN | EPOLLET;
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
        
           //��������
           if ((events[n].events & EPOLLERR) ||
                (events[n].events & EPOLLHUP) ||
                (!(events[n].events & EPOLLIN)))
           {
             perror("events");
             close (events[n].data.fd);
             continue;
           }


           //��listen���׽ӿڣ���ôѭ���������е�����
            if (events[n].data.fd == sock) {
                while(1)
                {
                    conn_sock = accept(sock,
                                    (struct sockaddr *) &stAddrFrom, &u32AddrFromLen);
                    if (conn_sock == -1) {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                          {
                            /* ���еȴ����ӱ��������ˣ��˳�*/
                            break;
                          }
                        else
                          {
                            perror ("accept");
                            break;
                          }
                    }
                    string addr = inet_ntoa(stAddrFrom.sin_addr);
                    cout << "the client addr is: " << addr << endl;    
                    setNonBlocking(conn_sock);
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = conn_sock;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                                &event) == -1) {
                        perror("epoll_ctl: conn_sock");
                        exit(EXIT_FAILURE);
                    }
                }
                continue;
                
            } else {
                //�������׽ӿ�
                int done = 0;
				ssize_t total = 0;
				char buf[512];
				ssize_t count;
                while(1)
                {
                     count = read (events[n].data.fd, buf+total, sizeof(buf)-total);
                     if(count == -1)
                     {
                         /* If errno == EAGAIN, that means we have read all
                            data. So go back to the main loop. */
                         if (errno != EAGAIN)
                         {
                           perror ("read -1");
                           done = 1;
                         }
                         else
                         {
                            perror("read EAGAIN");//֤�������Ѿ�����
                            //�������
                            write (1, buf, total);
                         }
                         break;
                     }else if (count == 0) // 0,peer closed
                     {
                         /* End of file. The remote has closed the
                            connection. */
                         perror ("read 0");
                         done = 1;
                         break;
                     }else{
                       /* Write the buffer to standard output */
					   /*
                       int s = write (1, buf, count);
                       if (s == -1)
                         {
                           perror ("write");
                           abort ();
                         }
                         */
                       total += count;
                     }
                }

                if (done)
                {
                  printf ("Closed connection on descriptor %d\n",
                          events[n].data.fd);
                
                  /* Closing the descriptor will make epoll remove it
                     from the set of descriptors which are monitored. */
                  close (events[n].data.fd);
                }
            }
        }
    }
        
    //�������ٽ���close()
    close(epollfd);

    return 0;  
}  


