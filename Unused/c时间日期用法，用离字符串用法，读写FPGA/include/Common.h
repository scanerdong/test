/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename      :  Common.h
 * @Version       :  1.0.0
 * @Brief           :  共同头文件和宏定义
 *   
 * @Author        :  zhangx
 * @CreatedDate:  2011年3月22日 11:03:39
 * @History        : 
 *   
 */
 
#ifndef COMMON_H
#define COMMON_H

/************************引用部分*****************************/
//系统头文件部分
#include <unistd.h>          //sleep, close的定义
#include <sys/types.h>
#include <ctype.h>

#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>      //ip协议地址的转换函数
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/telnet.h>    //telnet相关的声明
#include <arpa/inet.h>

#include <math.h>
#include <termios.h>

#include <pthread.h>
#include <signal.h>

#include <sys/shm.h>
#include <fcntl.h>
#include <sys/wait.h>      //waitpid
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "errno.h"

#include <algorithm>
#include <string>

//Cplusplus头文件
#include <cstdio>
#include <string>


//项目公共头文件部分
#include "Types.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
typedef struct
{
    uint32 u32Year;
    uint32 u32Mon;
    uint32 u32Day;
    uint32 u32Hour;
    uint32 u32Min;
    uint32 u32Sec;
}_stTime;

using namespace std;

const uint32 CPP_FILE_NAME_MAX_LEN = 32;
const uint32 MAX_FILE_NAME_LEN = 260;

const int32 INVALID_SOCKET = -1;
const int32 SOCKET_ERROR = -1;

const uint32 PRINT_BUF_LEN = 1024;
const uint32 MIN_TIME_STR_LEN = 20;

#ifdef _DEBUG_
#define ASSERT(n) assert(n)
#else
#define ASSERT(n) ((void)0)
#endif

#ifndef _UNIT_TEST_
#define PUBLIC public
#define PROTECTED protected
#define PRIVATE private
#else
#define PUBLIC public
#define PROTECTED public
#define PRIVATE public
#endif

#define OM_ERROR printf
#define OM_INFO  printf
#define OM_DEBUG printf

#define OM_LEVEL_DEBUG  (1)
#define OM_LEVEL_INFO   (2)
#define OM_LEVEL_ERROR  (3)

#define OM_DEBUG_LEVEL OM_LEVEL_DEBUG

extern uint8 g_u8PrintCtrl;

#if (OM_DEBUG_LEVEL == OM_LEVEL_DEBUG)
#undef OM_DEBUG
#undef OM_INFO
#undef OM_ERROR
#define OM_DEBUG(...) do{ if( (g_u8PrintCtrl&0x01) == 0x01){printf("OM_DEBUG: "); printf(__VA_ARGS__);} }while(0)
#define OM_INFO(...)  do{ if( (g_u8PrintCtrl&0x02) == 0x02){printf("OM_INFO: "); printf(__VA_ARGS__);} }while(0)
#define OM_ERROR(...) do{ if( (g_u8PrintCtrl&0x04) == 0x04){printf("OM_ERROR: "); printf("File: %s, Line: %d, ", __FILE__, __LINE__); printf(__VA_ARGS__);} }while(0)
#endif

#if (OM_DEBUG_LEVEL == OM_LEVEL_INFO)
#undef OM_INFO
#undef OM_ERROR
#define OM_INFO(...)  do{ if( (g_u8PrintCtrl&0x02) == 0x02){printf("OM_INFO: "); printf(__VA_ARGS__);} }while(0)
#define OM_ERROR(...) do{ if( (g_u8PrintCtrl&0x04) == 0x04){printf("OM_ERROR: "); printf("File: %s, Line: %d, ", __FILE__, __LINE__); printf(__VA_ARGS__);} }while(0)
#endif

#if (OM_DEBUG_LEVEL == OM_LEVEL_ERROR)
#undef OM_ERROR
#define OM_ERROR(...) do{ if( (g_u8PrintCtrl&0x04) == 0x04){printf("OM_ERROR: "); printf("File: %s, Line: %d, ", __FILE__, __LINE__); printf(__VA_ARGS__);} }while(0)
#endif

#define LOG_USER_INFO


inline void PRINT_BUF(void* const pBuf, uint32 u32Len)           
{
	uint32 u32I;                               
	int8* pi8BufTemp = (int8*)(pBuf);     

    printf("PRINT_BUF:");                     
	for(u32I = 0; u32I < u32Len; u32I++)         
	{
	   printf("0x%02x,", (uint8)pi8BufTemp[u32I]);
	}
	printf("\r\n");
}

inline void SPRINT_BUF(char* pchPrintBuf, void* const pBuf, uint32 u32Len)
{
	uint32 u32I;                               
	int8* pi8BufTemp = (int8*)(pBuf);     
    char pchTemp[20];
    
	for(u32I = 0; u32I < u32Len; u32I++)         
	{
	   sprintf(pchTemp, "0x%02x,", (uint8)pi8BufTemp[u32I]);
       strcat(pchPrintBuf, pchTemp);
	}
	strcat(pchPrintBuf, "\n");                          
}

inline int32 DUMP_PRINT(FILE* pf, const char* fmt, ...)
{
    char achBuf[1024];
	va_list args;
	int32 i32Count;

	va_start(args, fmt);
	i32Count = vsprintf(achBuf, fmt, args);
	va_end(args);
	if(pf == NULL)
	{
		fwrite(achBuf, 1, i32Count, stdout);
	}
	else
	{
		fwrite(achBuf, 1, strlen(achBuf), pf);
	}
    
	return i32Count;
}
#endif 
