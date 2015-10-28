/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MutexMgr.h
 * @version : 1.0.0
 * @brief : 定义互斥锁管理类
 *   
 * @author : zhangx
 * @date : 2011年3月22日 15:31:27
 * @history : 
 *   
 */

#ifndef MUTEX_MGR_H
#define MUTEX_MGR_H

/************************引用部分*****************************/
#include <map>
#include <pthread.h>
	
#include "Om.h"
#include "Singleton.h"
#include "ComBase.h"

/************************前向声明部分***********************/


/************************定义部分*****************************/

//其他模块通过下列宏使用互斥锁
//注意自动锁的作用域仅和自动锁对象相同，
//如在while代码段中使用自动锁其作用域也仅为while代码段
#define ADD_MUTEX() \
        TSingleton<CMutexMgr>::Instance()->AddMutex(MUTEX_TYPE_TRHEAD, __FILE__, __LINE__)
        
#define ADD_PROCESS_MUTEX() \
        TSingleton<CMutexMgr>::Instance()->AddMutex(MUTEX_TYPE_PROCESS, __FILE__, __LINE__)
        
#define DEL_MUTEX(mutexId) \
        TSingleton<CMutexMgr>::Instance()->DelMutex(mutexId)
    
#define AUTO_MUTEX(mutexId) \
    	CAutoMutex _clAutoMutex_##mutexId_(mutexId, __FILE__, __LINE__)
    	
#define MUTEX_LOCK(mutexId)		                         \
        do{                                              \
        CMutex* _pclMutex_##mutexId_ = (CMutex*)mutexId; \
        _pclMutex_##mutexId_->Lock(__FILE__, __LINE__); }while(0)
        
#define MUTEX_UNLOCK(mutexId)                            \
        do{                                              \
        CMutex* _pclMutex_##mutexId_ = (CMutex*)mutexId; \
        _pclMutex_##mutexId_->Unlock(); }while(0)          

//定义mutex ID
typedef uint32 MUTEXID;

//定义互斥锁类型
typedef enum
{
    MUTEX_TYPE_TRHEAD = 0,
    MUTEX_TYPE_PROCESS
}_enMutexType;


//定义CMutex
class CMutex
{
PUBLIC:
	CMutex(){m_u32LockCount = 0; m_pchFileName = NULL; m_u32LineNum = 0;}
    
	~CMutex(){};
    
    virtual _enResult Init() = 0;
    
    virtual _enResult Destroy() = 0;
    
	virtual void Lock(const char* pchFileName, uint32 u32LineNum) = 0;
    
	virtual void Unlock() = 0;    
    
	virtual uint32 GetLockCount(){return m_u32LockCount;}
    
PROTECTED:   
    friend class CMutexMgr;
	uint32 m_u32LockCount;    
	const char* m_pchFileName;
	uint32 m_u32LineNum;    
	const char* m_pchCreateFileName;
	uint32 m_u32CreateLineNum;
}; 

//定义CThreadMutex
class CThreadMutex : public CMutex
{
PUBLIC:    
    _enResult Init();
    
    _enResult Destroy();
    
	void Lock(const char* pchFileName, uint32 u32LineNum);
    
	void Unlock();
    
PRIVATE:    
	pthread_mutex_t m_pthreadMutex;
}; 

//定义CProcessMutex
class CProcessMutex : public CMutex
{
    _enResult Init(){return SUCC;}
    
    _enResult Destroy(){return SUCC;}
    
	void Lock(const char* pchFileName, uint32 u32LineNum){}
    
	void Unlock(){}    
}; 

//定义CAutoMutex
class CAutoMutex
{
PUBLIC:
	explicit CAutoMutex(MUTEXID mutexId, const char* pchFileName, uint32 u32LineNum) : m_pclMutex((CMutex*)mutexId)
        {m_pclMutex->Lock(pchFileName, u32LineNum);}
        
	~CAutoMutex()
        {m_pclMutex->Unlock();}
        
PRIVATE:
	CMutex* m_pclMutex;
};

//定义mutex映射        
typedef std::list<CMutex*> CMutexList;

//定义CMutexMgr
class CMutexMgr : PUBLIC CComBase
{
    DECLARE_DEFAULT_COM_FUN;
        
PUBLIC:
	MUTEXID AddMutex(_enMutexType enMutexType, const char* pchFileName, uint32 u32LineNum);
    
	_enResult DelMutex(MUTEXID mutexId);    
    
PRIVATE:
	MUTEXID AddMutexInternal(_enMutexType enMutexType, const char* pchFileName, uint32 u32LineNum);
    
	_enResult DelMutexInternal(MUTEXID mutexId);       
        
	MUTEXID m_thisMutexId;
    
	CMutexList m_clMutexList;
};

#endif 
