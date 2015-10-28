#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
using namespace std;

uint8 g_u8PrintCtrl = 0xff;


static void PrintBuf()
{
    cout << "in PrintBuf" << endl;
}

class CFather
{
PUBLIC:
    virtual ~CFather();
    
    virtual _enResult Init();
    
    CFather();
   // virtual ~CFather() {cout << "CFather Destruct " << endl; };
PROTECTED:    

    virtual _enResult InitSoft();
};

CFather::~CFather()
{
    cout << "in ~cfather"<< endl;
}

CFather::CFather()
{
    cout << "CFather Construct " << endl; 
}

_enResult CFather::Init()
{
    cout << "CFather Init " << endl; 
    //InitSoft();
    //PrintBuf();
}

_enResult CFather::InitSoft()
{
    cout << "CFather InitSoft " << endl; 
}


class CChild : PUBLIC CFather
{
PUBLIC:       
    virtual _enResult InitSoft();
    virtual _enResult InitHardware();
    CChild() ;
    ~CChild() ;
};
CChild::CChild()
    {cout << "CChild  Construct " << endl; }

CChild::~CChild()
    {cout << "CChild Destruct " << endl; }

_enResult CChild::InitHardware()
{
    cout << "CChild InitHardware" << endl;
}
_enResult CChild::InitSoft()
{
    cout << "CChild InitSoft " << __FUNCTION__ << endl; 
}


int main ()
{
	CFather* pclFather = TSingleton<CChild>::Instance();
	
	//TSingleton<CMsgProcMgr>::Instance()->Run();
    ((CChild*)pclFather)->InitHardware();
	
	while(1)
    {
       sleep(5);
	   printf("--------in main---------\n");
    }
	
	delete pclFather;

	return 0;
}

