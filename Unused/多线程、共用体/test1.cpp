//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

using namespace std;

template <typename T>
class Widget {
public:
    Widget() { ++count; }
    Widget(const Widget&) { ++count; }
    ~Widget() { --count; }

    static size_t howMany()
    { return count; }

private:
    static size_t count;
};
template <typename T>
size_t Widget<T>::count = 0;

class CBow : private Widget<CBow>
{
private:    
int8 m_i8Color;

public:
    void SetColor(int8 i8Color)
    {m_i8Color = i8Color;}
    int8 GetColor()
    {return m_i8Color;}
    using Widget<CBow>::howMany;
};
void test(CBow* pclBow)
{
    cout << pclBow->GetColor() << endl;
}
typedef struct 
{
    uint16 u16Arg1;
    uint16 u16Arg2;
    uint16 u16Arg3;
    
}_stFnArgs;
typedef union
{
    uint16 u16Status;
    struct _stStatus
    {
        uint16 bt1HeadTR:1;
        uint16 bt1BodyTR:1;
        uint16 bt1UnblankTR:1;
        uint16 bt10Status:10;
        uint16 bt1NonRecoverFault:1;
        uint16 bt1WatchdogStatus:1;
        uint16 bt1FaultStatus:1;
    }stStatus;
}_unStatus;

pthread_mutex_t threadMutex;

/*
// 通过默认条件建锁
    pthread_mutex_t *theMutex = new pthread_mutex_t; 
    pthread_mutexattr_t attr; 
    pthread_mutexattr_init(&attr); 
    pthread_mutex_init(theMutex,&attr); 
    pthread_mutexattr_destroy(&attr); 

    // 递归加锁
    pthread_mutex_lock (theMutex); 
    pthread_mutex_lock (theMutex); 
    pthread_mutex_unlock (theMutex); 
    pthread_mutex_unlock (theMutex);
*/

void ThreadTest(const char* pchFuncName)
{
    pthread_mutex_lock (&threadMutex);
    
    cout << "the called function is " << pchFuncName << endl;
    sleep(3);
    
    pthread_mutex_unlock (&threadMutex);
}

void* ThreadTask(void* args)
{
   
    //int32 i32Temp = *(int32*)args;
    _stFnArgs stFnArgs = *(_stFnArgs*)args;
    uint8 u8I;
    void* pvI;
    
    pthread_detach(pthread_self());
     
    while(u8I < 4)
    {
        cout << stFnArgs.u16Arg1 << " " << stFnArgs.u16Arg2<< endl;
        ThreadTest(__FUNCTION__);
        sleep(1);

        u8I++;
    }
    pthread_exit(0);
    return 0;
}

int main()
{
    vector<int> vecTest;
    vector<int>::iterator vecIte1, vecIte2;
    for(int32 i32I = 0; i32I < 10; i32I++)
    {
        vecTest.push_back(i32I);
    }
    vecIte1 = vecTest.begin();
    vecIte2 = vecTest.end();

    cout << "The distance is" << distance(vecIte1, vecIte2) << endl;

    vecIte1 = find(vecIte1, vecIte2, 7);
    cout << &(*vecIte1) << endl;
    vecTest.insert(vecIte1, 45);

    int32 i32Temp;
    vecTest.front() = 342;
    
    cout << *vecTest.begin() << endl << "vector size is " << vecTest.size() <<endl<<  vecTest.capacity() << endl;
    vecTest.reserve(30);
    cout << vecTest.capacity() << endl;


    CBow clBow;
    cout << clBow.howMany() << endl << Widget<CBow>::howMany() << endl;
    //Widget<CBow>* pclWidget = new CBow();
    cout << sizeof(CBow) << endl;
    CBow* pclBow = &clBow;
    pclBow->SetColor('A');
    test(pclBow);

    cout << pclBow->GetColor() << endl;
    OM_INFO("----------------------------------------------\n");
    //共用体中含结构体的用法
     _unStatus unStatus;
    unStatus.stStatus.bt10Status = 0x3ff;
    cout << (unStatus.u16Status >> 3) << endl;
    char achTmpFileName[20] = "om_z3";
    uint32 u32NewVersion;
    sscanf("om_z3", "om_z%d", &u32NewVersion);
    cout << u32NewVersion << endl;

    OM_INFO("----------------------------------------------\n");
    //多线程的应用
    i32Temp = 30;
    _stFnArgs stFnArgs;
    stFnArgs.u16Arg1 = 55;
    stFnArgs.u16Arg2 = 345;
    stFnArgs.u16Arg3 = 6;
    pthread_t threadId;
    int32 i32Ret;
    pthread_mutex_init(&threadMutex, NULL);

    i32Ret = pthread_create(&threadId, NULL, ThreadTask, (void*)&stFnArgs);
    if(i32Ret != 0) 
    {
        OM_ERROR("Creat ThreadTask failed! \n");
       
    }
    while(1)
    {
        ThreadTest(__FUNCTION__);
        sleep(2);
    }
    
    
    return 0;
	
}

#if 0
int main()
{
    uint32 u32I;
    float32 f32Power,f32Temp;

    f32Power = 25.78;
    u32I = *(uint32*)&f32Power;
    memcpy(&f32Temp, &u32I, sizeof(uint32));
    cout << u32I << " " << *(float32*)&u32I << endl;
    cout << f32Temp << endl;
}


// vector::push_back
using namespace std;

int main ()
{
  vector<int> myvector;
  int myint;

  cout << "Please enter some integers (enter 0 to end):\n";

  do {
    cin >> myint;
    myvector.push_back (myint);
  } while (myint);

  cout << "myvector stores " << (int) myvector.size() << " numbers.\n";

  return 0;
}
#endif
