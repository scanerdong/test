/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename      :  TSingleton.h
 * @Version       :  1.0.0
 * @Brief           :  ���嵥��ģ����
 *   
 * @Author        :  zhangx
 * @CreatedDate:  2011��3��22�� 14:28:25
 * @History        : 
 *   
 */

#ifndef SINGLETON_H
#define SINGLETON_H

/************************���ò���*****************************/
#include "Common.h"

/************************ǰ����������***********************/


/************************���岿��*****************************/
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
