#include <malloc.h>
#include <stdio.h>  
#include <memory.h>
#include <zmq.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <map>
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

char* getTimeStr (char *buf, time_t t)
{
    time_t the_time = t;
    struct tm *tm_ptr = localtime (&the_time);
    sprintf (buf, "%02d%02d%02d%02d%02d%02d", tm_ptr->tm_year,
            tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour,
            tm_ptr->tm_min, tm_ptr->tm_sec);
    return buf;
}

#include <boost/thread/thread.hpp> 
#include <boost/thread/tss.hpp> 
#include <cassert> 
  
//      这里定义了一个线程独立的全局变量 
//boost::thread_specific_ptr<int> value; 
int* value;
void increment() 
{ 
    //int* p = value.get();
	int* p = value;
    ++*p; 
} 
  
void thread_proc() 
{ 
    //value.reset(new int(0)); // 每个线程初始化自己的value。
    value = new int(0);
    for (int i=0; i<10; ++i) 
    { 
        increment(); 
        //int* p = value.get();
        int* p = value;
		printf("%d\n", *p);
		sleep(1);
        assert(*p == i+1); // 这里检查是不是有别的thread修改本线程的value 
    } 
} 
  
int main(int argc, char* argv[]) 
{ 
    boost::thread_group threads; 
    for (int i=0; i<5; ++i) 
       threads.create_thread(&thread_proc); 
    threads.join_all(); 
} 

