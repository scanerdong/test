/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Common.h
 * @version : 1.0.0
 * @brief : ��ͬͷ�ļ��ͺ궨��
 *   
 * @author : zhangx
 * @date : 2011��3��22�� 11:03:39
 * @history : 
 *   
 */
 
#ifndef COMMON_H
#define COMMON_H

/************************���ò���*****************************/
#include "Types.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/
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

