/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename      :  ComBase.h
 * @Version       :  1.0.0
 * @Brief           :  �������������Ļ���
 *   
 * @Author        :  zhangx
 * @CreatedDate:  2011��3��22�� 10:48:00
 * @History        : 
 *   
 */

#ifndef COM_BASE_H
#define COM_BASE_H

/************************���ò���*****************************/
#include <list>

#include "Common.h"

/************************ǰ����������***********************/


/************************���岿��*****************************/

#define DECLARE_DEFAULT_COM_FUN         \
PUBLIC:                                 \
	virtual _enResult Init();           \
	virtual _enResult Destroy();        \
	virtual _enResult Reset();          \
	virtual _enResult Run();            \
	virtual _enResult Dump(FILE* pf = NULL);

#define IMPLEMENT_DEFAULT_COM_FUN(className)                                          \
	_enResult className::Init(){OM_INFO("Init "#className".\n"); return SUCC;}        \
	_enResult className::Destroy(){OM_INFO("Destroy "#className".\n"); return SUCC;}  \
	_enResult className::Reset(){OM_INFO("Reset "#className".\n"); return SUCC;}      \
	_enResult className::Run(){OM_INFO("Run "#className".\n"); return SUCC;}          \
	_enResult className::Dump(FILE* pf){OM_INFO("Dump "#className".\n"); return SUCC;}


class CComBase
{
PUBLIC:
	virtual _enResult Init() = 0;			
    
	virtual _enResult Destroy() = 0;			
    
	virtual _enResult Reset() = 0;	
    
	virtual _enResult Run() = 0;		
    
	virtual _enResult Dump(FILE* pf) = 0;
};

typedef std::list<CComBase*> CComList;

#endif 

