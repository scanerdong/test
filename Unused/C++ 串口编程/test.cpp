//����C++���麯�����ڴ��еķֲ�
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "Test.h"

static int32 s_i32TtyS2Dev    =   0;      // �����崮�ڵ��ļ���ʶ��,�����崮����Ҫ������ʼ��

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
    _enBool bNeedConv = FALSE; /*����ָʾ��ǰ�����Ƿ���Ҫ����ת��*/
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
                //����˷�֧˵�����ͷ����͵����ݸ�ʽ������ת���ʽ
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

    //���豸�ļ�
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
    stNewtio.c_cflag |= CLOCAL|CREAD; //��֤�����򲻻��Ϊ�˿ڵ������ߣ��Ӷ��������ƹ����͹����ź�
    //8N1
    stNewtio.c_cflag &= ~CSIZE;   // Mask the character size bits
    stNewtio.c_cflag |= CS8;      // ��������λ
    stNewtio.c_cflag |= PARENB;   //������żУ��
    stNewtio.c_cflag &= ~PARODD;  //żУ��
    stNewtio.c_cflag &= ~CSTOPB;  //��һ��ֹͣλ
    stNewtio.c_cflag &= ~CRTSCTS; //��ֹRTS/CTS��Ӳ����������
    stNewtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  //ѡ���з�ʽ����:��ʽ�����ǲ��������
    stNewtio.c_iflag &= ~(IXON | IXOFF | IXANY);  //ȡ�����������(�����ÿ��ܴ��ڶ���)
    stNewtio.c_oflag &= ~OPOST;   //ѡ����ʽ���

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
    int16 i16OverTime = 0;     /*�������ݰ���ʱ*/
    int16 i16RecvLen=0;
    int16 i16BufLen=0;        /* BUF����Ч���ݵĳ���*/
    uint8 au8Buf[MAX_SLIP_BUF_LEN*2];
    uint8 au8RecvBuf[MAX_SLIP_BUF_LEN + 2]; //�ӣ���ʾ������֡��

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

                //������Ϣβ
                i16TrailPos = fnDevCustPos(au8Buf + 1, SLIP_END, i16BufLen - 1);
                if(i16TrailPos >= 0)
                {
                    /*������Ч���ݳ���*/
                    i16BufLen = i16TrailPos + 2;
                    break;
                }
            }
        }
        else
        {
            /*������Ϣͷ*/
            i16HeadPos = fnDevCustPos(au8RecvBuf, SLIP_END, i16RecvLen);
            if(i16HeadPos >= 0)
            {
                /*�µĽ�����Ч���ݳ���*/
                i16RecvLen -= i16HeadPos;
                memcpy(au8Buf, au8RecvBuf+i16HeadPos, i16RecvLen);
                i16BufLen = i16RecvLen;

                //������Ϣβ(�п�����Ϣͷ����Ϣβ����ͬһ������)
                i16TrailPos = fnDevCustPos(au8Buf + 1, SLIP_END, i16BufLen - 1);
                if(i16TrailPos >= 0)
                {
                    /*������Ч���ݳ���*/
                    i16BufLen = i16TrailPos + 2;
                    break;
                }

                /*���¸�λ��ʱ������*/
                i16OverTime = 0;
                bHaveMsgHead = TRUE;
            }
        }

        /*���û�н��յ���������Ϣ��,������5����*/
        usleep(10*1000);
        i16OverTime++;
    }/*end of while(i16Counter < 10)*/

    /*��ʱ�˳�*/
    if (i16OverTime >= 10)
    {
        //OM_INFO("fnDevBBUartRead over time\n");
        return DEV_OP_FAIL;
    }
    
    /*������յ����ݳ��Ȳ���ȷ,���˳�*/
    if(i16BufLen < 2) //�����У�����֡��
    {
        return DEV_OP_FAIL;
    }

    /*�Խ��յ������ݽ��н���,������������ݲ�����Ҫ�󣬱�ʾ�����д�*/
    i16RecvLen = fnDevSlipDecode(au8Buf, i16BufLen);

    if(i16RecvLen < 2) //�����У�����֡��
    {
        return DEV_OP_FAIL;
    }

    if(u32DataLen >= i16RecvLen - 2)
    {
        memcpy(pu8Data, au8Buf + 1, i16RecvLen - 2); //��β�����ֽ�Ϊ��֡�ֽڣ�������
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
