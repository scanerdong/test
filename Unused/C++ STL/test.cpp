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
#include "test.h"

using namespace std;



/*
typedef struct
{
    string strFuncName;
    _pfnHanders pfnHanders;
}_stProcessItem;

static _stProcessItem s_astProcessItem[] = 
{
    {},
    {},
    {}
};
*/
CFuncProcMap clFuncProcMap;

int32 a = 40;


void fun1()
{
    _pfnHanders pfnHanders = fun1;
    std::pair<CFuncProcMap::iterator, bool> clInsertPair;
    
    clInsertPair = clFuncProcMap.insert(CFuncProcMap::value_type(__FUNCTION__, pfnHanders)); 
    
    cout << "In Fun1" << endl;
}

void fun2()
{
    _pfnHanders pfnHanders = fun2;
    std::pair<CFuncProcMap::iterator, bool> clInsertPair;
    
    clInsertPair = clFuncProcMap.insert(CFuncProcMap::value_type(__FUNCTION__, pfnHanders)); 

    cout << "In Fun2" << endl;
}

void fun3()
{
    _pfnHanders pfnHanders = fun3;
    std::pair<CFuncProcMap::iterator, bool> clInsertPair;
    
    clInsertPair = clFuncProcMap.insert(CFuncProcMap::value_type(__FUNCTION__, pfnHanders)); 
    //if(clInsertPair.second == FALSE) return ;

    cout << "In Fun3" << endl;
}

void GetA(int32** ppi32Value)
{
    *ppi32Value = &a;
}
int main()
{
    char achFuncName[20] = {0};
    CFuncProcMap::iterator fi;
    _pfnHanders pfnHanders = NULL;
    fun1();
    fun2();
    fun3();

    cout << "cin a function name :" << endl;
    scanf("%s",achFuncName);

    fi = clFuncProcMap.find(achFuncName);
    if(fi != clFuncProcMap.end())
    {
        pfnHanders = (*fi).second;

        pfnHanders();
    }

    cout << "------------------------" << endl;
    int32 i32Value;
    int32* pi32Value = &i32Value;
    GetA(&pi32Value);
    cout << i32Value << endl << *pi32Value << endl;

    cout << "------------------------" << endl;
    char achFileName[20] = {0};
    char* pchExeFileName = "qq.exe";
    strcpy(achFileName, pchExeFileName);
    cout << achFileName << endl;
    return 0;
	
}
