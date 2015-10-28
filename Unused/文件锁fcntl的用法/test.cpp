#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "Socket.h"
#include "MutexMgr.h"
#include "MemMgr.h"
#include "FdResMgr.h"
#include "test.h"
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/types.h>
#include <fcntl.h>
using namespace std;

static int32 s_i32Count = 5;

int32 g_i32haha = 10;

void SysInit()
{
    TSingleton<CMutexMgr>::Instance()->Init();
    TSingleton<CMemMgr>::Instance()->Init();
    TSingleton<CFdResMgr>::Instance()->Init();
}

int32 lockReg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return fcntl(fd, cmd, &lock);
}

/*������(�����)��
������̿������ļ�����ͬ����(���ص�)����һ����������
����κν������ļ���ĳһ���־���һ���������������Ľ��̾Ͳ���������ͬ����������������
Ϊ�˻��һ�����������ļ�����ʹ�ö����Ƕ�д����ģʽ�򿪡�*/
#define READ_LOCK(fd, offset, whence, len) \
    lockReg(fd, F_SETLKW, F_RDLCK, offset, whence, len)

/*������(��д��)��
���ļ���ĳһ���ض�����ֻ������һ�����̻����������
һ����һ�����̾���һ�����������������Ľ��̾Ͳ������ڴ������ϻ���κ������͡�
Ҫ���һ�����������ļ�������д���Ƕ�дģʽ�򿪡�*/
#define WRITE_LOCK(fd, offset, whence, len) \
    lockReg(fd, F_SETLKW, F_WRLCK, offset, whence, len)

#define UNLOCK(fd, offset, whence, len) \
    lockReg(fd, F_SETLKW, F_UNLCK, offset, whence, len)


int32 testRead(char* pchFileName)
{
    int32 i32VerNo = -1;
    int32 i32LockRet;
    int32 i32Fd;
    FILE* pFile = NULL;
    char achLineBuf[1024];
    
    pFile = fopen(pchFileName, "rb");//�ļ�����ʹ�ö����Ƕ�дģʽ��
    if(NULL == pFile)
    {
        OM_INFO("fail\n");
        return -3;
    }
    
    i32Fd = fileno(pFile);

    printf("before read lock\n");
    i32LockRet = READ_LOCK(i32Fd, 0, SEEK_SET, 0);//��ʾ��ס�����ļ�
    if(i32LockRet != 0)
    {
        OM_ERROR("read lock file fail, ret %d\n", i32LockRet);
        perror("READ_LOCK");
        return -5;
    }
    printf("after read lock\n");

    if(fscanf(pFile,"verno=%d\n",&i32VerNo) < 1)
    {
        OM_ERROR("read number error\n");
        
        fgets(achLineBuf, sizeof(achLineBuf), pFile);
        OM_ERROR("achlinebuf %s\n", achLineBuf);
        
        perror("fscanf");
        fclose(pFile);
        return -1;
    }
    printf("read i32VerNo %d\n", i32VerNo);
    sleep(5);
    
    printf("before read unlock\n");
    i32LockRet = UNLOCK(i32Fd, 0, SEEK_SET, 0);//��ʾ�Ըý����ڴ��ļ��ϵ����������н��
    if(i32LockRet != 0)
    {
        OM_ERROR("unlock file fail\n");
        return -6;
    }
    printf("after read unlock\n");
    
    fclose(pFile);

    return 0;
}


int32 testWrite(char* pchFileName)
{
    int32 i32LockRet;
    int32 i32Fd;
    FILE* pFile = NULL;
    static int32 s_i32I = 0;
    int32 i32VerNo = -1;
    
    pFile = fopen(pchFileName, "r+");//�ļ�������д���Ƕ�дģʽ�򿪲��ܼ�д����ͬʱ��֤�������̲�����NULL������Ϊr+ģʽ�򿪣���Ϊ��w��ʱ���Ὣԭ���ļ�������ա��μ����ļ��Ķ���̶�д ���˲��͡�
    if(NULL == pFile)
    {
        OM_INFO("fail\n");
        return -2;
    }
    
    //��д��
    i32Fd = fileno(pFile);

    printf("before write lock\n");
    i32LockRet = WRITE_LOCK(i32Fd,  0, SEEK_SET, 0);
    if(i32LockRet != 0)
    {
        OM_ERROR("write lock file fail, ret %d\n", i32LockRet);
        perror("READ_LOCK");
        return -7;
    }
    printf("after write lock\n");

    //д�ļ�
    if(fscanf(pFile,"verno=%d\n",&i32VerNo) < 1)
    {
        OM_ERROR("read number error\n");
        perror("fscanf");
        fclose(pFile);
        return -1;
    }

    i32VerNo++;
    
    printf("write i32VerNo %d\n", i32VerNo);
    
    rewind(pFile);
    fprintf(pFile, "verno=%d\n", i32VerNo);
    fflush(pFile);
    sleep(5);

    //����
    printf("before write unlock\n");
    i32LockRet = UNLOCK(i32Fd, 0, SEEK_SET, 0);//��ʾ�Ըý����ڴ��ļ��ϵ����������н��
    if(i32LockRet != 0)
    {
        OM_ERROR("unlock file fail\n");
        return -8;
    }
    printf("after write unlock\n");
    
    fclose(pFile);

    return 0;
}


static void SignalProcTimer(int32 i32SigNo)
{
    char achInfo[100] = {0};

    if(SIGALRM == i32SigNo)
    {
        sprintf(achInfo, "SIGALRM arrived!\r\n");
        OM_INFO("%s", achInfo);
    }
    else
    {
        OM_ERROR("The signal handle function not math the signo!\n");
    }
}

int32 main()
{
    
    SysInit();
    char achData[50];


    struct itimerval stITimerVal;
    int32 i32SleepRet;
    
    //����������ض�ʱ��
    stITimerVal.it_value.tv_sec = 1;
    stITimerVal.it_value.tv_usec = 0;
    stITimerVal.it_interval = stITimerVal.it_value;
    //setitimer(ITIMER_REAL, &stITimerVal, NULL);

    //signal(SIGALRM, SignalProcTimer); //����CPU��ִ�������͵�ϵͳ���ñ��жϴ�ϣ������жϽ������Կɼ���ִ�и�ϵͳ���ã������Ƿ��أ��μ�linux.die.net/man/7/signal


    for(int32 i32I = 0; ;i32I++)
    {
        if(testRead("haha.txt") != 0)
        {
            system("killall om_11");
            return -1;
        }

        usleep(1000);
        
        if(testWrite("haha.txt") != 0)
        {
            system("killall om_11");
            return -2;
        }
    }


    
    while(1) 
    {
        OM_INFO("start sleep, time: %u\n", time(NULL));
        i32SleepRet = sleep(11);
        if(i32SleepRet != 0)
        {
            OM_INFO("sleep has been breaked, remain %d\n", i32SleepRet);
        }
        OM_INFO("stop sleep, time: %u\n", time(NULL));
    }
    
    
    return 0;
}
