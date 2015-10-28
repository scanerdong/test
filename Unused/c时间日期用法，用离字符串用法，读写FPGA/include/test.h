#include "Common.h"

extern uint8 g_u8PrintCtrl;

/* 函数返回值定义 */
#define DEV_SUCCEED             (0)   /* 函数操作成功 */
#define DEV_FILE_OPEN_FAILED    (-1)  /* 打开设备文件失败 */
#define DEV_OP_FAIL             (-2)  /* 操作失败 */
#define DEV_INVALID_PARAM       (-3)  /* 无效参数 */

#define DEV_TEMPR_ANOMALY       (-10) /* 温度异常 */
#define DEV_CRC_FAILD           (-11) /* CRC检验失败 */
#define DEV_TIMEOUT             (-12) /* 操作超时 */

/* 这些返回值用于FPGA加载 */
#define DEV_ERR_FILE_NAME_IR    (-21) /* IR文件名错误 */
#define DEV_ERR_FILE_NAME_IF    (-22) /* IF文件名错误 */
#define DEV_ERR_FILE_EXIST_IR   (-23) /* IR文件不存在 */
#define DEV_ERR_FILE_EXIST_IF   (-24) /* IF文件不存在 */
#define DEV_ERR_TIMEOUT_IR      (-25) /* IR配置超时错误 */
#define DEV_ERR_TIMEOUT_IF      (-26) /* IF配置超时错误 */
#define DEV_ERR_FILE_DEV        (-27) /* 设备文件错误 */
#define DEV_ERR_INIT_LOW        (-28) /* FPGA硬件错误 */
#define DEV_ERR_NO_MEM          (-29) /* 没有内存 */

#define NTS_IOCTL_MAGIC	'G'	//ioctl magic number define
#define IOC_FPGA_READ_WORD		_IOR(NTS_IOCTL_MAGIC, 0x01, int)

