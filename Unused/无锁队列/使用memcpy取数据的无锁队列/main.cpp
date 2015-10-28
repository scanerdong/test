#include "UnlockQueue.h"
#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#pragma pack(1)
struct student_info
{
   unsigned int len; //长度，包括len自身的4个字节
   long stu_id;
   unsigned int age;
   unsigned int score;
   char achInfo[0]; //学生的说明
};
#pragma pack()


void print_student_info(const student_info *stu_info)
{
    if(NULL == stu_info)
        return;
    
    printf("len:%u\t",stu_info->len);
    printf("id:%ld\t",stu_info->stu_id);
    printf("age:%u\t",stu_info->age);
    printf("score:%u\t",stu_info->score);
    printf("info:%s\n",stu_info->achInfo);
}


student_info * get_student_info()
{
    static long id;
    static unsigned int age;
    static unsigned int score;
    static unsigned int infolen = 1;
    
     student_info *stu_info = (student_info *)malloc(sizeof(student_info) + infolen);
     if (!stu_info)
     {
        fprintf(stderr, "Failed to malloc memory.\n");
        return NULL;
     }

     for(unsigned int i=0; i<infolen; i++)
     {
        stu_info->achInfo[i] = 'A';
     }
     stu_info->achInfo[infolen-1] = 0;
     
     stu_info->stu_id = id;
     stu_info->age = age;
     stu_info->score = score;
     stu_info->len = sizeof(student_info) + infolen;

     id++;age++;score++;infolen = infolen % 20 + 1;
     //print_student_info(stu_info);
     return stu_info;
}
 
void * consumer_proc(void *arg)
{
     UnlockQueue* queue = (UnlockQueue *)arg;
     static unsigned char ach_stu_info[1000];
     
     while(1)
     {
         usleep(10);
         #if 1
         unsigned int len = queue->Get(ach_stu_info, 1000);
         if(len > 0)
         {
             printf("------------------------------------------\n");
             printf("UnlockQueue length after get: %u\n", queue->GetDataLen());
             print_student_info((student_info*)ach_stu_info);
             printf("------------------------------------------\n");
         }
         #endif
     }
     return (void *)queue;
}
 
void * producer_proc(void *arg)
{
  time_t cur_time;
  UnlockQueue *queue = (UnlockQueue*)arg;
  while(1)
  {
      printf("******************************************\n");
      student_info *stu_info = get_student_info();
      printf("put a student info to queue.\n");
      queue->Put( (unsigned char *)stu_info, stu_info->len);
      free(stu_info);
      printf("UnlockQueue length after put: %u\n", queue->GetDataLen());
      printf("******************************************\n");
      usleep(10);
  }
 return (void *)queue;
}
 

#include <arpa/inet.h>
int main(int argc, char** argv)
{
    //int v = atoi(argv[1]);
    
    //printf("%s\n", inet_ntoa(*(struct in_addr*)&v));
    //exit(0);
    
    UnlockQueue unlockQueue(1024);
    if(!unlockQueue.Initialize())
    {
        return -1;
    }
 
    pthread_t consumer_tid, producer_tid;
 
    printf("multi thread test.......\n");
 
    if(0 != pthread_create(&producer_tid, NULL, producer_proc, (void*)&unlockQueue))
    {
         fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
                 errno, strerror(errno));
         return -1;
    }
 
    if(0 != pthread_create(&consumer_tid, NULL, consumer_proc, (void*)&unlockQueue))
    {
           fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
                   errno, strerror(errno));
           return -1;
    }
 
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);
 
    return 0;
 }

