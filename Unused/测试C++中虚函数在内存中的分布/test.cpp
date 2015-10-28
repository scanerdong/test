//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "test.h"

using namespace std;

//计数类的实现
template <typename T>
class Count
{
public:
    Count() {++m_i32Count;};
    
    Count(const Count&) {m_i32Count++;};

    ~Count() {--m_i32Count;};

    int32 GetCount();
    
private:
    static int32 m_i32Count;
};

template <typename T>
int32 Count<T>::GetCount()
{
    return m_i32Count;
}

template <typename T>
int32 Count<T>::m_i32Count = 0;

typedef void(*_pfnHander)(void);
//基类部分
class Base
{
 public:
    virtual void Init();
    
    virtual int32 HowMany();
};

void Base::Init()
{ 
    cout << "in Base Init" << endl;
    //HowMany();
}

int32 Base::HowMany()
{
    cout << "in Base HowMany" << endl;
    return 0;
}

//派生类部分
class Derived : public Base, public Count<Derived>
{
public:
    virtual void Init();

    int32 HowMany();
};

void Derived::Init()
{
    //Base::Init();
    cout << "in Derived Init" << endl;
}

int32 Derived::HowMany()
{
    cout << "in Derived HowMany" << endl;
    return GetCount();
}

//派生类部分
class Third : public Derived
{
public:
    virtual void Init();

    int32 HowMany();
};

void Third::Init()
{
    //Base::Init();
    cout << "in Third Init" << endl;
}

int32 Third::HowMany()
{
    cout << "in Third HowMany" << endl;
    return GetCount();
}

int main()
{
    Base* pclBase = TSingleton<Third>::Instance();
    Derived* pclDerived = TSingleton<Derived>::Instance();
    
    //TSingleton<Derived>::Instance()->Init();
    //pclBase->Init();

    ( (void (*)(void)) *((uint32*) *(uint32*)pclBase + 1) )();
    //pclBase->HowMany();
    return 0;

}
//基类指针执行自己的INIT
