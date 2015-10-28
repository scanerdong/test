//模拟一个生产者、消费者，生产者放入产品后，通知消费者取产品，消费者取产品加工完后，通知生产者放产品
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "test.h"
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/route.h>


using namespace std;

uint8 g_u8PrintCtrl = 7;

int32 fnDevGetEthInfo(const char *pch_eth_name,_stEthInfo *pstEthInfo)
{
    int32 i32Sock;

    struct  sockaddr_in st_addr_in;
    struct  ifreq stIfreq;

    if (pch_eth_name == NULL || pstEthInfo == NULL)
        return DEV_INVALID_PARAM;

    /*创建一个新的SOCKET*/
    i32Sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (i32Sock == -1) {
        perror("fnDevGetEthInfo, socket error!\n");
        return FAIL;
    }

    strncpy(stIfreq.ifr_name, pch_eth_name, IFNAMSIZ);
    stIfreq.ifr_name[IFNAMSIZ - 1]   =   0;

    /* 读取IP地址 */
    if (0 > ioctl(i32Sock, SIOCGIFADDR, &stIfreq)) {
        close(i32Sock);
        perror("fnDevGetEthInfo, acquire ip addr error!\n");
        return FAIL;
    }

    memcpy(&st_addr_in, &stIfreq.ifr_addr, sizeof(st_addr_in));
    pstEthInfo->stIpAddr.s_addr = st_addr_in.sin_addr.s_addr;

    /* 获取MAC地址 */
    if (0 > ioctl(i32Sock,  SIOCGIFHWADDR, &stIfreq))
    {
        close(i32Sock);
        perror("fnDevGetEthInfo, acquire mac addr error!\n");
        return -2;
    }
    memset(pstEthInfo->achMac,0,6);
    memcpy(pstEthInfo->achMac,stIfreq.ifr_hwaddr.sa_data,6);

    /*获取子网掩码*/
    if (0 > ioctl(i32Sock,SIOCGIFNETMASK,&stIfreq))
    {
        close(i32Sock);
        perror("fnDevGetEthInfo, acquire netmask error!\n");
        return -3;
    }
    memcpy(&st_addr_in, &stIfreq.ifr_netmask, sizeof(st_addr_in));
    pstEthInfo->stNetMask.s_addr = st_addr_in.sin_addr.s_addr;

    /*获取MTU*/
    if (0 > ioctl(i32Sock,SIOCGIFMTU,&stIfreq))
    {
        close(i32Sock);
        perror("fnDevGetEthInfo, acquire MTU error!\n");
        return -4;
    }
    pstEthInfo->i32Mtu = stIfreq.ifr_mtu;
    OM_INFO("mtu is %d\n",pstEthInfo->i32Mtu);

    /*获取广播地址*/
    if (0 > ioctl(i32Sock,SIOCGIFBRDADDR,&stIfreq))
    {
        close(i32Sock);
        perror("获取mtu时出错!\n");
        return -5;
    }
    memcpy(&st_addr_in, &stIfreq.ifr_broadaddr, sizeof(st_addr_in));
    pstEthInfo->stBroadcastAddr.s_addr = st_addr_in.sin_addr.s_addr;

    close(i32Sock);
    return 0;
}

static int32 fnDevClearEthFlag(int32 i32Fd, char *pchIfName, int16 i16Flag)
{
    struct ifreq stIfr;

    if (i32Fd < 0 || pchIfName == NULL)
        return FAIL;

    strncpy(stIfr.ifr_name, pchIfName, IFNAMSIZ);
    if (ioctl(i32Fd, SIOCGIFFLAGS, &stIfr) < 0)
    {
        OM_ERROR("get network card failed\n");
        return -2;
    }

    strncpy(stIfr.ifr_name, pchIfName, IFNAMSIZ);
    stIfr.ifr_flags &= ~i16Flag;

    if (ioctl(i32Fd, SIOCSIFFLAGS, &stIfr) < 0)
    {
        OM_ERROR("set network card information failed\n");
        return -3;
    }

    return 0;
}

