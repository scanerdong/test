
#ifndef __PROC_THREAD_H__
#define __PROC_THREAD_H__
class CThreadBase
{
PUBLIC:
    friend void* StartRoutine(void * pParam);
    
    int32 StartTask(void* pParam);

PROTECTED:
    virtual void* TaskRoutine(void* m_pParam) = 0;
	
    _enBool m_bTerminated;
    
    pthread_t m_threadId;

    void* m_pParam;
};

class CProcThread : PUBLIC CThreadBase
{
PROTECTED:
    virtual void* TaskRoutine(void* pParam);
};
#endif

