/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Stream.h
 * @version : 1.0.0
 * @brief : 定义通信设备基类
 *   
 * @author : zhangx
 * @date : 2011年4月2日 10:10:30
 * @history : 
 *   
 */

#ifndef STREAM_H
#define STREAM_H

/************************引用部分*****************************/
#include "Om.h"
#include "MsgCfg.h"

/************************前向声明部分***********************/


/************************定义部分*****************************/
const int32 INVALID_FD = -1;

//CStream
class CStream
{
PUBLIC:    
    virtual ~CStream(){}
    
    virtual int32 Send(const void *pBuf, uint32 u32BufLen) = 0;
    
    virtual int32 Recv(void *pBuf, uint32 u32BufLen) = 0;
    
    virtual _enResult ResetFd() = 0;

    virtual void CloseFd() = 0;
    
    int32 GetFd(){return m_streamFd;}
    
PROTECTED:
    CStream(){m_streamFd = INVALID_FD;}
    
    STREAMFD m_streamFd;    
};

#endif
