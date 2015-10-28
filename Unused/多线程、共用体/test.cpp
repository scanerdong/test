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

static int32 s_i32Count = 5;
//static pthread_attr_t s_attr;//�߳����ԣ��ڴ����߳����õ���
static pthread_mutex_t s_mutexId;//�������ṹ��
//static pthread_mutexattr_t s_muteattr; //�����������Խṹ��
static pthread_cond_t s_condId;  //���������ṹ��
//static pthread_condattr_t s_condAttr;//�������Խṹ��
pthread_t* pTid;

//pthread_mutexattr_t�����������ԣ����ú����Ժ��ڴ���������ʱ�õ���������4�����͵Ļ�����
//int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
/*
pthread_mutexattr_settype �﷨
#include <pthread.h>
int pthread_mutexattr_settype(pthread_mutexattr_t *attr , int type);
�������Ե�ȱʡֵΪ PTHREAD_MUTEX_DEFAULT��
type����ָ�������������͡������г�����Ч�Ļ��������ͣ�
PTHREAD_MUTEX_NORMAL ����: �����͵Ļ���������������������߳��ڲ����Ƚ��������������³������������û����������������������Խ���������߳������Ļ������������ȷ������Ϊ��������Խ��δ�����Ļ���������������ȷ������Ϊ��
PTHREAD_MUTEX_ERRORCHECK ����: �����͵Ļ��������ṩ�����顣����߳��ڲ����Ƚ������������������³������������û���������᷵�ش�������̳߳��Խ�������Ļ������Ѿ��������߳���������᷵�ش�������̳߳��Խ��δ�����Ļ���������᷵�ش���
PTHREAD_MUTEX_RECURSIVE ����: ����߳��ڲ����Ƚ������������������³������������û���������ɳɹ������û������� �� PTHREAD_MUTEX_NORMAL ���͵Ļ�������ͬ���Դ����ͻ�����������������ʱ�������������������������������Ҫ������ͬ�����Ľ�������ſ����ͷŸ�����Ȼ�������̲߳��ܻ�ȡ�û�����������̳߳��Խ�������Ļ������Ѿ��������߳���������᷵�ش��� ����̳߳��Խ��δ�����Ļ���������᷵�ش���
PTHREAD_MUTEX_DEFAULT ����: ��������Եݹ鷽ʽ���������͵Ļ���������������ȷ������Ϊ�����ڲ����ɵ����߳������Ĵ����ͻ�������������Զ��������������������ȷ������Ϊ��������δ�����Ĵ����ͻ�������������Զ������������Ҳ�������ȷ������Ϊ��������ʵ���н��û�����ӳ�䵽��������������֮һ������ Solaris �̣߳�PTHREAD_PROCESS_DEFAULT ��ӳ�䵽 PTHREAD_PROCESS_NORMAL��
*/
//��������˵��������ֻ�ܱ����߳����������߳̽����
//�����߳�2
void* ThreadTask1(void* pArg)
{
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;
    int32 i32Ret;

    //������̲߳���������������pthread_join
    //pthread_detach(pthread_self());
    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        OM_DEBUG("Send signal to task2\n");

        //ʹ�߳�2�����������õ���
        sleep(1); //sleep��Ϊ�˽����ε���ʱ������������task2һֱ���ܵõ���
        pthread_mutex_lock(&s_mutexId);

        if(s_i32Count>0)
        {
            s_i32Count--; //���ѣ���֪ͨ
        }
        //���û���̱߳����������������ϣ���ô����pthread_cond_signal()��û�����ã������ֻ��Ѷ�ʧ
        pthread_cond_signal(&s_condId);//��1��ԭ�Ӳ����������ź�;�����źź�Task2��cond_wait���������Task2�����������������ڴ�ʱTask1��û�ͷ�����Task2����������������
        pthread_mutex_unlock(&s_mutexId); //Task1�ͷ�������Task2�����ɹ���Task2������ִ��pthread_cond_wait֮�������
        
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
    uint8 u8Count = *(uint8*)pArg;
    uint8 u8Loop;

    //pthread_detach(pthread_self());

    for(u8Loop = 0; u8Loop < u8Count; u8Loop++)
    {
        pthread_mutex_lock(&s_mutexId);
        OM_DEBUG("before receive signal, s_i32Count=%d\n", s_i32Count);
        //pthread_cond_wait��ִ������ԭ�Ӳ���  1.����; 2.������δ�õ������ź�ǰ�����߳̽�һֱ���������
        
        //3.���յ��źź󣬱��߳̽�������������ʱ�����������߳�ռ�ã���һֱ��������״̬�������ڴ�
        pthread_cond_wait(&s_condId, &s_mutexId); //s_condIdֻҪʹ�ú��ʧЧ����Ҫpthread_cond_signal�������¸�������Ż���Ч

        if(s_i32Count == 0)
        {
            s_i32Count++;
        }

        OM_DEBUG("after receive signal, s_i32Count=%d\n", s_i32Count);

        //�õ������źź���while���ж������Ƿ�����
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

    //��ʼ����
    pthread_mutex_init(&s_mutexId, NULL);//���ṹ�������Խṹ������Ϊ��ʱ���������̬�ṹ��ֵPTHREAD_MUTEX_INITIALIZERһ����Ч��
    pthread_cond_init(&s_condId, NULL);//�����ṹ���������Խṹ������Ϊ��ʱ�뾲̬�ṹPTHREAD_COND_INITIALIZERһ����Ч��
    
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
/*
    while(1)
    {
        sleep(5);
    }
*/    
    //һ���̲߳��ܱ�����̵߳ȴ���Ҳ����˵��һ���߳�ֻ��ֻ�ܵ���һ��pthread_join������ֻ��һ������ȷ���أ������Ľ�����ESRCH ����
    char* pchRet;
    pthread_join(threadId2, (void**)&pchRet);
    printf("*pi32ThreadRet:%s\n", pchRet);//pchRet���Խ����̷߳���ֵ�������߳��з���ֵ��NULL�����Եõ�NULL
    pthread_join(threadId1, (void**)&pchRet);
    printf("*pi32ThreadRet:%s\n", pchRet);
    return 0;
}

