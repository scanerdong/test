//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
using namespace std;

uint8 g_u8PrintCtrl = 0xff;

class Student
{
public:
     ~Student();

    virtual void ChangeParam();

    int32 GetParam();

    void SetParam(int i32Param = 12);

    Student* GetThis();
private:
    int32 m_i32Param;
};

Student::~Student()
{
    cout << "in ~Student " <<endl;
}

int Student::GetParam()
{
    return m_i32Param;
}

void Student::SetParam(int i32Param)
{
    m_i32Param = i32Param;
}

void Student::ChangeParam()
{
    m_i32Param++;
    cout << "in ChangeParam" << endl;
}

Student* Student::GetThis()
{
return this;
}

typedef void (*MyFuncPtrType)();

int main()
{
    MyFuncPtrType FuncPtr;
    int32 i32Temp,i32Temp2;
        
    TSingleton<Student>::Instance()->SetParam();
    cout << sizeof(Student) << endl << TSingleton<Student>::Instance()->GetParam() << endl;

    
    //i32Temp = (int32)(TSingleton<Student>::Instance()->GetThis());
    //i32Temp2 = *((int32*)i32Temp);
    FuncPtr = (MyFuncPtrType)(*(int32*)(TSingleton<Student>::Instance()));//(MyFuncPtrType)(*(int32*)i32Temp2);
    FuncPtr();
    
    TSingleton<Student>::Instance()->ChangeParam();
    
    cout << TSingleton<Student>::Instance()->GetParam() << endl;
    
    cout << *(int32*)((char*)(TSingleton<Student>::Instance()) + 4)<< endl;

    delete TSingleton<Student>::Instance();
}
