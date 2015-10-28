//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "Test.h"

static int32 s_i32TtyS2Dev    =   0;      // 基带板串口的文件标识符,基带板串口需要单独初始化

static _stDevFileInfo s_astDevFile[] =
{
    {&s_i32TtyS2Dev,    "/dev/ttyS2"},
};

static int32 fnDevCustPos(uint8 *pu8Buf, uint8 u8Value, uint32 u32Len)
{
    uint32 u32Index = 0;

    for(u32Index=0; u32Index < u32Len; u32Index++)
    {
        if(pu8Buf[u32Index] == u8Value)
        {
            return u32Index;
        }
    }

    return FAIL;
}

static int32 fnDevSlipDecode(uint8 *pu8Buf,int16 i16Len)
{
    int16 i16DesIndex = 0;
    int16 i16SrcIndex = 0;
    _enBool bNeedConv = FALSE; /*用来指示当前数据是否需要进行转义*/
    uint8 u8Temp=0;

    for(i16SrcIndex = 0; i16SrcIndex < i16Len; i16SrcIndex++ )
    {
        u8Temp = pu8Buf[i16SrcIndex];
        if(u8Temp == SLIP_ESC)
        {
            bNeedConv = TRUE;
            continue;
        }

        if(bNeedConv)
        {
            if (u8Temp == SLIP_ESC_END)
            {
                u8Temp = SLIP_END;
            }
            else if (u8Temp == SLIP_ESC_ESC)
            {
                u8Temp = SLIP_ESC;
            }
            else
            {
                //进入此分支说明发送方发送的数据格式不符合转义格式
                OM_ERROR("data format invalid!\n");
            }

            bNeedConv = FALSE;
        }

        pu8Buf[i16DesIndex] = u8Temp;
        i16DesIndex ++;
    }

    return i16DesIndex;
}

static int32 fnDevInitTtyS2(void)
{
    int32 i32Fd;
    struct termios stNewtio;
    char achInfo[256];

    //打开设备文件
    i32Fd = open(s_astDevFile[0].pchFileName, O_RDWR| O_NOCTTY|O_NDELAY);

    if (i32Fd == -1)
    {
        sprintf(achInfo, "open %s failure!\n", s_astDevFile[0].pchFileName);
        perror(achInfo);
        return FAIL;
    }
    else    //set to block;
    {
        fcntl(i32Fd, F_SETFL, 0);
    }

    // clear struct for new port settings
    bzero(&stNewtio, sizeof(stNewtio));

    //configure the serial port;
    cfsetispeed(&stNewtio, B115200);
    cfsetospeed(&stNewtio, B115200);
    stNewtio.c_cflag |= CLOCAL|CREAD; //保证本程序不会成为端口的所有者，从而妨碍控制工作和挂起信号
    //8N1
    stNewtio.c_cflag &= ~CSIZE;   // Mask the character size bits
    stNewtio.c_cflag |= CS8;      // 设置数据位
    stNewtio.c_cflag |= PARENB;   //进行奇偶校验
    stNewtio.c_cflag &= ~PARODD;  //偶校验
    stNewtio.c_cflag &= ~CSTOPB;  //送一个停止位
    stNewtio.c_cflag &= ~CRTSCTS; //禁止RTS/CTS（硬件）控制流
    stNewtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  //选择行方式输入:行式输入是不经处理的
    stNewtio.c_iflag &= ~(IXON | IXOFF | IXANY);  //取消软件流控制(不设置可能存在丢码)
    stNewtio.c_oflag &= ~OPOST;   //选择行式输出

    tcflush(i32Fd, TCIFLUSH);    // clear input buffer
    stNewtio.c_cc[VTIME] = 1;     // 100ms timeout
    stNewtio.c_cc[VMIN] = 0;      // blocking read until 0 character arrives
    tcsetattr(i32Fd, TCSANOW, &stNewtio);

    return i32Fd;
}


