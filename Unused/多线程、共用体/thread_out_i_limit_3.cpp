#include<pthread.h>   
#include<unistd.h>   
#include<stdio.h>   
#include<stdlib.h>   
   
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;/*初始化互斥锁*/  
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
        pthread_mutex_lock(&mutex); //互斥锁   
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
示例的解释： 
call thread2：是线程2即t_b首先上锁，即 pthread_mutex_lock(&mutex);锁住了mutex使得此进程执行线程2中的临界区的代码，当执行到45行：if(i%3 != 0)，此时i=1,满足此条件，则执行46行： pthread_cond_wait(&cond,&mutex); 这句是关键，pthread_cond_wait(&cond,&mutex)操作有两步，是原子操作：第一 解锁，先解除之前的pthread_mutex_lock锁定的mutex；第二 挂起，阻塞并在等待对列里休眠，即线程2挂起，直到再次被唤醒，唤醒的条件是由pthread_cond_signal(&cond);发出的cond信号来唤醒。 

call thread1:由于pthread_cond_wait已经对线程2解锁，此时另外的线程只有线程1，那么线程1对mutex上锁，若这时有多个线程，那么线程间上锁的顺序和操作系统有关。 

thread1: 1：线程1上锁后执行临界区的代码，当执行到if(i%3 == 0)此时i=1,不满足条件，则pthread_cond_signal(&cond);不被执行，那么线程2仍处于挂起状态，输出thread1: 1后线程1由pthread_mutex_unlock(&mutex);解锁。 

thread1: 2：这时此进程中只有2个线程，线程2处于挂起状态，那么只有线程1,则线程1又对mutex上锁，此时同样执行临界区的代码，而且i=2,不满足条件，pthread_cond_signal(&cond);不被执行，那么线程2仍处于挂起状态，输出thread1: 1后线程1由pthread_mutex_unlock(&mutex);解锁。 

call thread1：同样由线程1上锁，但此时i=3,满足条件pthread_cond_signal(&cond)被执行，那么pthread_cond_signal(&cond)会发出信号，来唤醒处于挂起的线程2。pthread_cond_signal同样由两个原子操作：1,解锁;2,发送信号;解锁即对线程1解锁，解除对mutex的上锁。发送信号，即给等待signal挂起的线程2发送信号，唤醒挂起的线程2。 

thread2: 3：由于pthread_cond_signal唤醒了线程2,即i=3满足条件，pthread_cond_wait(&cond,&mutex);被执行，那么pthread_cond_wait(&cond,&mutex)此时也有一步操作：上锁;即对线程2上锁，此时的pthread_cond_wait(&cond,&mutex)的操作相当与pthread_mutex_lock(&mutex);那么线程2继续执行上锁后的临界区的代码，并由pthread_mutex_unlock(&mutex);对线程2进行解锁。 

剩下的输出原理和上面解释的一样。 


纵观pthread_cond_wait，它的理解不可之把它看作一个简单的wait函数，它里面应该是一族函数，不同的函数在不同的条件下执行，理解pthread_cond_wait的机制可以很好的学习条件变量。 
*/