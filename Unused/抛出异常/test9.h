#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

class Base
{
public:
	void show()
	{
		cout << "in base " << endl;
	}
};

class Child : public Base
{
public:
	void show()
	{
		cout << "in child" << endl;
	}
		
};
