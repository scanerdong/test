/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename       :  MutexMgr.h
 * @Version        :  1.0.0
 * @Brief            :  ���廥����������
 *   
 * @Author         :  zhangx
 * @CreatedDate :  2011��3��22�� 15:31:27
 * @History         : 
 *   
 */

#ifndef MUTEX_MGR_H
#define MUTEX_MGR_H

/************************���ò���*****************************/
#include <map>
#include <pthread.h>
	
#include "Common.h"
#include "Singleton.h"
#include "ComBase.h"

/************************ǰ����������***********************/


/************************���岿��*****************************/

//����ģ��ͨ�����к�ʹ�û�����
//ע���Զ���������������Զ���������ͬ��
//����while�������ʹ���Զ�����������Ҳ��Ϊwhile�����
#define ADD_MUTEX(mutexId) \
        TSingleton<CMutexMgr>::Instance()->AddMutex(mutexId, MUTEX_TYPE_TRHEAD)
        
#define ADD_PROCESS_MUTEX(mutexId) \
        TSingleton<CMutexMgr>::Instance()->AddMutex(mutexId, MUTEX_TYPE_PROCESS)
        
#define DEL_MUTEX(mutexId) \
        TSingleton<CMutexMgr>::Instance()->DelMutex(mutexId)
    
#define GET_MUTEX(mutexId) \
        TSingleton<CMutexMgr>::Instance()->GetMutex(mutexId)

#define AUTO_MUTEX(mutexId)  												                 \
        CMutex& _clMutex_##mutexId_ = TSingleton<CMutexMgr>::Instance()->GetMutex(mutexId);  \
        TSingleton<CMutexMgr>::Instance()->LogLock(_clMutex_##mutexId_, __FILE__, __LINE__); \
    	CAutoMutex _clAutoMutex_##mutexId_(_clMutex_##mutexId_)
    	
#define AUTO_MUTEX_BY_REF(clMutex)										    	 \
        TSingleton<CMutexMgr>::Instance()->LogLock(clMutex, __FILE__, __LINE__); \
        CAutoMutex _clAutoMutex_##pclMutex_(clMutex)

#define MUTEX_LOCK(mutexId)												                     \
        do{                                                                                  \
        CMutex& _clMutex_##mutexId_ = TSingleton<CMutexMgr>::Instance()->GetMutex(mutexId);  \
        TSingleton<CMutexMgr>::Instance()->LogLock(_clMutex_##mutexId_, __FILE__, __LINE__); \
        _clMutex_##mutexId_.Lock(); }while(0)
        
#define MUTEX_LOCK_BY_REF(clMutex)											        \
        do{                                                                         \
        TSingleton<CMutexMgr>::Instance()->LogLock(clMutex, __FILE__, __LINE__);    \
        clMutex.Lock(); }while(0)                    

#define MUTEX_UNLOCK(mutexId)                                                               \
        do{                                                                                 \
        CMutex& _clMutex_##mutexId_ = TSingleton<CMutexMgr>::Instance()->GetMutex(mutexId); \
        _clMutex_##mutexId_.Unlock();}while(0)
        
#define MUTEX_UNLOCK_BY_REF(clMutex) \
        do{clMutex.Unlock();}while(0)
            

//����mutex ID
typedef uint32 MUTEXID;

//���廥��������
typedef enum
{
    MUTEX_TYPE_TRHEAD = 0,
    MUTEX_TYPE_PROCESS
}_enMutexType;


//����CMutex
class CMutex
{
PUBLIC:
	CMutex(){m_u32LockCount = 0; m_mutexId = 0;}
    
	~CMutex(){};
    
    virtual _enResult Init(MUTEXID mutexId) = 0;
    
    virtual _enResult Destroy() = 0;
    
	virtual void Lock() = 0;
    
	virtual void Unlock() = 0;    
    
	virtual uint32 GetLockCount(){return m_u32LockCount;}

    virtual MUTEXID GetId(){ return m_mutexId;}
    
PROTECTED:   
	uint32 m_u32LockCount;
    MUTEXID m_mutexId;
}; 

//����CThreadMutex
class CAutoMutex
{
PUBLIC:
	explicit CAutoMutex(CMutex& clMutex) : m_clMutex(clMutex)
        {m_clMutex.Lock();}
        
	~CAutoMutex()
        {m_clMutex.Unlock();}
        
PRIVATE:
	CMutex& m_clMutex;
};

//����CThreadMutex
class CThreadMutex : public CMutex
{
PUBLIC:    
    _enResult Init(MUTEXID mutexId);
    
    _enResult Destroy();
    
	void Lock();
    
	void Unlock();
    
PRIVATE:    
	pthread_mutex_t m_pthreadMutex;
}; 

//����CProcessMutex
class CProcessMutex : public CMutex
{
    _enResult Init(MUTEXID mutexId){return SUCC;}
    
    _enResult Destroy(){return SUCC;}
    
	void Lock(){}
    
	void Unlock(){}    
}; 

//����mutex�ڵ�
class CMutexNode
{
PUBLIC:
    CMutexNode(){pclMutex = NULL; memset(achFileName, 0, CPP_FILE_NAME_MAX_LEN);}
	CMutex* pclMutex;
	char achFileName[CPP_FILE_NAME_MAX_LEN];
	uint32 u32LineNum;
};

//����mutexӳ��        
typedef std::map<MUTEXID, CMutexNode> CMutexNodeMap;

//����CMutexMgr
class CMutexMgr : PUBLIC CComBase
{
    DECLARE_DEFAULT_COM_FUN;
        
PUBLIC:
	CMutex& AddMutex(MUTEXID mutexId, _enMutexType enMutexType);
    
	_enResult DelMutex(MUTEXID mutexId);
    
	CMutex& GetMutex(MUTEXID mutexId);
    
	void LogLock(CMutex& clMutex, const char* pchFileName, uint32 u32LineNum);
    
PRIVATE:
	CMutex& AddMutexInternal(MUTEXID mutexId, _enMutexType enMutexType);
    
	_enResult DelMutexInternal(MUTEXID mutexId);
    
	CMutex& GetMutexInternal(MUTEXID mutexId);
    
	_enResult AppendMutexNode(MUTEXID mutexId, CMutexNode& clMutexNode);
 
	_enResult EraseMutexNode(MUTEXID mutexId);
    
	uint32 m_thisMutexID;
    
	CThreadMutex m_clThreadMutex;
    
	CMutexNodeMap m_clMutexNodeMap;
};

#endif 
