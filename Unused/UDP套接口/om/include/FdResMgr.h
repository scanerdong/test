/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : FdResMgr.h
 * @version : 1.0.0
 * @brief : 定义系统内的文件描述符资源管理类
 *   
 * @author : zhangx
 * @date :  2011年10月28日 9:29:40
 * @history :
 *   
 */

#ifndef FD_RES_MGR_H
#define FD_RES_MGR_H

/************************引用部分*****************************/
#include <list>
#include "ComBase.h"
#include "MutexMgr.h"
#include "ResMgrBase.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
#define SOCK_OPEN(fd, domain, type, protocol)                             \
    do{                                                                   \
        char achBuf[PRINT_BUF_LEN];                                       \
        fd = socket(domain, type, protocol);                              \
        if(fd > 0)                                                        \
        {                                                                 \
            sprintf(achBuf, "Des: created at %s %d", __FILE__, __LINE__); \
            TSingleton<CFdResMgr>::Instance()->RegisterRes(fd, achBuf);   \
        }                                                                 \
    }while(0)
    
#define SOCK_CLOSE(fd)                                        \
    do{                                                       \
        close(fd);                                            \
        TSingleton<CFdResMgr>::Instance()->DeregisterRes(fd); \
    }while(0)
    
#define FILE_OPEN(fd, fileName, openMode)                               \
    do{                                                                 \
        char achBuf[PRINT_BUF_LEN];                                     \
        fd = open(fileName, openMode);                                  \
        if(fd > 0)                                                      \
        {                                                               \
            sprintf(achBuf, "Des: open at %s %d", __FILE__, __LINE__);  \
            TSingleton<CFdResMgr>::Instance()->RegisterRes(fd, achBuf); \
        }                                                               \
    }while(0)
    
#define FILE_CLOSE(fd)                                        \
    do{                                                       \
        close(fd);                                            \
        TSingleton<CFdResMgr>::Instance()->DeregisterRes(fd); \
    }while(0)
        
#define FP_OPEN(fp, fileName, openMode)                                  \
    do{                                                                  \
        char achBuf[PRINT_BUF_LEN];                                      \
        int32 fd;                                                        \
        fp = fopen(fileName, openMode);                                  \
        if(fp != NULL)                                                   \
        {                                                                \
            fd = fileno(fp);                                             \
            sprintf(achBuf, "Filename: %s, Des: fopen at %s %d", fileName, __FILE__, __LINE__); \
            TSingleton<CFdResMgr>::Instance()->RegisterRes(fd, achBuf);  \
        }                                                                \
    }while(0)
    
#define FP_CLOSE(fp)                                          \
    do{                                                       \
        int32 fd;                                             \
        fd = fileno(fp);                                      \
        fclose(fp);                                           \
        TSingleton<CFdResMgr>::Instance()->DeregisterRes(fd); \
    }while(0)

//CFdResMgr
class CFdDesNode
{
PUBLIC:
    CFdDesNode(int32 i32Fd, const string &strDes):m_i32Fd(i32Fd), m_strDes(strDes){}    
    int32 m_i32Fd;
    string m_strDes;
};

typedef std::list<CFdDesNode> CFdDesList;

class CFdResMgr : PUBLIC TResMgrBase<int32>, PUBLIC CComBase
{
    DECLARE_DEFAULT_COM_FUN;
    
PUBLIC:
    virtual _enResult RegisterRes(int32 i32Fd, const string& strDes);
    
    virtual _enResult DeregisterRes(int32 i32Fd);

PRIVATE:
    CFdDesList m_clFdDesList;

    MUTEXID m_thisMutexId;
};

#endif

