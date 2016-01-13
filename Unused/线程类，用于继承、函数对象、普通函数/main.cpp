#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

class Runnable
{
public:
    virtual void operator()() = 0;
};

template < class T >
class Thread : public Runnable
{
public:
    Thread() : mRunObj(NULL)
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init (&_condition, NULL);
    }
    
    Thread(void* runObj) : mRunObj(runObj)
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init (&_condition, NULL);
    }

    virtual ~Thread()
    {
    	pthread_cond_destroy(&_condition);
    	pthread_mutex_destroy(&_mutex);
    }

    
    virtual void operator()()
    {
        cout << "thread operator()" << endl;
        T& t = *(T*)mRunObj;
        t();

    }

	virtual bool start()
	{
        pthread_mutex_lock(&_mutex);
        
        int tmp = pthread_create(&_handle, NULL, routine, this);
        bool r = (tmp == 0);

    	if(r) {
    		pthread_cond_wait(&_condition, &_mutex);
			printf("Succ to initialize thread (on_start())\n");
    	} else {
    		printf("Failed to create thread \n");
    	}

        
        pthread_mutex_unlock(&_mutex);
    }

    static void* routine(void* arg)
    {
        Runnable& t = *(Runnable*)arg;
        t();
    }
    
public:
    void* mRunObj;
	pthread_t _handle;
	pthread_mutex_t _mutex;
	pthread_cond_t _condition;
};

class MyThread : public Thread<Runnable>
{
public:
    MyThread(){}
    
    virtual void operator()()
    {
        pthread_mutex_lock(&_mutex);

        cout << "MyThread operator()" << endl;
        sleep(2);
        
        pthread_cond_signal(&_condition);
        pthread_mutex_unlock(&_mutex);
    }
};

class RunObj : public Runnable
{
public:
    RunObj(){}

    void setLockAndCondition(pthread_mutex_t* lock, pthread_cond_t* cond){this->lock=lock;this->cond=cond;}
    
    virtual void operator()()
    {
        pthread_mutex_lock(lock);

        cout << "RunObj operator()" << endl;
        sleep(2);
        
        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    pthread_mutex_t* lock;
    pthread_cond_t* cond;
};

pthread_mutex_t* glock;
pthread_cond_t* gcond;
void run()
{
    pthread_mutex_lock(glock);
    
    cout << "run()" << endl;
    sleep(2);
    
    pthread_cond_signal(gcond);
    pthread_mutex_unlock(glock);
}

int main(void)
{
    //继承线程类，然后直接调用执行
    MyThread* thd1 = new MyThread();
    thd1->start();

    //线程类执行RunObj函数对象
    RunObj* runObj = new RunObj();
    Thread<Runnable>* thd2 = new Thread<Runnable>(runObj);
    runObj->setLockAndCondition(&thd2->_mutex,&thd2->_condition);
    thd2->start();

    //线程类执行run函数
    typedef void (*Handle)();
    Handle f = run;
    Thread< Handle >* thd3 = new Thread< Handle >((void*)&f); //这里将函数的指针的指针强转为void*才能编译通过
    glock= &thd3->_mutex;
    gcond= &thd3->_condition;
    thd3->start();
    
    return 0;
}



