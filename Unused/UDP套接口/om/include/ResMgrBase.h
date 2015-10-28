/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : ResMgrBase.h
 * @version : 1.0.0
 * @brief : 定义系统资源管理类
 *   
 * @author : zhangx
 * @date : 2011年4月20日 17:06:15
 * @history : 
 *   
 */

#ifndef RES_MGR_BASE_H
#define RES_MGR_BASE_H

/************************引用部分*****************************/
#include "Om.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
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

