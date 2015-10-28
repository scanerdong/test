//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Socket.h"
#include "Singleton.h"
#include <iostream>

uint8 g_u8PrintCtrl = 0xff;

int getPeerMacbySocketFd( int sockfd, char *buf, char* localethname ) 
{ 
    int ret =0; 
    struct arpreq arpreq; 
    struct sockaddr_in dstadd_in; 
    socklen_t  len = sizeof( struct sockaddr_in ); 
    memset( &arpreq, 0, sizeof( struct arpreq )); 
    memset( &dstadd_in, 0, sizeof( struct sockaddr_in ));  
    if( getpeername( sockfd, (struct sockaddr*)&dstadd_in, &len ) < 0 ) 
    { 
        perror( "get peer name wrong, %s/n"); 
        return -1; 
    } 
    else 
    { 
        memcpy( ( struct sockaddr_in * )&arpreq.arp_pa, ( struct sockaddr_in * )&dstadd_in, sizeof( struct sockaddr_in )); 
        strcpy(arpreq.arp_dev, localethname); 
        arpreq.arp_pa.sa_family = AF_INET; 
        arpreq.arp_ha.sa_family = AF_UNSPEC; 
        if( ioctl( sockfd, SIOCGARP, &arpreq ) < 0 ) 
        { 
            perror( "ioctl SIOCGARP wrong"); 
            return -1; 
        } 
        else 
        { 
            unsigned char* ptr = (unsigned char *)arpreq.arp_ha.sa_data; 
            sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)); 
        } 
     } 
     return ret; 
} 
#if 0
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
    char achBuf[100] = {1,2,3,4,5,6,7,8};
    
    bzero(&stTimeout, sizeof(stTimeout));
    pclTcpServerSocket = new CTcpServerSocket(9507);
    
    pclTcpClientSocket = pclTcpServerSocket->Accept(strForeignAddr);

    getPeerMacbySocketFd(pclTcpClientSocket->GetFd(), (char*)au8RecvBuf, "eth1");
    OM_DEBUG("peer mac addr %s\n", (char*)au8RecvBuf);
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

while(1)
{
    sleep(1);
    pclTcpClientSocket->Send(achBuf, 10);
    achBuf[9]++;
}
OM_DEBUG("server exit\n");
}
#endif

#include "MutexMgr.h"
#include "MemMgr.h"
#include "FdResMgr.h"

int32 main()
{
    /*
    int socket_fd;
            struct sockaddr_in my_addr,user_addr;
            int so_broadcast=1;          
            my_addr.sin_family=AF_INET;
                my_addr.sin_port=htons(3914);//SERVPORT
                my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
                bzero(&(my_addr.sin_zero), 8 );           

                user_addr.sin_family=AF_INET;
                user_addr.sin_port=htons(9650);//CLIENTPORT
                user_addr.sin_addr.s_addr=inet_addr("192.168.192.2");//255.255.255.255
                bzero(&(user_addr.sin_zero), 8 );
                if((socket_fd=(socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)))==-1) 
                        {
                    printf("socket:%s:%dn", strerror( errno ), errno );
                    exit(1);
                }
                setsockopt(socket_fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));
                if((bind(socket_fd,(struct sockaddr *)&my_addr, sizeof(struct sockaddr)))==-1) 
                        {
                        printf("bind:%s:%dn", strerror( errno ), errno );
                        exit(1);
                   }            

                char buf[8]={3, 0, 4, 192, 168, 1, 101, 0};          
                while( 1 )
               {             //sleep 1 second
                sleep( 1 );            //send broadcast message to inform client the address of server                
                if( sendto(socket_fd,buf, 7 ,0,(struct sockaddr *)&user_addr,sizeof(user_addr) ) == -1)
                {
                        printf("sendto():%s:%dn", strerror( errno ), errno );
                }       
                }
*/

    TSingleton<CMutexMgr>::Instance()->Init();    
    TSingleton<CMemMgr>::Instance()->Init();
    TSingleton<CFdResMgr>::Instance()->Init();

    char buf[8]={3, 0, 4, 192, 168, 1, 101, 0};       
    CUdpSocket clUdpSendSocket;
    clUdpSendSocket.SetLocalPort(55555);
    string strForeignAddr = "255.255.255.255";



    while( 1 )
    {   
        sleep(1);//sleep 1 second
        //clUdpSendSocket.SendTo(buf, 7, strForeignAddr, 55556);
        
        uint8 au8Value[4] = {0x11, 0x22, 0x33, 0x44};
        uint32 u32Value;
        union
        {
            struct  
            {
                uint8 b2L:2;
                uint8 b2M:2;
                uint8 b4H:4;
                
                uint8 b2L1:2;
                uint8 b2M1:2;
                uint8 b4H1:4;
            }stTest;

            uint16 u8Test;
        }unTest;

        unTest.u8Test = 0;
        unTest.stTest.b4H = 0x5;
        unTest.stTest.b4H1 = 0x4;

        memcpy(&u32Value, au8Value, 4);
        printf("%#x\n, %#x\n", u32Value, unTest.u8Test);
    }

}
