#ifndef __SRC_H__
#define __SRC_H__
#include <stdio.h>

//int sum_test(int a,int b);

class Base
{
public:
virtual void p(){printf("m_n = %d\n",m_n);}
virtual void set(int n) {m_n = n;}
private:

int m_n;
};

    #ifdef __cplusplus
    extern "C" {
    #endif
void* create_obj();
void free_obj(void* obj);


#ifdef __cplusplus
    }
    #endif
#endif