static int32 fnDevSetEthInfoImp(int32 i32Fd, const char *pchIfName, _stEthInfo *pstEthInfo)
{
    struct  sockaddr_in st_addr_in;
    struct  sockaddr st_addr;
    struct  ifreq stIfreq;

    if (i32Fd < 0 || pchIfName == NULL || pstEthInfo == NULL)
        return FAIL;

    strncpy(stIfreq.ifr_name, pchIfName, IFNAMSIZ);
    stIfreq.ifr_name[IFNAMSIZ - 1] = 0;
    st_addr_in.sin_family = PF_INET;

    /* 设置IP地址 */
    st_addr_in.sin_addr.s_addr = pstEthInfo->stIpAddr.s_addr;
    memcpy(&stIfreq.ifr_addr, &st_addr_in, sizeof(st_addr_in));

    if (0 > ioctl(i32Fd, SIOCSIFADDR, &stIfreq))
    {
        OM_ERROR("设置IP地址时出错!\n");
        return -2;
    }

#if 0
    /*设置MTU*/
    stIfreq.ifr_mtu = pstEthInfo->i32Mtu;
    if (0 > ioctl(i32Fd, SIOCSIFMTU, &stIfreq))
    {
        OM_ERROR("设置MTU时出错!\n");
        return -2;
    }
#endif

    /* 设置子网掩码 */
    st_addr_in.sin_family = PF_INET;
    st_addr_in.sin_addr.s_addr = pstEthInfo->stNetMask.s_addr;
    memcpy(&stIfreq.ifr_netmask, &st_addr_in, sizeof(st_addr_in));

    if (0 > ioctl(i32Fd, SIOCSIFNETMASK, &stIfreq))
    {
        OM_ERROR("设置子网掩码时出错!\n");
        return -3;
    }

    //设置广播地址
    st_addr_in.sin_family = PF_INET;
    st_addr_in.sin_addr.s_addr = pstEthInfo->stBroadcastAddr.s_addr;
    memcpy(&stIfreq.ifr_broadaddr, &st_addr_in, sizeof(st_addr_in));
    
    if (0 > ioctl(i32Fd, SIOCSIFBRDADDR, &stIfreq))
    {
        OM_ERROR("设置广播地址时出错!\n");
        return -5;
    }

    /*设置MAC地址*/
    if(pstEthInfo->achMac[0] == 0 && pstEthInfo->achMac[1] == 0 && pstEthInfo->achMac[2] == 0
         && pstEthInfo->achMac[3] == 0 && pstEthInfo->achMac[4] == 0 && pstEthInfo->achMac[5] == 0)
    {
        return 0;
    }
    
    st_addr.sa_family = 1;
    memcpy((char *)&st_addr.sa_data, (char*)pstEthInfo->achMac, 6);

    memcpy((char *) &stIfreq.ifr_hwaddr, (char *) &st_addr,sizeof(struct sockaddr));

    if (0 > ioctl (i32Fd,  SIOCSIFHWADDR, &stIfreq))
    {
        OM_ERROR("设置MAC地址时出错!\n");
        return -4;
    }

    return 0;
}
static int32 fnDevSetEthFlag(int32 i32Fd, char *pchIfName, int16 i16Flag)
{
    struct ifreq stIfr;

    if (i32Fd < 0 || pchIfName == NULL)
        return FAIL;

    strncpy(stIfr.ifr_name, pchIfName, IFNAMSIZ);

    if (ioctl(i32Fd, SIOCGIFFLAGS, &stIfr) < 0)
    {
        OM_ERROR("获取网卡状态失败!\n");
        return -2;
    }

    strncpy(stIfr.ifr_name, pchIfName, IFNAMSIZ);
    stIfr.ifr_flags |= i16Flag;

    if (ioctl(i32Fd, SIOCSIFFLAGS, &stIfr) < 0)
    {
        OM_ERROR("设置网卡状态失败!\n");
        return -3;
    }

    return 0;
}

