/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : ResMgrBase.h
 * @version : 1.0.0
 * @brief : ����ϵͳ��Դ������
 *   
 * @author : zhangx
 * @date : 2011��4��20�� 17:06:15
 * @history : 
 *   
 */

#ifndef RES_MGR_BASE_H
#define RES_MGR_BASE_H

/************************���ò���*****************************/
#include "Om.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/
//TResMgrBase
template <typename T>
class TResMgrBase
{    
PUBLIC:
    virtual ~TResMgrBase(){}
    
PROTECTED:
    TResMgrBase(){}
    
    virtual _enResult RegisterRes(T, const string&) = 0;
    
    virtual _enResult DeregisterRes(T) = 0;
    
    virtual _enResult Dump(FILE* pf) = 0;    
};
    
#endif

