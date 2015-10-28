#include "Common.h"


/* ��������ֵ���� */
#define DEV_SUCCEED             (0)   /* ���������ɹ� */
#define DEV_FILE_OPEN_FAILED    (-1)  /* ���豸�ļ�ʧ�� */
#define DEV_OP_FAIL             (-2)  /* ����ʧ�� */
#define DEV_INVALID_PARAM       (-3)  /* ��Ч���� */

#define SLIP_END            192    //�˽���300  0xc0 
#define SLIP_ESC            219    //�˽���333  0xdb
#define SLIP_ESC_END        220    //�˽���334  0xdc
#define SLIP_ESC_ESC        221    //�˽���335  0xdd
#define MAX_SLIP_BUF_LEN    1006   //Э�齨�鷢�ͺͽ������ݱ�����󳤶�Ϊ1006�ֽ�



typedef struct 
{
    int32 *pi32FileId;            //�ļ���������ָ��
    const char  *pchFileName;     //�豸�ļ�
}_stDevFileInfo;


