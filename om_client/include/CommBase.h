/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename       :  CommBase.h
 * @Version        :  1.0.0
 * @Brief            :  定义通信组件基类
 *   
 * @Author         :  zhangx
 * @CreatedDate :  2011年5月23日 15:34:28
 * @History         : 
 *   
 */

#ifndef COMM_BASE_H
#define COMM_BASE_H

/************************引用部分*****************************/
#include <map>

#include "Common.h"
#include "MutexMgr.h"
#include "MemMgr.h"
#include "MsgCfg.h"
#include "Stream.h"
#include "MsgQueue.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
typedef enum
{
    CONNECTION_STATE_OK = 0,
    CONNECTION_STATE_ERR    
}_enConectionState;

typedef std::list<CStream*> CStreamList;

class CCommBase
{        
PUBLIC:            
    virtual ~CCommBase()
    {
        CStreamList::iterator mi;
        CStream* pclStream;
        
        DELETE(m_pclMainStream);
        DEL_MUTEX(m_thisMutexId);
    }
    
    virtual void ResetStream()
    {
        m_pclMainStream->ResetFd();
    }
    
    string& GetName(){return m_strServiceName;}
    
    CStream* GetMainStream(){return m_pclMainStream;}
    
    virtual _enConectionState SetConnectionState(_enConectionState enConnectionState){m_enConnectionState = enConnectionState;}
    
    virtual _enConectionState GetConnectionState(){return m_enConnectionState;}
            
PROTECTED: 
	CCommBase(string& strServiceName, CStream* pclStream) 
        : m_strServiceName(strServiceName) , m_pclMainStream(pclStream)
    { 
        m_thisMutexId = (uint32)&m_thisMutexId;
        m_enConnectionState = CONNECTION_STATE_OK;
        ADD_MUTEX(m_thisMutexId);
    }
    
    MUTEXID m_thisMutexId;

    string m_strServiceName;
    
    CStream* m_pclMainStream;

    _enConectionState m_enConnectionState;
};

//CCommBuilderBase
class CCommBuilderBase
{
PUBLIC:        
    virtual _enResult Construct() = 0;
    
    virtual _enResult Destruct() = 0;    

    virtual void SetBuildParam(void* pParam) = 0;

    virtual void GetBuildParam(void* pParam) = 0;
    
    virtual CCommBase* GetComm() = 0;   
    
    virtual _enResult ResetComm() = 0;
};


#endif

