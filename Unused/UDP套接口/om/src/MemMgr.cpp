/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MemMgr.cpp
 * @version : 1.0.0
 * @brief : 实现内存管理类
 *   
 * @author : zhangx
 * @date : 2011年3月25日 10:46:43
 * @history : 
 *   
 */

/************************引用部分*****************************/
#include "MemMgr.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/


/************************实现部分*****************************/
_enResult CMemMgr::Init()
{    
    OM_INFO("Init CMemMgr.\n");
    
	m_thisMutexId = ADD_MUTEX();
	
	return SUCC;
}

_enResult CMemMgr::Destroy()
{    
    CMemNode* pclMemNode;
	CMemNodeMap::iterator mi;
    
    OM_INFO("Destroy CMemMgr.\n");
    
    for(mi = m_clMemNodeMap.begin(); mi != m_clMemNodeMap.end(); mi++)
    {
	    pclMemNode = (*mi).second;
        delete pclMemNode;
    }
    
    m_clMemNodeMap.clear();
    DEL_MUTEX(m_thisMutexId);
    
	return SUCC;
}

_enResult CMemMgr::Reset()
{
	return SUCC;
}

_enResult CMemMgr::Run()
{
	return SUCC;
}

_enResult CMemMgr::Dump(FILE* pf)
{    
    CMemNode* pclMemNode;
	CMemNodeMap::iterator mi;
    uint32 u32TotalSize = 0;
    
    AUTO_MUTEX(m_thisMutexId);
    
    for(mi = m_clMemNodeMap.begin(); mi != m_clMemNodeMap.end(); mi++)
    {
	    pclMemNode = (*mi).second;
        DUMP_PRINT(pf, "CMemMgr::Dump MemPtr %p, Type %s, Size %d, Alloc at %s %d\r\n", 
            pclMemNode->pMemPointer, pclMemNode->strType.c_str(), pclMemNode->u32Size, pclMemNode->pchFileName, pclMemNode->u32LineNum);
        u32TotalSize += pclMemNode->u32Size;
    }
    DUMP_PRINT(pf, "Total allocated memory size is %d byets!\r\n", u32TotalSize);
    
    return SUCC;
}

_enResult CMemMgr::AppendMemNode (void* pMemPointer, const char* pchFileName, uint32 u32LineNum, string strType, uint32 u32Size)
{
    AUTO_MUTEX(m_thisMutexId);

	CMemNode* pclMemNode;
	uint32 key = (uint32)(pMemPointer);
    std::pair<CMemNodeMap::iterator, bool> clInsertPair;

    pclMemNode = new CMemNode();
	pclMemNode->pMemPointer = pMemPointer;
    pclMemNode->pchFileName = pchFileName;
	pclMemNode->u32LineNum = u32LineNum;
    pclMemNode->strType = strType;
    pclMemNode->u32Size = u32Size;

	clInsertPair = m_clMemNodeMap.insert(CMemNodeMap::value_type(key, pclMemNode));	
	if(clInsertPair.second == true)
	{	
		return SUCC;
	}
	else
	{
		return FAIL;
	}
}

_enResult CMemMgr::EraseMemNode(void* pMemPointer, const char* pchFileName, uint32 u32LineNum)
{
    AUTO_MUTEX(m_thisMutexId);
	
	CMemNodeMap::iterator mi;
	uint32 key = (uint32)(pMemPointer);
    
	mi = m_clMemNodeMap.find(key);	
	if(mi != m_clMemNodeMap.end())
	{
        delete (mi->second);
	    m_clMemNodeMap.erase(mi);
        
        return SUCC;
	}
    
    return FAIL;
}

#ifdef __MEM_MGR_TEST__
	
class CTest
{
public:
	CTest(int32 a){m_a = a; m_b = 0;}
	CTest(int32 a, int32 b){m_a = a; m_b = b;}
	void print(){printf("m_a %d, m_b %d\n", m_a, m_b);}
private:
	int32 m_a;	
	int32 m_b;
};
	
void* MonitorMemTask(void * pParam)
{
    pthread_detach(pthread_self());
    while(1)
    {
        sleep(2);
        cout<<"Begin monitor mem--------"<<endl;
        TSingleton<CMemMgr>::Instance()->Dump(NULL);
        cout<<"End monitor mem--------"<<endl;
    }
    return NULL;
}
        
void* ResumeMemTask(void * pParam)
{
    CTest* pclTest;
    char* pchTest;
    
    pthread_detach(pthread_self());
    while(1)
    {
        sleep(3);
        
        cout<<"Before malloc-------"<<endl;
        NEW(pclTest, CTest, 10, 20);
        MALLOC(pchTest, char, 100);
        sleep(5);
        DELETE(pclTest);
        FREE(pclTest);
        cout<<"After malloc------"<<endl;
    }
    return NULL;
}

int32 main()
{    
    CTest *p1, *p2, *p3;
    char* pch1;
	CMemMgr* pclMemMgr = TSingleton<CMemMgr>::Instance();
    TSingleton<CMutexMgr>::Instance()->Init();    

    pclMemMgr.Init();
	pclMemMgr.Dump(NULL);
    NEW(p1, CTest, 1, 1);    
    NEW(p2, CTest, 2, 2);
    NEW(p3, CTest, 3, 3);
    p1->print();
    p2->print();
    p3->print();
	pclMemMgr.Dump(NULL);
    
    DELETE(p1);    
    MALLOC(pch1, char, 10);
    p2->print();
    p3->print();
	pclMemMgr.Dump(NULL);

    FREE(pch1);
    DELETE(p2);    
    DELETE(p3);
	pclMemMgr.Dump(NULL);
    
	pthread_t m_resumeThreadId;
    pthread_create(&m_resumeThreadId, 0, ResumeMemTask, 0);
    
	pthread_t m_monitorThreadId;
    pthread_create(&m_monitorThreadId, 0, MonitorMemTask, 0);
	
    while(1) sleep(10);

	return 0;
}
#endif
