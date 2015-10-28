//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <string>
#include "Socket.h"
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

using namespace std;
#define  PEER_IP_LENGTH      16
#define  PEER_STRING_LENGTH   256

//  char ipaddr[PEER_IP_LENGTH];
//  char string_addr[PEER_STRING_LENGTH];

int getpeer_information(int fd, char* ipaddr, char* string_addr) //fd, peer ip, host ip
 {
     struct sockaddr_in name;
     size_t namelen = sizeof(name);
     struct hostent* result;

     assert(fd >= 0);
     assert(ipaddr != NULL);
     assert(string_addr != NULL);

     /*
      * Clear the memory.
      */
     memset(ipaddr, '\0', PEER_IP_LENGTH);
     memset(string_addr, '\0', PEER_STRING_LENGTH);
 
    if (getpeername(fd, (struct sockaddr *)&name, &namelen) != 0) {
         return -1;
     } else {
         strcpy(ipaddr,
            inet_ntoa(*(struct in_addr *)&name.sin_addr.s_addr)
            );
        }
 
     result = gethostbyaddr((char *)&name.sin_addr.s_addr, 4, AF_INET);
     if (result) {
         strcpy(string_addr, result->h_name);
         return 0;
     } else {
         return -1;
     }
  }

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


int main()
{
    CTcpClientSocket* pclTcpClientSocket;
    string strForeignAddr;
	int8 ai8RecvBuf[1024] = "this is a test ....-------a\n";
	int32 i32SendLen = 0;
    int32 i32RecvLen;
    char achBuf[PRINT_BUF_LEN];
    int32 i32Result;
    
    pclTcpClientSocket = new CTcpClientSocket();
    
    i32Result = pclTcpClientSocket->Connect("192.168.192.210",30000);
    if(i32Result < 0)
    {
        OM_ERROR("connect error\n");
        perror("connect:");
    }
    char achPeerIp[16], achHostIp[16];
    if(0 == getpeer_information(pclTcpClientSocket->GetFd(), achPeerIp, achHostIp))
    {
        OM_DEBUG("peer ip %s, host ip %s\n", achPeerIp, achHostIp);
    }

    getPeerMacbySocketFd( pclTcpClientSocket->GetFd(), achBuf, "eth1");
    OM_DEBUG("peer mac is %s\n", achBuf);
    
    while(1)
    {
        sleep(1);
            
        //i32SendLen = pclTcpClientSocket->Send(ai8RecvBuf, strlen((char*)ai8RecvBuf));  

        i32RecvLen = pclTcpClientSocket->Recv(ai8RecvBuf, 20);  

        if(i32RecvLen > 0)
            PRINT_BUF(ai8RecvBuf, i32RecvLen);
        else
        {
            perror("Recv:");
            OM_DEBUG("error number:%d, i32RecvLen:%d\n", errno,i32RecvLen);
            
        }
    }

    //fprintf(stderr, "/bin/sh not available \n");
    //system("ls -l");
}
