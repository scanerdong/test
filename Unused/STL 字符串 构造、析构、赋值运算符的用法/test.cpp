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

String::String(const char* pchSrc)
{
    if(pchSrc == NULL)
    {
        m_pchData = new char[1];
        *m_pchData = '\0';
    }
    else
    {
        m_pchData = new char[sizeof(pchSrc) + 1];
        strcpy(m_pchData, pchSrc);
    }
}

String::String(const String& strOther)
{
    int32 i32OtherLen = strlen(strOther.m_pchData);
    m_pchData = new char[i32OtherLen + 1];
    strcpy(m_pchData, strOther.m_pchData);
    
}

String::~String(void)
{
    delete [] m_pchData;
}

String& String::operator=(const String& strOther)
{
    if(this == &strOther) return *this;
    
    delete [] m_pchData;

    int32 i32OtherLen = strlen(strOther.m_pchData);
    m_pchData = new char[i32OtherLen + 1];
    strcpy(m_pchData, strOther.m_pchData);

    return *this;
}

int main()
{
    String strTest("hello");
    String strTemp;

    strTemp = strTest;

    cout << strTest.m_pchData << endl << strTemp.m_pchData << endl;
    cout << sizeof("hello") << endl << strlen("hello") << endl;
    return 0;
	
}