static int32 fnDevBBUartRead(uint8 *pu8Data, uint32 u32DataLen)
{
//#ifdef __DRIVER_API_TEST__
//    return 0;
//#endif
    int16 i16OverTime = 0;     /*接收数据包超时*/
    int16 i16RecvLen=0;
    int16 i16BufLen=0;        /* BUF中有效数据的长度*/
    uint8 au8Buf[MAX_SLIP_BUF_LEN*2];
    uint8 au8RecvBuf[MAX_SLIP_BUF_LEN + 2]; //加２表示２个分帧符

    int16 i16HeadPos = -1;
    int16 i16TrailPos = -1;
    _enBool bHaveMsgHead = FALSE;

    if (pu8Data == NULL)
        return DEV_INVALID_PARAM;

    if (s_i32TtyS2Dev <= 0)
    {
        s_i32TtyS2Dev = fnDevInitTtyS2();
        if(s_i32TtyS2Dev <= 0)
        {
            return DEV_FILE_OPEN_FAILED;
        }
    }

    memset(au8Buf, 0, MAX_SLIP_BUF_LEN);
    memset(au8RecvBuf, 0, MAX_SLIP_BUF_LEN);
    i16RecvLen = 0;
    i16BufLen = 0;

    while(i16OverTime < 10)
    {
        i16RecvLen = read(s_i32TtyS2Dev, au8RecvBuf, MAX_SLIP_BUF_LEN);
        if (i16RecvLen <= 0)
        {
            //OM_INFO("fnDevBBUartRead failed, i16RecvLen = %d\n", i16RecvLen);
            usleep(5*1000);
            i16OverTime++;
            //continue;
            return DEV_OP_FAIL;
        }
        
        if(bHaveMsgHead)
        {
            if(i16BufLen+i16RecvLen < MAX_SLIP_BUF_LEN*2)
            {
                memcpy(au8Buf+i16BufLen, au8RecvBuf, i16RecvLen);
                i16BufLen += i16RecvLen;

                //查找消息尾
                i16TrailPos = fnDevCustPos(au8Buf + 1, SLIP_END, i16BufLen - 1);
                if(i16TrailPos >= 0)
                {
                    /*最后的有效数据长度*/
                    i16BufLen = i16TrailPos + 2;
                    break;
                }
            }
        }
        else
        {
            /*查找消息头*/
            i16HeadPos = fnDevCustPos(au8RecvBuf, SLIP_END, i16RecvLen);
            if(i16HeadPos >= 0)
            {
                /*新的接收有效数据长度*/
                i16RecvLen -= i16HeadPos;
                memcpy(au8Buf, au8RecvBuf+i16HeadPos, i16RecvLen);
                i16BufLen = i16RecvLen;

                //查找消息尾(有可能消息头和消息尾都在同一个包中)
                i16TrailPos = fnDevCustPos(au8Buf + 1, SLIP_END, i16BufLen - 1);
                if(i16TrailPos >= 0)
                {
                    /*最后的有效数据长度*/
                    i16BufLen = i16TrailPos + 2;
                    break;
                }

                /*重新复位超时计数器*/
                i16OverTime = 0;
                bHaveMsgHead = TRUE;
            }
        }

        /*如果没有接收到完整的消息包,则休眠5毫秒*/
        usleep(10*1000);
        i16OverTime++;
    }/*end of while(i16Counter < 10)*/

    /*超时退出*/
    if (i16OverTime >= 10)
    {
        //OM_INFO("fnDevBBUartRead over time\n");
        return DEV_OP_FAIL;
    }
    
    /*如果接收到数据长度不正确,则退出*/
    if(i16BufLen < 2) //至少有２个分帧符
    {
        return DEV_OP_FAIL;
    }

    /*对接收到的数据进行解码,如果解码后的数据不满足要求，表示数据有错*/
    i16RecvLen = fnDevSlipDecode(au8Buf, i16BufLen);

    if(i16RecvLen < 2) //至少有２个分帧符
    {
        return DEV_OP_FAIL;
    }

    if(u32DataLen >= i16RecvLen - 2)
    {
        memcpy(pu8Data, au8Buf + 1, i16RecvLen - 2); //首尾两个字节为分帧字节，不拷贝
        return (i16RecvLen - 2);
    }
    else
    {
        return DEV_OP_FAIL;
    }
}

int32 main()
{
    uint8 u8UartData[600] = {0};
    int32 i32RecvLen = 0;

    while(1)
    {
        i32RecvLen = fnDevBBUartRead(u8UartData,600);
        if(i32RecvLen <= 0)
        {
            OM_INFO("-----------no data --------------\n");
            sleep(2);
            continue;
        }
        else
        {
            OM_INFO("the recv data is %s", u8UartData);

            OM_INFO("Slip RecvTask: ");
            PRINT_BUF(u8UartData, i32RecvLen);
        }

        sleep(2);
    }
    
    return 0;
}
