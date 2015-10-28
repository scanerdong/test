#include "Common.h"


/* 函数返回值定义 */
#define DEV_SUCCEED             (0)   /* 函数操作成功 */
#define DEV_FILE_OPEN_FAILED    (-1)  /* 打开设备文件失败 */
#define DEV_OP_FAIL             (-2)  /* 操作失败 */
#define DEV_INVALID_PARAM       (-3)  /* 无效参数 */

#define SLIP_END            192    //八进制300  0xc0 
#define SLIP_ESC            219    //八进制333  0xdb
#define SLIP_ESC_END        220    //八进制334  0xdc
#define SLIP_ESC_ESC        221    //八进制335  0xdd
#define MAX_SLIP_BUF_LEN    1006   //协议建议发送和接收数据报文最大长度为1006字节



typedef struct 
{
    int32 *pi32FileId;            //文件描述符的指针
    const char  *pchFileName;     //设备文件
}_stDevFileInfo;


