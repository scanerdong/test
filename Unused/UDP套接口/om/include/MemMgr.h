/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MemMgr.h
 * @version : 1.0.0
 * @brief : �����ڴ������
 *   
 * @autho : zhangx
 * @date :  2011��3��22�� 15:20:37
 * @history : 
 *   
 */

#ifndef MEM_MGR_H
#define MEM_MGR_H

/************************���ò���*****************************/
#include <map>
#include <typeinfo>

#include "Om.h"
#include "Singleton.h"
#include "MutexMgr.h"
		
/************************ǰ����������***********************/


/************************���岿��*****************************/
	
//����ģ��ͨ�����к�����ڴ�ķ�����ͷ�
#ifdef __DEBUG__

#define NEW(ptr, type, ...) 	 \
	do{							 \
    ptr = new type(__VA_ARGS__); \
	TSingleton<CMemMgr>::Instance()->AppendMemNode(ptr, __FILE__, __LINE__, typeid(type).name(), sizeof(type));}while(0)

#define DELETE(ptr) 															                 \
    do{                                                                                          \
    if(ptr){delete ptr; TSingleton<CMemMgr>::Instance()->EraseMemNode(ptr, __FILE__, __LINE__);} \
    ptr = 0;}while(0)
        
#define MALLOC(ptr, type, size) 		                 \
    do{                                                  \
    uint32 u32RawSize = MemRoundUp((size)*sizeof(type)); \
    ptr = (type*)calloc(1, u32RawSize);                  \
    TSingleton<CMemMgr>::Instance()->AppendMemNode(ptr, __FILE__, __LINE__, typeid(type).name(), u32RawSize);}while(0)
        
#define FREE(ptr) 																                \
    do{                                                                                         \
    if(ptr){free(ptr); TSingleton<CMemMgr>::Instance()->EraseMemNode(ptr, __FILE__, __LINE__);} \
    ptr = 0;}while(0)

#else

#define NEW(ptr, type, ...) 	 \
	do{							 \
    ptr = new type(__VA_ARGS__); \
    }while(0)
    
#define DELETE(ptr) 	  \
    do{                   \
    if(ptr){delete ptr; } \
    ptr = 0;}while(0)
        
#define MALLOC(ptr, type, size) 		                 \
    do{                                                  \
    uint32 u32RawSize = MemRoundUp((size)*sizeof(type)); \
    ptr = (type*)calloc(1, u32RawSize);                  \
    }while(0)
        
#define FREE(ptr) 	     \
    do{                  \
    if(ptr){free(ptr); } \
    ptr = 0;}while(0)
    
#endif



//�����ڴ������
static const uint8 MEM_DATA_ALIGN = 4;
inline uint32 MemRoundUp(uint32 u32Size){return (((u32Size) + MEM_DATA_ALIGN-1) & ~(MEM_DATA_ALIGN-1));}
        
class CMemNode
{
PUBLIC:
    CMemNode(){pMemPointer = NULL; pchFileName = NULL; u32LineNum = 0; u32Size = 0;}
    void* pMemPointer;
    const char* pchFileName;
    uint32 u32LineNum;
    string strType;
    uint32 u32Size;
};

typedef std::map<uint32, CMemNode*> CMemNodeMap;
typedef CMemNodeMap::iterator CMemNodeMapIter;

class CMemMgr : PUBLIC CComBase
{
    DECLARE_DEFAULT_COM_FUN;
    
PUBLIC:
	_enResult AppendMemNode(void* p, const char* pchFileName, uint32 u32LineNum, string strType, uint32 u32Size);
    
    _enResult EraseMemNode(void* p, const char* pchFileName, uint32 u32LineNum);
    
PRIVATE:
	MUTEXID m_thisMutexId;
    
	CMemNodeMap m_clMemNodeMap;
};

#endif 
