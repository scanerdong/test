#include "UnlockQueue.h"
#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include "DFilter.h"
#include <boost/bind.hpp>

using namespace std;


class Call
{
public:
    void operator()(int i)
    {
        cout << "Call product id = " << i << endl;
    }
};

void call(int i)
{
    cout << "call product id = " << i << endl;
}

template <class C>
class Product
{
public:
    void operator()(C c, int i)
    {
        c(i);
    }
};


int main(int argc, char** argv)
{

//函数对象
Call c;
Product<Call> pdt;

pdt(c, 15);


//C风格函数
typedef void (*F)(int);

F f = call;

Product<F > pdt2;

pdt2(f,16);

cout << sizeof(c) << endl;

return 0;
}

