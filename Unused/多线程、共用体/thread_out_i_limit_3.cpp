#include<pthread.h>   
#include<unistd.h>   
#include<stdio.h>   
#include<stdlib.h>   
   
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;/*��ʼ��������*/  
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;//init cond   
   
void *thread1(void*);   
void *thread2(void*);   
   
int i = 1; //global   
   
int main(void){   
    pthread_t t_a;   
    pthread_t t_b;//two thread   
   
    pthread_create(&t_a,NULL,thread2,(void*)NULL);   
    pthread_create(&t_b,NULL,thread1,(void*)NULL);//Create thread   
   
    pthread_join(t_b,NULL);//wait a_b thread end   
    pthread_mutex_destroy(&mutex);   
    pthread_cond_destroy(&cond);   
    exit(0);   
}   
   
void *thread1(void *junk){   
    for(i = 1;i<= 9; i++){   
        pthread_mutex_lock(&mutex); //������   
        printf("call thread1 \n");   
        if(i%3 == 0)   
            pthread_cond_signal(&cond); //send sianal to t_b   
        else  
            printf("thread1: %d\n",i);   
        pthread_mutex_unlock(&mutex);   
        sleep(1);   
    }   
}   
   
void *thread2(void*junk){   
    while(i < 9)   
    {   
        pthread_mutex_lock(&mutex);   
        printf("call thread2 \n");   
        if(i%3 != 0)   
            pthread_cond_wait(&cond,&mutex); //wait   
        printf("thread2: %d\n",i);   
        pthread_mutex_unlock(&mutex);   
        sleep(1);   
    }   
}                  


/*
ʾ���Ľ��ͣ� 
call thread2�����߳�2��t_b������������ pthread_mutex_lock(&mutex);��ס��mutexʹ�ô˽���ִ���߳�2�е��ٽ����Ĵ��룬��ִ�е�45�У�if(i%3 != 0)����ʱi=1,�������������ִ��46�У� pthread_cond_wait(&cond,&mutex); ����ǹؼ���pthread_cond_wait(&cond,&mutex)��������������ԭ�Ӳ�������һ �������Ƚ��֮ǰ��pthread_mutex_lock������mutex���ڶ� �����������ڵȴ����������ߣ����߳�2����ֱ���ٴα����ѣ����ѵ���������pthread_cond_signal(&cond);������cond�ź������ѡ� 

call thread1:����pthread_cond_wait�Ѿ����߳�2��������ʱ������߳�ֻ���߳�1����ô�߳�1��mutex����������ʱ�ж���̣߳���ô�̼߳�������˳��Ͳ���ϵͳ�йء� 

thread1: 1���߳�1������ִ���ٽ����Ĵ��룬��ִ�е�if(i%3 == 0)��ʱi=1,��������������pthread_cond_signal(&cond);����ִ�У���ô�߳�2�Դ��ڹ���״̬�����thread1: 1���߳�1��pthread_mutex_unlock(&mutex);������ 

thread1: 2����ʱ�˽�����ֻ��2���̣߳��߳�2���ڹ���״̬����ôֻ���߳�1,���߳�1�ֶ�mutex��������ʱͬ��ִ���ٽ����Ĵ��룬����i=2,������������pthread_cond_signal(&cond);����ִ�У���ô�߳�2�Դ��ڹ���״̬�����thread1: 1���߳�1��pthread_mutex_unlock(&mutex);������ 

call thread1��ͬ�����߳�1����������ʱi=3,��������pthread_cond_signal(&cond)��ִ�У���ôpthread_cond_signal(&cond)�ᷢ���źţ������Ѵ��ڹ�����߳�2��pthread_cond_signalͬ��������ԭ�Ӳ�����1,����;2,�����ź�;���������߳�1�����������mutex�������������źţ������ȴ�signal������߳�2�����źţ����ѹ�����߳�2�� 

thread2: 3������pthread_cond_signal�������߳�2,��i=3����������pthread_cond_wait(&cond,&mutex);��ִ�У���ôpthread_cond_wait(&cond,&mutex)��ʱҲ��һ������������;�����߳�2��������ʱ��pthread_cond_wait(&cond,&mutex)�Ĳ����൱��pthread_mutex_lock(&mutex);��ô�߳�2����ִ����������ٽ����Ĵ��룬����pthread_mutex_unlock(&mutex);���߳�2���н����� 

ʣ�µ����ԭ���������͵�һ���� 


�ݹ�pthread_cond_wait��������ⲻ��֮��������һ���򵥵�wait������������Ӧ����һ�庯������ͬ�ĺ����ڲ�ͬ��������ִ�У����pthread_cond_wait�Ļ��ƿ��Ժܺõ�ѧϰ���������� 
*/