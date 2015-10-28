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
#include "sqlite3.h"

using namespace std;

static int32 s_i32Count = 5;

void SysInit()
{
    TSingleton<CMutexMgr>::Instance()->Init();
    TSingleton<CMemMgr>::Instance()->Init();
    TSingleton<CFdResMgr>::Instance()->Init();
}

//执行sqlite3_exec时传入的空指针，argc是返回记录的列数，argv记录各个列的值，每个列的列名称表
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


int32 InitDatabase(const string& strDatabasePath, sqlite3** ppDb)
{
    int32 i32Ret;
    
    i32Ret = sqlite3_open(strDatabasePath.c_str(), ppDb);
    if(i32Ret != SQLITE_OK)
    {
        printf("open database failed: msg:%s\n", sqlite3_errmsg(*ppDb));
        sqlite3_close(*ppDb);
    }
}

int32 main()
{
    
    SysInit();

    int32 i32Ret;
    string strDbPath = "./test.db";
    sqlite3* pDb;
    string strSqlStatment;
    char* pchErrMsg;
    sqlite3_stmt* pStmt;
    int32 i32Id;
    char* pchName;
    
    InitDatabase(strDbPath, &pDb);

    strSqlStatment = "select * from student where sname = ?"; //?表示绑定参数
    i32Ret = sqlite3_prepare_v2(pDb, strSqlStatment.c_str(), -1, &pStmt, NULL);
    if( i32Ret!=SQLITE_OK )
    {
        sqlite3_finalize(pStmt);
        fprintf(stderr, "SQL error: %s\n", pchErrMsg);
        sqlite3_free(pchErrMsg);
    }
    else
    {
        //编译好的语句，SQL语句中绑定参数的位置（从1开始），要绑定的值，值的长度（为负数表示整个字符串长度）
        //值的类型（若为静态SQLITE_STATIC，则不执行值拷贝；若为动态SQLITE_STRANSIENT，则执行值拷贝以免数据丢失）
        sqlite3_bind_text(pStmt, 1, "wang", -1, SQLITE_STATIC);
        
        while(sqlite3_step(pStmt) == SQLITE_ROW)
        {
            if(SQLITE_INTEGER != sqlite3_column_type(pStmt, 0)) //0列的数据类型
            {
                printf("SQL ERROR: not integer\n");
            }
            else
            {
                i32Id = sqlite3_column_int(pStmt, 0);
            }
            
            if(SQLITE_TEXT != sqlite3_column_type(pStmt, 1)) //1列的数据类型
            {
                printf("SQL ERROR: not integer\n");
            }
            else
            {
                pchName = (char*)sqlite3_column_text(pStmt, 1);
            }

            printf("id:%d, name:%s\n", i32Id, pchName);
            
        }
        
        sqlite3_finalize(pStmt);
    }
    

/*
    i32Ret = sqlite3_exec(pDb, strSqlStatment.c_str(), callback, NULL, &pchErrMsg);
    if( i32Ret!=SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", pchErrMsg);
    }
*/    
    sqlite3_close(pDb);

    return 0;
}
