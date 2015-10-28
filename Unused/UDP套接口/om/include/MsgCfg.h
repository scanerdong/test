/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MsgCfg.h
 * @version  : 1.0.0
 * @brief : ������Ϣ����ģ����ʹ�õĹ������ݽṹ���
 *   
 * @author : zhangx
 * @date : 2011��3��30�� 14:16:24
 * @history : 
 *   
 */

#ifndef MSG_CFG_H
#define MSG_CFG_H

/************************���ò���*****************************/
#include "Om.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/
const int32 PPP_INDEX_INITIAL = -1;
const int32 PPP_LEVEL_NUM = 8;

typedef uint32 PPPID;  						 //ͨ�ų����豸Ψһ��ʶ
typedef int32 STREAMFD;   				 	 //�ļ�������
typedef uint8 PROTOCOLID;                     //Э��Ψһ��ʶ
typedef PROTOCOLID CommPPP[PPP_LEVEL_NUM];    //���ص�Э�鴫��·��

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

