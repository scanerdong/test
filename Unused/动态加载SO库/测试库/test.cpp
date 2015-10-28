#include <stdio.h>
#include <iostream>
#include "test.h"
 #ifdef __cplusplus
    extern "C" {
    #endif

void* create_obj()
{
Base* b = new (std::nothrow) Base();
printf("create_obj is called\n");
return b;

}

void free_obj(void* obj)
{
Base* b = (Base*)obj;
printf("free obj is called\n");
delete b;

}

   #ifdef __cplusplus
    }
    #endif
