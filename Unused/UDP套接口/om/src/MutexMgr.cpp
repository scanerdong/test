/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : CMutexMgr.cpp
 * @version : 1.0.0
 * @brief : 实现互斥锁管理类
 *   
 * @author : zhangx
 * @date : 2011年3月23日 10:30:29
 * @history : 
 *   
 */

/************************引用部分*****************************/
#include "MutexMgr.h"

/************************前向声明部分***********************/


/************************定义部分*****************************/


/************************实现部分*****************************/

//实现CThreadMutex
_enResult CThreadMutex::Init()
{
    int32 i32Ret;
    
    i32Ret = pthread_mutex_init(&m_pthreadMutex, NULL); 
    return (i32Ret == 0)?SUCC:FAIL;
}

_enResult CThreadMutex::Destroy()
{
    int32 i32Ret;
    
    i32Ret = pthread_mutex_destroy(&m_pthreadMutex); 
    return (i32Ret == 0)?SUCC:FAIL;
}

void CThreadMutex::Lock(const char* pchFileName, uint32 u32LineNum)
{    
    m_pchFileName = pchFileName;
    m_u32LineNum = u32LineNum;
    
    m_u32LockCount++; 
    pthread_mutex_lock(&m_pthreadMutex);
}

void CThreadMutex::Unlock()
{
    if(m_u32LockCount > 0)
    {
        m_u32LockCount--; 
    }
    pthread_mutex_unlock(&m_pthreadMutex);
}    


//实现CMutexMgr
_enResult CMutexMgr::Init()
{
    OM_INFO("Init CMutexMgr.\n");

    m_thisMutexId = (MUTEXID)(new CThreadMutex());

    return SUCC;
}

_enResult CMutexMgr::Destroy()
{
	CMutexList::iterator li;        
    CMutex* pclMutex;    
    
    OM_INFO("Destroy CMutexMgr.\n");
    
    for(li = m_clMutexList.begin(); li != m_clMutexList.end(); li++)
    {
	    pclMutex = (*li);
        pclMutex->Destroy();
        delete pclMutex;
    }
    
    m_clMutexList.clear();
        
	return SUCC;
}


_enResult CMutexMgr::Reset()
{
	return SUCC;
}

_enResult CMutexMgr::Run()
{
	return SUCC;
}

_enResult CMutexMgr::Dump(FILE* pf)
{	
    CMutexList::iterator li;    
    CMutex* pclMutex;    
    AUTO_MUTEX(m_thisMutexId);
	
    DUMP_PRINT(pf, "CMutexMgr::Dump there are %d mutex(s) in system, They are: \r\n", m_clMutexList.size());
    for(li = m_clMutexList.begin(); li != m_clMutexList.end(); li++)
    {
	    pclMutex = (*li);
        DUMP_PRINT(pf, "Key 0x%x, Lock count %d, Lock at %s %d, Created at %s %d\r\n", 
            (MUTEXID)pclMutex, pclMutex->GetLockCount(), pclMutex->m_pchFileName, pclMutex->m_u32LineNum, 
            pclMutex->m_pchCreateFileName, pclMutex->m_u32CreateLineNum);
    }
    
    return SUCC;
}

MUTEXID CMutexMgr::AddMutex(_enMutexType enMutexType, const char* pchFileName, uint32 u32LineNum)
{
    AUTO_MUTEX(m_thisMutexId);
    
	return AddMutexInternal(enMutexType, pchFileName, u32LineNum);
}

_enResult CMutexMgr::DelMutex(MUTEXID mutexId)
{
    AUTO_MUTEX(m_thisMutexId);
    
	return DelMutexInternal(mutexId);
}

MUTEXID CMutexMgr::AddMutexInternal(_enMutexType enMutexType, const char* pchFileName, uint32 u32LineNum)
{
	CMutex* pclMutex;

    //这里不能使用NEW宏来分配资源否则会互锁
    //根据锁类型创建锁对象
    if(enMutexType == MUTEX_TYPE_TRHEAD)
    {
	    pclMutex = new CThreadMutex();
    }
    else
    {
        pclMutex = new CProcessMutex();
    }
    
    pclMutex->Init();		
    pclMutex->m_pchCreateFileName = pchFileName;
    pclMutex->m_u32CreateLineNum = u32LineNum;
    m_clMutexList.push_back(pclMutex);	
	
	return (MUTEXID)pclMutex;
}

_enResult CMutexMgr::DelMutexInternal(MUTEXID mutexId)
{
    CMutex* pclMutex = NULL;
    CMutexList::iterator li;

    li = find(m_clMutexList.begin(), m_clMutexList.end(), (CMutex*)mutexId);
    if(li == m_clMutexList.end())
	{
        return FAIL;
	}

    pclMutex = *li;
    pclMutex->Destroy();
    m_clMutexList.erase(li);

    return SUCC;
}


#ifdef __MUTEX_MGR_TEST__
	
//定义系统中会使用的锁        
typedef enum
{
    DEFAUL_MUTEX = 0,
    FB_REG_MUTEX,
}_enMutexEnum;

MUTEXID mutexId1;
MUTEXID mutexId2;

void TestAutoMutex ()
{
	printf("Enter TestAutoMutex:\n");
	AUTO_MUTEX(mutexId1);
	printf("Out TestAutoMutex:\n");
}

void* MonitorMutexTask(void * pParam)
{
    pthread_detach(pthread_self());
    while(1)
    {
        sleep(20);
        printf("Before unlock-------\n");
        MUTEX_UNLOCK(mutexId1);        
        MUTEX_UNLOCK(mutexId2);        
        printf("After unlock-------\n");
        TSingleton<CMutexMgr>::Instance()->Dump(NULL); 
    }
    return NULL;
}
		
void* ResumeMutexTask(void * pParam)
{
    pthread_detach(pthread_self());
    while(1)
    {
        sleep(3);
        
        printf("Before lock------\n");
        MUTEX_LOCK(mutexId1);
        MUTEX_LOCK(mutexId2);
        printf("After lock-------\n");
    }
    return NULL;
}
int32 main()
{
    MUTEXID mutexId3, mutexId4, mutexId5;
	CMutexMgr* pclMutexMgr = TSingleton<CMutexMgr>::Instance();

    printf("mutex manager test 1\n");
	pclMutexMgr->Init();	
	pclMutexMgr->Dump(NULL);
    printf("mutex manager test 2\n");

    mutexId1 = ADD_MUTEX();
    mutexId2 = ADD_MUTEX();
	mutexId3 = ADD_MUTEX();
    mutexId4 = ADD_MUTEX();
    mutexId5 = ADD_MUTEX();

    MUTEX_UNLOCK(mutexId1);
    MUTEX_LOCK(mutexId2);
    MUTEX_LOCK(mutexId3);
    MUTEX_LOCK(mutexId4);
    
    printf("mutex manager test 3\n");
	pclMutexMgr->Dump(NULL);    
   
    TestAutoMutex();
    
    printf("mutex manager test 4\n");
	pthread_t m_resumeThreadId;
    pthread_create(&m_resumeThreadId, 0, ResumeMutexTask, 0);
    
	pthread_t m_monitorThreadId;
    pthread_create(&m_monitorThreadId, 0, MonitorMutexTask, 0);
	
    printf("mutex manager test 5\n");
    while(1) sleep(10);
    
	return SUCC;
}

#endif

