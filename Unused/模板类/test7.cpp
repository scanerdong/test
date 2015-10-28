//庁医窃議喘隈。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。

#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
using namespace std;

uint8 g_u8PrintCtrl = 0xff;

#ifndef _COMPARE_H
#define _COMPARE_H

template <typename T>
class Compare
{
public:
 ~Compare(void);
 Compare(T,T);
 T max();
 T min();
private:
 T x;
 T y;
};
#endif

//Compare.cpp
template <typename T>
Compare<T>::Compare(T a,T b)
{
 x=a;
 y=b;
}
template <typename T>
Compare<T>::~Compare(void)
{
}
template <typename T>
T Compare<T>::max()
{
 return (x>y)?x:y;
}
template <typename T>
T Compare<T>::min()
{
 return (x<y)?x:y;
}

int main()
{
 Compare<int> tt(1,2);
cout<<tt.max()<<endl;

cout << sizeof(Compare<int>) <<endl;
 //system("gedit");
 getchar();
 return 0;
}

