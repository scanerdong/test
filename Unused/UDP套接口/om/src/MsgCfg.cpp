/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MsgCfg.cpp
 * @version : 1.0.0
 * @brief : 定义消息管理模块中使用的配置项
 *   
 * @author : zhangx
 * @date : 2011年3月30日 14:17:26
 * @history : 
 *   
 */

/************************引用部分*****************************/
#include "MsgCfg.h"


/************************前向声明部分***********************/


/************************定义部分*****************************/

_stPPPs g_astPPPs[] = {
    {PPPID_LMT, {PROTOCOL_LMT, 0, 0, 0, 0, 0, 0, 0}},    
    {PPPID_SLIP, {PROTOCOL_SLIP, 0, 0, 0, 0, 0, 0}},
    {PPPID_IBM, {PROTOCOL_IBM, 0, 0, 0, 0, 0, 0}},
    {PPPID_420ASYD, {PROTOCOL_420ASYD, 0, 0, 0, 0, 0, 0}}
}; //此处可考虑动态配置TBD


/************************实现部分*****************************/
/**
 * @brief 根据抽象设备标识获取其协议传递路径
 * @param [in] pppId 抽象设备的唯一标识
 * @param [out] commPPP 协议传递路径
 * @return void
 * @author zhangx
 *  
 * @history 1. Created this function on 2011年3月30日 14:17:26
 *   
**/
void GetPPPById(PPPID pppId, CommPPP commPPP)
{
    memcpy(commPPP, g_astPPPs[pppId-1].commPPP, PPP_LEVEL_NUM);
}


