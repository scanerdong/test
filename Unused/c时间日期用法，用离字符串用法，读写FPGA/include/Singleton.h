/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename      :  TSingleton.h
 * @Version       :  1.0.0
 * @Brief           :  定义单例模板类
 *   
 * @Author        :  zhangx
 * @CreatedDate:  2011年3月22日 14:28:25
 * @History        : 
 *   
 */

#ifndef SINGLETON_H
#define SINGLETON_H

/************************引用部分*****************************/
#include "Common.h"

/************************前向声明部分***********************/


/************************定义部分*****************************/
template <typename T>
class TSingleton
{
PUBLIC:
    static T* Instance()
    {
        static T*  theInstance = NULL;
        
        if(theInstance == NULL)
        {
            theInstance = new T();
        }
        return theInstance;
    }
    
PROTECTED:
    virtual ~TSingleton() {}
    
    TSingleton() {}
    
PRIVATE:
    TSingleton(const TSingleton&);
    
    TSingleton& operator = (const TSingleton&);    
};
	

#endif 
