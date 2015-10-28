#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;

/*
bool __sync_bool_compare_and_swap(type* ptr, type oldval, type newval)函数
其中type为 1, 2, 4 or 8 字节的int或uint
该函数的作用是进行原子(线程安全)的比较和交换，
如果ptr里的值为oldval，则将其变成newval，并返回true;
如果ptr里的值不为oldval，则返回false
*/

//0:none lock, 1:locked
static int lock = 0;
#define cas_lock(lock) do{\
while(!__sync_bool_compare_and_swap(lock, 0, 1)){\ //死循环，直到lock里的值被改为1为止
sched_yield();\
}\
}while(0);

#define cas_unlock(lock) do{\
*lock = 0; \ //将lock里的值重新改为0，表示重新赋值
}while(0);

void *test_func(void *arg)
{
        int i=0;
        for(i=0;i<20000;++i){
               // __sync_fetch_and_add(&count,1);
               cas_lock(&lock);
               count++;
               cas_unlock(&lock);
        }
        return NULL;
}

int main(int argc, const char *argv[])
{
        pthread_t id[20];
        int i = 0;

        for(i=0;i<20;++i){
                pthread_create(&id[i],NULL,test_func,NULL);
        }

        for(i=0;i<20;++i){
                pthread_join(id[i],NULL);
        }

        printf("%d\n",count);
        return 0;
}
