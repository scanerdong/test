/***************************************************************************
 *
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Common.cpp
 * @version : 1.0.0
 * @brief : 实现公共函数
 *
 * @author : zhangx
 * @date : 2011年3月30日 15:03:12
 * @history :
 *
 */

/************************引用部分*****************************/
#include "Common.h"
#include <time.h>
#include <stdio.h>
 

/************************前向声明部分***********************/


/************************定义部分*****************************/
#define DIV(a, b)	          ((a)/(b)-((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y)  (DIV(y,4) - DIV(y, 100) + DIV(y, 400))
#define ISLEAP(y)	          ((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

static const uint32 MON_DAY[2][13] =
{
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

/************************实现部分*****************************/
_enResult fnTimeSetNow(_stTime* pstTime)
{
    uint32 u32Year;
    uint32 u32Mon;
    uint32 u32Time;
    int32 i32Ret;
    
    u32Year = pstTime->u32Year;
    u32Mon = pstTime->u32Mon;

    if(0 >= (int32)(u32Mon -= 2))
    {
        u32Mon += 12;
        u32Year -= 1;
    }

    u32Time =((((uint32)(u32Year/4 - u32Year/100 + u32Year/400 + 367*u32Mon/12 + pstTime->u32Day)
        + u32Year*365 - 719499)*24 + pstTime->u32Hour)*60 + pstTime->u32Min)*60 + pstTime->u32Sec;

    i32Ret = stime((time_t*)&u32Time);
    
    return (i32Ret==0)?SUCC:FAIL;
}

_enResult fnTimeGetNow(uint32* pu32Time)
{
    if(pu32Time == NULL) return FAIL;

	time_t theTime = time(NULL);
	*pu32Time = (theTime + 8*3600);

    return SUCC;
}

_enResult fnTimeGmTime(uint32 u32Time, _stTime* pstTime)
{
    lint32 ilDays;
    lint32 ilYear;
    uint32 u32Rem;
    const uint32* pu32Ptr;

    if(NULL == pstTime || NULL == pstTime)  return FAIL;

    ilDays = u32Time / 86400;
    u32Rem = u32Time % 86400;
    pstTime->u32Hour = u32Rem /3600;
    u32Rem %= 3600;
    pstTime->u32Min = u32Rem / 60;
    pstTime->u32Sec = u32Rem % 60;

    ilYear = 1970;
    while(ilDays < 0 || ilDays >= (ISLEAP(ilYear)? 366:365))
    {
        lint32 il_yg = ilYear + ilDays/365 - (ilDays % 365 < 0);
        ilDays -= (il_yg - ilYear)*365;
        ilDays -= LEAPS_THRU_END_OF(il_yg - 1) - LEAPS_THRU_END_OF(ilYear -1);
        ilYear = il_yg;
    }

    pstTime->u32Year = ilYear;
    pu32Ptr = MON_DAY[ISLEAP(ilYear)];

    for(ilYear = 11; ilDays < pu32Ptr[ilYear]; --ilYear) continue;

    ilDays -= pu32Ptr[ilYear];
    pstTime->u32Mon = ilYear + 1;
    pstTime->u32Day = ilDays + 1;

    return SUCC;
}

_enResult fnTimeTime2String(uint32 u32Time, char achBuf[], uint32 u32BufLen)
{
	_stTime stTime;

    if(NULL == achBuf || u32BufLen < TIME_STRING_LEN) return FAIL;

    if(FAIL == fnTimeGmTime(u32Time, &stTime))
    {
        printf("fnTimeGmTime error!\n");
        return FAIL;
    }

    sprintf(achBuf, "%04d-%02d-%02d %02d:%02d:%02d", stTime.u32Year, stTime.u32Mon,
            stTime.u32Day, stTime.u32Hour, stTime.u32Min, stTime.u32Sec);

    return SUCC;
}



