//����C++���麯�����ڴ��еķֲ�
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

typedef struct
{
    uint32 u32Year;         // �������
    uint32 u32Month;        // ������
    uint32 u32Day;          // ������
    uint32 u32Hour;         // ����Сʱ
    uint32 u32Minute;       // �������
}_stDate;

int main()
{
    _stDate stDate = {2008, 3, 15, 11, 55};
    _stDate stDate2 = {2011, 5, 16, 12, 45};
    _stDate stDateTemp, stDateTemp2;
    uint16 u16FileLen;
    FILE* pFile = NULL;
    uint8 achBuf[1024];

    //��ֻд��ʽ���ļ�
    pFile = fopen("./f/offline.txt", "wb");
    if(!pFile)
    {
        OM_ERROR("Create offline parameter filen failed!\n");
    }
    
    //�������߲���
    memcpy(achBuf, &stDate, sizeof(_stDate));
    memcpy(achBuf + sizeof(_stDate), &stDate2, sizeof(_stDate));    

    //������д�뵽�ļ���
    //fprintf(pFile, "%d,%d,%d", stDate.u32Day, stDate.u32Hour, stDate.u32Minute);
    fwrite(&achBuf, sizeof(_stDate), 2, stdout);
    fclose(pFile);

    //��ֻ����ʽ���ļ�
    pFile = fopen("./f/offline.txt", "rb");
    
    fseek(pFile, 0, SEEK_END);
    u16FileLen = ftell(pFile);
    OM_INFO("Size of offline config is %d, The file len is %d\n", sizeof(stDate), u16FileLen);

    //����ָ���Ƶ��ļ�ͷ���ֱ��ȡ������д������ݡ�
    rewind(pFile);
    fread(&stDateTemp, sizeof(_stDate), 1, pFile);
    fread(&stDateTemp2, sizeof(_stDate), 1, pFile);
    //fscanf(pFile, "%d,%d,%d", &stDateTemp.u32Day, &stDateTemp.u32Hour, &stDateTemp.u32Minute);
    printf("%d,%d,%d,%d", stDateTemp.u32Day, stDateTemp.u32Hour, stDateTemp2.u32Day, stDateTemp2.u32Hour);

    fclose(pFile);
	return 0;
	
}

