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


using namespace std;

uint8 g_u8PrintCtrl = 7;

static int32 s_i32Count = 5;
static pthread_mutex_t s_mutexId;
static pthread_cond_t s_condId;
//static pthread_condattr_t s_condAttr;
pthread_t* pTid;

//唤醒线程2
void* ThreadTask1(void* pArg)
{
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;
    int32 i32Ret;

    //分离的线程不能再主程序里用pthread_join
    pthread_detach(pthread_self());

    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        OM_DEBUG("Send signal to task2\n");

        //使线程2先跑起来，得到锁
        sleep(1); //sleep是为了将两次得锁时间间隔开，以免task2一直不能得到锁
        
        pthread_mutex_lock(&s_mutexId);
        s_i32Count--; //消费，再通知
        pthread_cond_signal(&s_condId);//由两个原子操作：1,解锁;2,发送信号;
        pthread_mutex_unlock(&s_mutexId);
        
        i32Ret = pthread_kill(*pTid, 0);
        if(i32Ret != 0)
        {
            OM_DEBUG("task2 has exit\n");
            //与pthread_exit(NULL);同
            return NULL;
        }
        
    }

    OM_DEBUG("Task1 exit\n");

    return NULL;
}

//阻塞等唤醒
void* ThreadTask2(void* pArg)
{
    pthread_detach(pthread_self());

    pthread_mutex_lock(&s_mutexId);
    
    while(s_i32Count > 0)
    {

        OM_DEBUG("before receive signal, s_i32Count=%d\n", s_i32Count);
        //pthread_cond_wait先执行两个原子操作  1.解锁; 2.挂起（在未得到唤醒信号前，本线程将一直阻塞在这里）
        
        //3.在收到信号后，本线程将重新争锁，此时若锁被其它线程占用，则将一直处于争锁状态而阻塞于此
        pthread_cond_wait(&s_condId, &s_mutexId); //s_condId只要使用后就失效，需要pthread_cond_signal函数重新给条件后才会生效

        OM_DEBUG("after receive signal, s_i32Count=%d\n", s_i32Count);

        //得到唤醒信号后，在while中判断条件是否满足
        sleep(1);
    }

    pthread_mutex_unlock(&s_mutexId);

    OM_DEBUG("Task2 exit\n");

    return NULL;
}

//删除一条路由
int32 setRoute(char* pchEthName, char* pchRtAddr, _enBool bIsAdd)
{
    struct	rtentry rtent;
    struct	sockaddr_in	*p;
    int32 i32Socket; 
    uint32 u32RtAddr = inet_addr(pchRtAddr);

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

    if(bIsAdd == TRUE)
    {
        if ( ioctl(i32Socket,SIOCADDRT,&rtent) == -1 )
        {
            return -1;
        }
        else
        {
            OM_INFO("getway has been set\n");
        }
    }
    else
    {
        if ( ioctl(i32Socket,SIOCDELRT,&rtent) == -1 )
        {
            return -1;
        }
        else
        {
            OM_INFO("getway has been delete\n");
        }
    }
    close(i32Socket);
    return 0;
}

int32 main()
{
    pthread_t threadId1,threadId2;
    int32 i32Ret;
    uint8 u8LoopCount = 20;
    
    pTid = &threadId2;

    //建两个线程
    i32Ret = pthread_create(&threadId1, NULL, ThreadTask1, (void*)&u8LoopCount);
    if(i32Ret != 0)
    {
        OM_ERROR("Create ThreadTask1 error\n");
    }

    i32Ret = pthread_create(&threadId2, NULL, ThreadTask2, (void*)&u8LoopCount);
    if(i32Ret != 0)
    {
        OM_ERROR("Create ThreadTask2 error\n");
    }

    while(1)
    {
        sleep(5);
    }

    return 0;
}