int setDefaultRoute(char* pchEthName, char* route_addr)
{
    struct	rtentry rtent;
    struct	sockaddr_in	*p;
    int32 i32Socket; 
    uint32 u32RtAddr = inet_addr(route_addr);

    memset(&rtent,0,sizeof(struct rtentry));
    //destination address
    p = (struct sockaddr_in *)&rtent.rt_dst;
    p->sin_family =	AF_INET;
    p->sin_addr.s_addr = 0;
    //gateway
    p = (struct sockaddr_in *)&rtent.rt_gateway;
    p->sin_family =	AF_INET;
    memcpy(&p->sin_addr.s_addr,&u32RtAddr,4);
    //netmask
    p = (struct sockaddr_in *)&rtent.rt_genmask;
    p->sin_family =	AF_INET;
    p->sin_addr.s_addr = 0;
    //others
    rtent.rt_dev = pchEthName;
    rtent.rt_metric	= 5;
    rtent.rt_window	= 0;
    rtent.rt_flags = RTF_UP|RTF_GATEWAY;

    i32Socket = socket(AF_INET, SOCK_DGRAM, 0);

    if ( ioctl(i32Socket,SIOCADDRT,&rtent) == -1 )
    {
#if 0
        if ( errno == ENETUNREACH )    /* possibly gateway is over the bridge */
        {                            /* try adding a route to gateway first */
            memset(&rtent,0,sizeof(struct rtentry));
            p = (struct sockaddr_in *)&rtent.rt_dst;
            p->sin_family =	AF_INET;
            memcpy(&p->sin_addr.s_addr,&u32RtAddr,4);
            p = (struct sockaddr_in *)&rtent.rt_gateway;
            p->sin_family =	AF_INET;
            p->sin_addr.s_addr = 0;
            p = (struct sockaddr_in *)&rtent.rt_genmask;
            p->sin_family =	AF_INET;
            p->sin_addr.s_addr = 0xffffffff;
            rtent.rt_dev		=	pchEthName;
            rtent.rt_metric     =	  1;
            rtent.rt_flags      =	  RTF_UP|RTF_HOST;

            //桥接到地址为u32RtAddr的机器上
            if ( ioctl(i32Socket,SIOCADDRT,&rtent) == 0 )
            {
                memset(&rtent,0,sizeof(struct rtentry));
                p =	(struct sockaddr_in *)&rtent.rt_dst;
                p->sin_family	=	AF_INET;
                p->sin_addr.s_addr	=	0;
                p			=	(struct sockaddr_in *)&rtent.rt_gateway;
                p->sin_family	=	AF_INET;
                memcpy(&p->sin_addr.s_addr,&u32RtAddr,4);
                p			=	(struct sockaddr_in *)&rtent.rt_genmask;
                p->sin_family	=	AF_INET;
                p->sin_addr.s_addr	=	0;
                rtent.rt_dev	=	pchEthName;
                rtent.rt_metric	=	1;
                rtent.rt_window	=	0;
                rtent.rt_flags	=	RTF_UP|RTF_GATEWAY;
                if ( ioctl(i32Socket,SIOCADDRT,&rtent) == -1 )
                {
                    OM_ERROR("dhcpConfig: ioctl SIOCADDRT\n");
                    perror("dhcpConfig");
                    close(i32Socket);
                    return -1;
                }
            }
        }
        else
        {
            OM_ERROR("dhcpConfig: ioctl SIOCADDRT, error no is: %d\n", errno);
            perror("dhcpConfig");
            close(i32Socket);
            return -1;
        }
#endif
        return -1;
    }
    else
    {
        OM_INFO("default getway has been set\n");
    }
    
    close(i32Socket);
    return 0;
}

int32 fnDevSetEthInfo(const char *pch_eth_name, _stEthInfo *pstEthInfo)
{
    int32 i32Sock;
    int32 i32Result = 0;

    if (pch_eth_name == NULL || pstEthInfo == NULL)
        return DEV_INVALID_PARAM;

    /*创建一个新的SOCKET*/
    i32Sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (i32Sock == -1)
    {
        OM_ERROR("初始化网卡信息出错!\n");
        return FAIL;
    }

    /*关闭网卡*/
    fnDevClearEthFlag(i32Sock, (char*)pch_eth_name, IFF_UP);

    /*设置网卡信息*/
    i32Result=fnDevSetEthInfoImp(i32Sock,pch_eth_name,pstEthInfo);

    /*打开网卡*/
    fnDevSetEthFlag(i32Sock, (char*)pch_eth_name,(IFF_UP | IFF_RUNNING));

    close(i32Sock);

    return i32Result;
}

int32 main()
{
    _stEthInfo stEthInfo;
    fnDevGetEthInfo("eth1", &stEthInfo);
    
    OM_INFO("get the eth1 ip addr: %s\n", inet_ntoa(stEthInfo.stIpAddr));
    OM_INFO("broadcast addr:%s\n", inet_ntoa(stEthInfo.stBroadcastAddr));
    OM_INFO("netmask: %s\n", inet_ntoa(stEthInfo.stNetMask));

    setDefaultRoute("eth1", "192.168.192.1");

    uint32 u32A = 0x12345678;
    char achB[4] = {0};
    memcpy(achB, &u32A, 4);
    OM_DEBUG("achB[0]:%#x\n", achB[0]);
        
    return 0;
}
