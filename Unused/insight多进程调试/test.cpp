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
#include <sys/syscall.h>

using namespace std;

static int32 s_i32Count = 5;

void SysInit()
{
    TSingleton<CMutexMgr>::Instance()->Init();
    TSingleton<CMemMgr>::Instance()->Init();
    TSingleton<CFdResMgr>::Instance()->Init();
}

int32 main()
{
    pid_t pid;
    
    pid=fork();
    
    if(pid<0){
    perror("fail to fork");
    exit(1);
    }else if(pid==0){
    /*==================调试用的辅助代码=================*/
    int check=1;
    
    pid=getpid();
    printf("the id of child process is %u",pid);
    
    
    while(check==1) /*循环休眠等待调试*/
    sleep(5);
    /*==================辅助代码结束=================*/

    SysInit();

    printf("the child done/n");
    exit(0);
    }else{
    if(wait(NULL)==-1){ /*等待子进程执行结束*/
    perror("fail to wait");
    exit(1);
    }
    
    printf("the parent done/n");
    
    }
    
    return 0;
}
