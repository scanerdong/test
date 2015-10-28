//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

typedef struct
{
    uint32 u32Year;         // 定标年份
    uint32 u32Month;        // 定标月
    uint32 u32Day;          // 定标天
    uint32 u32Hour;         // 定标小时
    uint32 u32Minute;       // 定标分钟
}_stDate;

int main()
{
    _stDate stDate = {2008, 3, 15, 11, 55};
    _stDate stDate2 = {2011, 5, 16, 12, 45};
    _stDate stDateTemp, stDateTemp2;
    uint16 u16FileLen;
    FILE* pFile = NULL;
    uint8 achBuf[1024];

    //以只写方式打开文件
    pFile = fopen("./f/offline.txt", "wb");
    if(!pFile)
    {
        OM_ERROR("Create offline parameter filen failed!\n");
    }
    
    //设置离线参数
    memcpy(achBuf, &stDate, sizeof(_stDate));
    memcpy(achBuf + sizeof(_stDate), &stDate2, sizeof(_stDate));    

    //将数据写入到文件中
    //fprintf(pFile, "%d,%d,%d", stDate.u32Day, stDate.u32Hour, stDate.u32Minute);
    fwrite(&achBuf, sizeof(_stDate), 2, stdout);
    fclose(pFile);

    //以只读方式打开文件
    pFile = fopen("./f/offline.txt", "rb");
    
    fseek(pFile, 0, SEEK_END);
    u16FileLen = ftell(pFile);
    OM_INFO("Size of offline config is %d, The file len is %d\n", sizeof(stDate), u16FileLen);

    //这里指针移到文件头，分别读取这两块写入的数据。
    rewind(pFile);
    fread(&stDateTemp, sizeof(_stDate), 1, pFile);
    fread(&stDateTemp2, sizeof(_stDate), 1, pFile);
    //fscanf(pFile, "%d,%d,%d", &stDateTemp.u32Day, &stDateTemp.u32Hour, &stDateTemp.u32Minute);
    printf("%d,%d,%d,%d", stDateTemp.u32Day, stDateTemp.u32Hour, stDateTemp2.u32Day, stDateTemp2.u32Hour);

    fclose(pFile);
	return 0;
	
}

