#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "msgproc.h"
#include "test.h"

using namespace std;

_enResult WriteDataToFile(const char* pchFileName, const uint8* pu8FileData, const uint32 u32FileDataLen)
{
    size_t sizeWriteItemCount = 0;
    FILE* pFile = NULL;
    
    //打开文件
    pFile = fopen(pchFileName, "wb+");
    if(pFile == NULL)
    {
        OM_ERROR("open file error in %s!\n", __FUNCTION__);
        return FAIL;
    }
    
    //将文件数据写入
    sizeWriteItemCount = fwrite(pu8FileData, u32FileDataLen, 1, pFile);
    if(sizeWriteItemCount != 1)
    {
        OM_ERROR("write error in %s!\n", __FUNCTION__);
        fclose(pFile);
        return FAIL;
    }
    else
    {
        OM_INFO("Write Success!\n");
        fclose(pFile);
    }

    return SUCC;
}

_enResult ReadDataFromFile(const char* pchFileName, uint8* pu8FileData, const uint32 u32FileDataLen, uint32 u32StartPos, uint32* pu32ReadLen)
{
    size_t sizeReadCount = 0;
    FILE* pFile =NULL;
    
    //打开文件
    pFile = fopen(pchFileName, "rb");
    if(pFile == NULL)
    {
        OM_ERROR("open file error in %s!\n", __FUNCTION__);
        return FAIL;
    }
    
    //偏移指针    
    fseek (pFile, u32StartPos, SEEK_SET);

    //读取数据
    sizeReadCount = fread(pu8FileData, 1, u32FileDataLen, pFile);
    if((sizeReadCount != u32FileDataLen) && (!feof(pFile)))
    {
        OM_ERROR("open read error in %s!\n", __FUNCTION__);
        fclose(pFile);
        return FAIL;
    }
    else
    {
        OM_INFO("Read Success!\n");
    }
    
    fclose(pFile);
    *pu32ReadLen = sizeReadCount;
    
    return SUCC;
}

int32 FindMatchPos(const uint8* pu8Buf, const uint32 u32BufLen, const uint8 u8MatchCode)
{
    int32 i32Index;

    for(i32Index = 0; i32Index < u32BufLen; i32Index++)
    {
        if(*(pu8Buf + i32Index) == u8MatchCode)
        {
            return i32Index;
        }
    }
    
    return -1;
}

//读取一条完整的消息
_enResult ReadWholeMsg(uint8* pu8Data, const uint32 u32DataLen)
{
    assert(pu8Data != NULL && u32DataLen > 0);
    
    _enBool bHaveHead = FALSE;
    uint32 u32OverTime;
    char* pchFileName = "../temp.dat";
    uint8 au8FileData[20] = {14, 4, 0, 6, 7, 8, 4, 5, 5, 1, 3, 7, 9, 2, 1, 0, 1, 6, 4, 14};
    uint32 u32StartPos = 0;
    uint8 au8ReadBuf[19] = {0};
    uint32 u32FileReadLen = 0;
    int32 i32HeadPos = 0;
    int32 i32TailPos = 0;
    uint8 u8MatchCode = 14;
    uint8 au8MsgBuf[100] = {0};
    uint32 u32MsgBufPos = 0;
    
    //将数据写入文件
    if(SUCC != WriteDataToFile(pchFileName, au8FileData, sizeof(au8FileData)))
    {
        return FAIL;
    }

    for(u32OverTime = 0; u32OverTime < 10; u32OverTime++)
    {
        //从文件中读取一段数据
        if(SUCC != ReadDataFromFile(pchFileName, au8ReadBuf, sizeof(au8ReadBuf), u32StartPos, &u32FileReadLen))
        {
            return FAIL;
        }

        u32StartPos += u32FileReadLen;
        //
        PRINT_BUF(au8ReadBuf, sizeof(au8ReadBuf));

        if(!bHaveHead)
        {
            //找头
            i32HeadPos = FindMatchPos(au8ReadBuf, u32FileReadLen, u8MatchCode);
            
            if(i32HeadPos == -1)
            {
                //读取的数据没有头，丢弃
                memset(au8ReadBuf, 0, sizeof(au8ReadBuf));
                printf("in head throw\n");
            }
            else
            {
                bHaveHead = TRUE;
                memcpy(au8MsgBuf, au8ReadBuf + i32HeadPos, u32FileReadLen - i32HeadPos);
                u32FileReadLen -= i32HeadPos;
                
                //找尾，尾是在au8ReadBuf中的i32HeadPos + 1处开始找的
                i32TailPos = FindMatchPos(au8MsgBuf + 1, u32FileReadLen - 1, u8MatchCode);
                if(i32TailPos == -1)
                {
                    //未找到
                    u32MsgBufPos += u32FileReadLen;
                    memset(au8ReadBuf, 0, sizeof(au8ReadBuf));

                    printf("in head no tail");
                    //PRINT_BUF(au8MsgBuf, u32MsgBufPos);
                }
                else
                {
                    printf("in head have tail ,head pos is %d, tail pos is %d", i32HeadPos, i32TailPos);
                    u32MsgBufPos = i32TailPos + 2;
                    break;
                }
            }
        }
        else
        {
            //找尾
            i32TailPos = FindMatchPos(au8ReadBuf, u32FileReadLen, u8MatchCode);
            if(i32TailPos == -1)
            {
                //本次未找到尾，先存
                memcpy(au8MsgBuf + u32MsgBufPos, au8ReadBuf, u32FileReadLen);
                u32MsgBufPos += u32FileReadLen;
                memset(au8ReadBuf, 0, sizeof(au8ReadBuf));

                PRINT_BUF(au8MsgBuf, u32MsgBufPos);
            }
            else
            {
                memcpy(au8MsgBuf + u32MsgBufPos, au8ReadBuf, i32TailPos + 1);
                u32MsgBufPos += i32TailPos + 1;

                PRINT_BUF(au8MsgBuf, u32MsgBufPos);
                break;
            }
        }

        usleep(10*1000);
    }

    if(u32OverTime >= 10)
    {
        return FAIL;
    }
    
    memcpy(pu8Data, au8MsgBuf, u32MsgBufPos);

    return SUCC;
}

