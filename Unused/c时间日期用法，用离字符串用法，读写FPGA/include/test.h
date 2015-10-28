#include "Common.h"

extern uint8 g_u8PrintCtrl;

/* ��������ֵ���� */
#define DEV_SUCCEED             (0)   /* ���������ɹ� */
#define DEV_FILE_OPEN_FAILED    (-1)  /* ���豸�ļ�ʧ�� */
#define DEV_OP_FAIL             (-2)  /* ����ʧ�� */
#define DEV_INVALID_PARAM       (-3)  /* ��Ч���� */

#define DEV_TEMPR_ANOMALY       (-10) /* �¶��쳣 */
#define DEV_CRC_FAILD           (-11) /* CRC����ʧ�� */
#define DEV_TIMEOUT             (-12) /* ������ʱ */

/* ��Щ����ֵ����FPGA���� */
#define DEV_ERR_FILE_NAME_IR    (-21) /* IR�ļ������� */
#define DEV_ERR_FILE_NAME_IF    (-22) /* IF�ļ������� */
#define DEV_ERR_FILE_EXIST_IR   (-23) /* IR�ļ������� */
#define DEV_ERR_FILE_EXIST_IF   (-24) /* IF�ļ������� */
#define DEV_ERR_TIMEOUT_IR      (-25) /* IR���ó�ʱ���� */
#define DEV_ERR_TIMEOUT_IF      (-26) /* IF���ó�ʱ���� */
#define DEV_ERR_FILE_DEV        (-27) /* �豸�ļ����� */
#define DEV_ERR_INIT_LOW        (-28) /* FPGAӲ������ */
#define DEV_ERR_NO_MEM          (-29) /* û���ڴ� */

#define NTS_IOCTL_MAGIC	'G'	//ioctl magic number define
#define IOC_FPGA_READ_WORD		_IOR(NTS_IOCTL_MAGIC, 0x01, int)

