/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Stream.h
 * @version : 1.0.0
 * @brief : ����ͨ���豸����
 *   
 * @author : zhangx
 * @date : 2011��4��2�� 10:10:30
 * @history : 
 *   
 */

#ifndef STREAM_H
#define STREAM_H

/************************���ò���*****************************/
#include "Om.h"
#include "MsgCfg.h"

/************************ǰ����������***********************/


/************************���岿��*****************************/
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
