/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Common.h
 * @version : 1.0.0
 * @brief : 共同头文件和宏定义
 *   
 * @author : zhangx
 * @date : 2011年3月22日 11:03:39
 * @history : 
 *   
 */
 
#ifndef COMMON_H
#define COMMON_H

/************************引用部分*****************************/
#include "Types.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
#define TIME_STRING_LEN             (20)

typedef struct
{
    uint32 u32Year;
    uint32 u32Mon;
    uint32 u32Day;
    uint32 u32Hour;
    uint32 u32Min;
    uint32 u32Sec;
}_stTime;

extern _enResult fnTimeSetNow(_stTime* pstTime);

extern _enResult fnTimeGetNow(uint32* pu32Time);

extern _enResult fnTimeTime2String(uint32 u32Time, char achBuf[], uint32 u32BufLen);

extern _enResult fnTimeGmTime(uint32 u32Time, _stTime* pstTime);

#endif 

