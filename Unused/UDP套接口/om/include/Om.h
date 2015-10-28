/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : Om.h
 * @version : 1.0.0
 * @brief : ����OM��ʹ�õ��Ĺ����궨��
 *   
 * @author : zhangx
 * @date :  2011��8��2�� 12:52:16
 * @history :
 *   
 */

#ifndef OM_H
#define OM_H

/************************���ò���*****************************/
//ϵͳͷ�ļ�����
#include <unistd.h>          //sleep, close�Ķ���
#include <sys/types.h>
#include <ctype.h>

#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>      //ipЭ���ַ��ת������
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/telnet.h>    //telnet��ص�����
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

#include <sys/time.h>

//Cplusplusͷ�ļ�
#include <cstdio>


//��Ŀ����ͷ�ļ�����
#include "Types.h"
#include "Common.h"


/************************ǰ����������***********************/
extern uint8 g_u8PrintCtrl;


/************************���岿��*****************************/
using namespace std;

const uint32 CPP_FILE_NAME_MAX_LEN = 32;
const uint32 MAX_FILE_NAME_LEN = 260;

const int32 INVALID_SOCKET = -1;
const int32 SOCKET_ERROR = -1;

const uint32 PRINT_BUF_LEN = 1024;

#ifdef __DEBUG__
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


#define OM_ERROR 
#define OM_INFO  
#define OM_DEBUG 

#define OM_LEVEL_DEBUG  (1)
#define OM_LEVEL_INFO   (2)
#define OM_LEVEL_ERROR  (3)

#define OM_DEBUG_LEVEL OM_LEVEL_DEBUG

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
	i32Count = vsnprintf(achBuf, 1024, fmt, args);
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

