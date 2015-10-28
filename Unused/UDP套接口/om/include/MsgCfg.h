/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MsgCfg.h
 * @version  : 1.0.0
 * @brief : 定义消息处理模块中使用的公共数据结构或宏
 *   
 * @author : zhangx
 * @date : 2011年3月30日 14:16:24
 * @history : 
 *   
 */

#ifndef MSG_CFG_H
#define MSG_CFG_H

/************************引用部分*****************************/
#include "Om.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/
const int32 PPP_INDEX_INITIAL = -1;
const int32 PPP_LEVEL_NUM = 8;

typedef uint32 PPPID;  						 //通信抽象设备唯一标识
typedef int32 STREAMFD;   				 	 //文件描述符
typedef uint8 PROTOCOLID;                     //协议唯一标识
typedef PROTOCOLID CommPPP[PPP_LEVEL_NUM];    //承载的协议传递路径

typedef struct
{
    PPPID pppId;
    CommPPP commPPP;
}_stPPPs;

const PROTOCOLID PROTOCOL_INVALID = 0;
const PROTOCOLID PROTOCOL_LMT = 1;
const PROTOCOLID PROTOCOL_SLIP = 7;
const PROTOCOLID PROTOCOL_IBM = 9;
const PROTOCOLID PROTOCOL_420ASYD = 10; 

const uint32 PPPID_LMT = 1;
const uint32 PPPID_SLIP = 2;
const uint32 PPPID_IBM = 3;
const uint32 PPPID_420ASYD = 4;

extern void GetPPPById(PPPID pppId, CommPPP commPPP);

#endif

