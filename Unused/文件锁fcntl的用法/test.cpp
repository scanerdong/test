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

/*共享锁(或读锁)。
多个进程可以在文件的相同区域(或重叠)具有一个共享锁。
如果任何进程在文件的某一部分具有一个共享锁，其他的进程就不可以在相同的区域获得排他锁。
为了获得一个共享锁，文件必须使用读或是读写访问模式打开。*/
#define READ_LOCK(fd, offset, whence, len) \
    lockReg(fd, F_SETLKW, F_RDLCK, offset, whence, len)

/*排他锁(或写锁)。
在文件的某一个特定区域只可以有一个进程获得排他锁。
一旦有一个进程具有一个这样的锁，其他的进程就不可以在此区域上获得任何锁类型。
要获得一个排他锁，文件必须以写或是读写模式打开。*/
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
    
    pFile = fopen(pchFileName, "rb");//文件必须使用读或是读写模式打开
    if(NULL == pFile)
    {
        OM_INFO("fail\n");
        return -3;
    }
    
    i32Fd = fileno(pFile);

    printf("before read lock\n");
    i32LockRet = READ_LOCK(i32Fd, 0, SEEK_SET, 0);//表示锁住整个文件
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
    i32LockRet = UNLOCK(i32Fd, 0, SEEK_SET, 0);//表示对该进程在此文件上的所有锁进行解除
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
    
    pFile = fopen(pchFileName, "r+");//文件必须以写或是读写模式打开才能加写锁，同时保证其它进程不读到NULL，必须为r+模式打开，因为带w打开时，会将原有文件内容清空。参见《文件的多进程读写 新浪博客》
    if(NULL == pFile)
    {
        OM_INFO("fail\n");
        return -2;
    }
    
    //加写锁
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

    //写文件
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

    //解锁
    printf("before write unlock\n");
    i32LockRet = UNLOCK(i32Fd, 0, SEEK_SET, 0);//表示对该进程在此文件上的所有锁进行解除
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
    
    //设置心跳监控定时器
    stITimerVal.it_value.tv_sec = 1;
    stITimerVal.it_value.tv_usec = 0;
    stITimerVal.it_interval = stITimerVal.it_value;
    //setitimer(ITIMER_REAL, &stITimerVal, NULL);

    //signal(SIGALRM, SignalProcTimer); //测试CPU在执行阻塞型的系统调用被中断打断，并在中断结束后仍可继续执行该系统调用，而不是返回，参见linux.die.net/man/7/signal


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
