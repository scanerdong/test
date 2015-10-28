/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : MsgCfg.cpp
 * @version : 1.0.0
 * @brief : ������Ϣ����ģ����ʹ�õ�������
 *   
 * @author : zhangx
 * @date : 2011��3��30�� 14:17:26
 * @history : 
 *   
 */

/************************���ò���*****************************/
#include "MsgCfg.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/

_stPPPs g_astPPPs[] = {
    {PPPID_LMT, {PROTOCOL_LMT, 0, 0, 0, 0, 0, 0, 0}},    
    {PPPID_SLIP, {PROTOCOL_SLIP, 0, 0, 0, 0, 0, 0}},
    {PPPID_IBM, {PROTOCOL_IBM, 0, 0, 0, 0, 0, 0}},
    {PPPID_420ASYD, {PROTOCOL_420ASYD, 0, 0, 0, 0, 0, 0}}
}; //�˴��ɿ��Ƕ�̬����TBD


/************************ʵ�ֲ���*****************************/
/**
 * @brief ���ݳ����豸��ʶ��ȡ��Э�鴫��·��
 * @param [in] pppId �����豸��Ψһ��ʶ
 * @param [out] commPPP Э�鴫��·��
 * @return void
 * @author zhangx
 *  
 * @history 1. Created this function on 2011��3��30�� 14:17:26
 *   
**/
void GetPPPById(PPPID pppId, CommPPP commPPP)
{
    memcpy(commPPP, g_astPPPs[pppId-1].commPPP, PPP_LEVEL_NUM);
}


