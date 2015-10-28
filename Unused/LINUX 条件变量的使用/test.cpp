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
        sleep(2);

        pthread_mutex_lock(&s_mutexId);
        
        pthread_cond_signal(&s_condId);
        s_i32Count--;
        
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

        //在未得到唤醒信号前，本线程将一直阻塞在这里，并解锁
        //在收到信号后，本线程将重新争锁，此时若锁被其它线程占用，则将一直处于争锁状态而阻塞于此
        pthread_cond_wait(&s_condId, &s_mutexId);

        //得到唤醒信号后，在while中判断条件是否满足
        
        sleep(1);
    }

    pthread_mutex_unlock(&s_mutexId);

    OM_DEBUG("Task2 exit\n");

    return NULL;
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

