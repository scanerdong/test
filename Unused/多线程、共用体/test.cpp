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

static int32 s_i32Count = 5;
//static pthread_attr_t s_attr;//线程属性，在创建线程中用到的
static pthread_mutex_t s_mutexId;//互斥锁结构体
//static pthread_mutexattr_t s_muteattr; //互斥锁的属性结构体
static pthread_cond_t s_condId;  //条件变量结构体
//static pthread_condattr_t s_condAttr;//条件属性结构体
pthread_t* pTid;

//pthread_mutexattr_t互斥锁的属性，设置好属性后在创建互斥锁时用到，可设置4种类型的互斥锁
//int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
/*
pthread_mutexattr_settype 语法
#include <pthread.h>
int pthread_mutexattr_settype(pthread_mutexattr_t *attr , int type);
类型属性的缺省值为 PTHREAD_MUTEX_DEFAULT。
type参数指定互斥锁的类型。以下列出了有效的互斥锁类型：
PTHREAD_MUTEX_NORMAL 描述: 此类型的互斥锁不会检测死锁。如果线程在不首先解除互斥锁的情况下尝试重新锁定该互斥锁，则会产生死锁。尝试解除由其他线程锁定的互斥锁会产生不确定的行为。如果尝试解除未锁定的互斥锁，则会产生不确定的行为。
PTHREAD_MUTEX_ERRORCHECK 描述: 此类型的互斥锁可提供错误检查。如果线程在不首先解除锁定互斥锁的情况下尝试重新锁定该互斥锁，则会返回错误。如果线程尝试解除锁定的互斥锁已经由其他线程锁定，则会返回错误。如果线程尝试解除未锁定的互斥锁，则会返回错误。
PTHREAD_MUTEX_RECURSIVE 描述: 如果线程在不首先解除锁定互斥锁的情况下尝试重新锁定该互斥锁，则可成功锁定该互斥锁。 与 PTHREAD_MUTEX_NORMAL 类型的互斥锁不同，对此类型互斥锁进行重新锁定时不会产生死锁情况。多次锁定互斥锁需要进行相同次数的解除锁定才可以释放该锁，然后其他线程才能获取该互斥锁。如果线程尝试解除锁定的互斥锁已经由其他线程锁定，则会返回错误。 如果线程尝试解除未锁定的互斥锁，则会返回错误。
PTHREAD_MUTEX_DEFAULT 描述: 如果尝试以递归方式锁定此类型的互斥锁，则会产生不确定的行为。对于不是由调用线程锁定的此类型互斥锁，如果尝试对它解除锁定，则会产生不确定的行为。对于尚未锁定的此类型互斥锁，如果尝试对它解除锁定，也会产生不确定的行为。允许在实现中将该互斥锁映射到其他互斥锁类型之一。对于 Solaris 线程，PTHREAD_PROCESS_DEFAULT 会映射到 PTHREAD_PROCESS_NORMAL。
*/
//上述描述说明互斥锁只能被本线程上锁，本线程解除；
//唤醒线程2
void* ThreadTask1(void* pArg)
{
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;
    int32 i32Ret;

    //分离的线程不能再主程序里用pthread_join
    //pthread_detach(pthread_self());
    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        OM_DEBUG("Send signal to task2\n");

        //使线程2先跑起来，得到锁
        sleep(1); //sleep是为了将两次得锁时间间隔开，以免task2一直不能得到锁
        pthread_mutex_lock(&s_mutexId);

        if(s_i32Count>0)
        {
            s_i32Count--; //消费，再通知
        }
        //如果没有线程被阻塞在条件变量上，那么调用pthread_cond_signal()将没有作用，即出现唤醒丢失
        pthread_cond_signal(&s_condId);//由1个原子操作：发送信号;发送信号后，Task2的cond_wait被激活，于是Task2醒来并争锁，但由于此时Task1还没释放锁，Task2还处于争锁阻塞中
        pthread_mutex_unlock(&s_mutexId); //Task1释放锁，则Task2争锁成功，Task2将可以执行pthread_cond_wait之后的内容
        
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
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;

    //pthread_detach(pthread_self());

    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        pthread_mutex_lock(&s_mutexId);
        OM_DEBUG("before receive signal, s_i32Count=%d\n", s_i32Count);
        //pthread_cond_wait先执行两个原子操作  1.解锁; 2.挂起（在未得到唤醒信号前，本线程将一直阻塞在这里）
        
        //3.在收到信号后，本线程将重新争锁，此时若锁被其它线程占用，则将一直处于争锁状态而阻塞于此
        pthread_cond_wait(&s_condId, &s_mutexId); //s_condId只要使用后就失效，需要pthread_cond_signal函数重新给条件后才会生效

        if(s_i32Count == 0)
        {
            s_i32Count++;
        }

        OM_DEBUG("after receive signal, s_i32Count=%d\n", s_i32Count);

        //得到唤醒信号后，在while中判断条件是否满足
        pthread_mutex_unlock(&s_mutexId);
    }

    OM_DEBUG("Task2 exit\n");

    return NULL;
}

int32 main()
{
    pthread_t threadId1,threadId2;
    int32 i32Ret;
    uint8 u8LoopCount = 20;
    
    pTid = &threadId2;

    //初始条件
    pthread_mutex_init(&s_mutexId, NULL);//锁结构，锁属性结构，属性为空时与变量赋静态结构初值PTHREAD_MUTEX_INITIALIZER一样的效果
    pthread_cond_init(&s_condId, NULL);//条件结构，条件属性结构，属性为空时与静态结构PTHREAD_COND_INITIALIZER一样的效果
    
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
/*
    while(1)
    {
        sleep(5);
    }
*/    
    //一个线程不能被多个线程等待，也就是说对一个线程只程只能调用一次pthread_join，否则只有一个能正确返回，其他的将返回ESRCH 错误。
    char* pchRet;
    pthread_join(threadId2, (void**)&pchRet);
    printf("*pi32ThreadRet:%s\n", pchRet);//pchRet可以接收线程返回值，这里线程中返回值是NULL，所以得到NULL
    pthread_join(threadId1, (void**)&pchRet);
    printf("*pi32ThreadRet:%s\n", pchRet);
    return 0;
}