const uint8 SLIP_ESC = 7;
const uint8 SLIP_END = 14;
const uint8 SLIP_ESC_ESC = 8;
const uint8 SLIP_ESC_END = 9;

uint32 SlipMsgAntiTrans(uint8* pu8Data, uint32 u32DataLen)
{
    uint32 u32SrcIndex;
    uint32 u32DesIndex;
    uint8 u8Temp;
    _enBool bNeedConv = FALSE;
    
    for(u32SrcIndex = 0, u32DesIndex = 0; u32SrcIndex < u32DataLen; u32SrcIndex++, u32DesIndex++)
    {
        u8Temp = pu8Data[u32SrcIndex];
        if(u8Temp == SLIP_ESC)
        {
            u32SrcIndex++;
            bNeedConv = TRUE;
        }

        if(bNeedConv)
        {
            
            if(pu8Data[u32SrcIndex] == SLIP_ESC_END)
            {
                pu8Data[u32DesIndex] = SLIP_END;
            }

            if(pu8Data[u32SrcIndex] == SLIP_ESC_ESC)
            {
                pu8Data[u32DesIndex] = SLIP_ESC;
            }
            
            bNeedConv = FALSE;
            continue;
        }

        pu8Data[u32DesIndex] = u8Temp;
    }

    return u32DesIndex;
}

uint32 SlipMsgTrans(uint8* pu8DesData, uint32 u32DesDataLen, uint8* pu8SrcData, uint32 u32SrcDataLen)
{
    uint32 u32SrcIndex;
    uint32 u32DesIndex;
    uint8 u8Temp;
    
    for(u32SrcIndex = 0, u32DesIndex = 0; u32SrcIndex < u32SrcDataLen; u32SrcIndex++, u32DesIndex++)
    {
        u8Temp = pu8SrcData[u32SrcIndex];

        switch(u8Temp)
        {
            case SLIP_ESC:

                pu8DesData[u32DesIndex] = SLIP_ESC;
                u32DesIndex++;
                pu8DesData[u32DesIndex] = SLIP_ESC_ESC;
                break;
                
            case SLIP_END:

                pu8DesData[u32DesIndex] = SLIP_ESC;
                u32DesIndex++;
                pu8DesData[u32DesIndex] = SLIP_ESC_END;
                break;
                
            default:

                pu8DesData[u32DesIndex] = u8Temp;
                break;
        }
      }
    
    return u32DesIndex;
}

int main()
{
    uint8 au8Msg[100] = {0};
    uint32 u32MsgLen = sizeof(au8Msg);

    ReadWholeMsg(au8Msg, u32MsgLen);
    
    OM_INFO("Recv Data: ");
    PRINT_BUF(au8Msg, 20);
    
    SlipMsgAntiTrans(au8Msg, 20);
    PRINT_BUF(au8Msg, 30);

    uint8 au8DesMsg[40] = {0};
    SlipMsgTrans(au8DesMsg, 40, au8Msg, 30);
    PRINT_BUF(au8DesMsg, 40);
    
    string strMapPosKey = "123,456";
    int32 i32X;
    int32 i32Y;
    sscanf(strMapPosKey.c_str(), "%d,%d", &i32X, &i32Y);
    cout << i32X << " " << i32Y << endl;

    return 0;
}
