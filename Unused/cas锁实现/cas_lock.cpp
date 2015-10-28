#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;

/*
bool __sync_bool_compare_and_swap(type* ptr, type oldval, type newval)����
����typeΪ 1, 2, 4 or 8 �ֽڵ�int��uint
�ú����������ǽ���ԭ��(�̰߳�ȫ)�ıȽϺͽ�����
���ptr���ֵΪoldval��������newval��������true;
���ptr���ֵ��Ϊoldval���򷵻�false
*/

//0:none lock, 1:locked
static int lock = 0;
#define cas_lock(lock) do{\
while(!__sync_bool_compare_and_swap(lock, 0, 1)){\ //��ѭ����ֱ��lock���ֵ����Ϊ1Ϊֹ
sched_yield();\
}\
}while(0);

#define cas_unlock(lock) do{\
*lock = 0; \ //��lock���ֵ���¸�Ϊ0����ʾ���¸�ֵ
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
