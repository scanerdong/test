#include "ProcThread.h" 
//CMsgProcMgr
#ifndef __MSG_PROC_H__
#define __MSG_PROC_H__
class CMsgProcMgr
{
       
PUBLIC:
    friend class CProcThread;

	_enResult Init();
	_enResult Run();
    
PRIVATE:
	void RecvMessage();
	
    CProcThread* m_pclProcThread;

};
#endif

