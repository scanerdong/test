//ģ��һ�������ߡ������ߣ������߷����Ʒ��֪ͨ������ȡ��Ʒ��������ȡ��Ʒ�ӹ����֪ͨ�����߷Ų�Ʒ
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

//�����߳�2
void* ThreadTask1(void* pArg)
{
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;
    int32 i32Ret;

    //������̲߳���������������pthread_join
    pthread_detach(pthread_self());

    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        OM_DEBUG("Send signal to task2\n");

        //ʹ�߳�2�����������õ���
        sleep(1); //sleep��Ϊ�˽����ε���ʱ������������task2һֱ���ܵõ���
        
        pthread_mutex_lock(&s_mutexId);
        s_i32Count--; //���ѣ���֪ͨ
        pthread_cond_signal(&s_condId);//������ԭ�Ӳ�����1,����;2,�����ź�;
        pthread_mutex_unlock(&s_mutexId);
        
        i32Ret = pthread_kill(*pTid, 0);
        if(i32Ret != 0)
        {
            OM_DEBUG("task2 has exit\n");
            //��pthread_exit(NULL);ͬ
            return NULL;
        }
        
    }

    OM_DEBUG("Task1 exit\n");

    return NULL;
}

//�����Ȼ���
void* ThreadTask2(void* pArg)
{
    pthread_detach(pthread_self());

    pthread_mutex_lock(&s_mutexId);
    
    while(s_i32Count > 0)
    {

        OM_DEBUG("before receive signal, s_i32Count=%d\n", s_i32Count);
        //pthread_cond_wait��ִ������ԭ�Ӳ���  1.����; 2.������δ�õ������ź�ǰ�����߳̽�һֱ���������
        
        //3.���յ��źź󣬱��߳̽�������������ʱ�����������߳�ռ�ã���һֱ��������״̬�������ڴ�
        pthread_cond_wait(&s_condId, &s_mutexId); //s_condIdֻҪʹ�ú��ʧЧ����Ҫpthread_cond_signal�������¸�������Ż���Ч

        OM_DEBUG("after receive signal, s_i32Count=%d\n", s_i32Count);

        //�õ������źź���while���ж������Ƿ�����
        sleep(1);
    }

    pthread_mutex_unlock(&s_mutexId);

    OM_DEBUG("Task2 exit\n");

    return NULL;
}

//ɾ��һ��·��
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

    //�������߳�
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

