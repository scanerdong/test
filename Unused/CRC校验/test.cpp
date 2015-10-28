//测试C++中虚函数在内存中的分布
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include "Singleton.h"
#include "msgproc.h"
#include "Test.h"

uint16 fnGetCrcCode(const uint8 *pu8Buf, uint32 u32Len)
{
    uint32 u32Index1, u32Index2;
    uint16 u16CrcCode = 0;
    uint16 u16Current;

    for (u32Index1 = 0; u32Index1 < u32Len; u32Index1++)
    {
        u16Current = pu8Buf[u32Index1] << 8;
        for (u32Index2 = 0; u32Index2 < 8; u32Index2++)
        {
            if ((int16)(u16CrcCode ^ u16Current) < 0)
            {
                u16CrcCode = (u16CrcCode << 1) ^ 0x1021;
            }
            else
            {
                u16CrcCode <<= 1;
            }

            u16Current <<= 1;
        }
    }

    return u16CrcCode;
}

uint16 calcCRC(const uint8* pchData, uint32 u32Len)
{ 
    uint16 u16Crc = 0;
    uint32 u32Index1, u32Index2;
    uint16 u16Current;
    for(u32Index1 = 0; u32Index1 < u32Len; u32Index1++)
    {
        u16Current = pchData[u32Index1];
        for(u32Index2 = 0; u32Index2 < 8; u32Index2++ )
        {
            if ((pchData[u32Index1] ^ u16Crc) & 0x0001)
            {
                u16Crc = ( u16Crc >> 1 ) ^ 12358;
            }
            else
            {
                u16Crc >>= 1;
            }
            
            u16Current >>= 1;
        }
    }

    return u16Crc;
}

unsigned int cal_crc(unsigned char *ptr, unsigned char len) {
unsigned char i;
unsigned int crc=0;
while(len--!=0) {
for(i=0x80; i!=0; i/=2) {
if((crc&0x8000)!=0) {crc*=2; crc^=0x1021;} /* 余式CRC 乘以2 再求CRC */
else crc*=2;
if((*ptr&i)!=0) crc^=0x1021; /* 再加上本位的CRC */
}
ptr++;
}
return(crc);
}

#define POLY 0x8408
/*
//                                      16   12   5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.
*/

unsigned short crc16(char *data_p, unsigned short length)
{
      unsigned char i;
      unsigned int data;
      unsigned int crc = 0xffff;

      if (length == 0)
            return (~crc);

      do
      {
            for (i=0, data=(unsigned int)0xff & *data_p++;
                 i < 8; 
                 i++, data >>= 1)
            {
                  if ((crc & 0x0001) ^ (data & 0x0001))
                        crc = (crc >> 1) ^ POLY;
                  else  crc >>= 1;
            }
      } while (--length);

      crc = ~crc;
      data = crc;
      crc = (crc << 8) | (data >> 8 & 0xff);

      return (crc);
}

//这也是CRC的另一种版本
uint16 GetCrcCode(const uint8 *pu8Buf, uint32 u32Len)
{
    uint8 u8I;
    uint16 u16Crc = 0;
    
    while(u32Len--)
    {
        for(u8I = 0x80; u8I != 0; u8I >>= 1)//每次计算数据流中的8位数据
        {
            if((u16Crc & 0x8000) != 0)//取CRC第16位，就是上一次的CRC15位
            {
                u16Crc <<= 1;
                u16Crc ^= 0x1021;
            }
            else
            {
                u16Crc <<=1;
            }

            if((*pu8Buf & u8I) != 0)
            {
                u16Crc ^= 0x1021;
            }
        }
        
        pu8Buf++;
    }

    return u16Crc;
    
}

int32 main()
{
    //uint32 i32I = (0xc1 << 24) | (0x43 << 16) | (0xbc << 8) | 0xff;
    //uint32 i32I = 0xc143bcff;
    //float32 f32Power;

    //memcpy(&f32Power, &i32I, 4);

    //cout << f32Power << endl << hex << *(uint32*)&f32Power << endl << hex <<htonl(i32I) << endl;

    char achSrc[] = "abcdefghijklmnopqrstuvwxyz";
    cout << hex << GetCrcCode((uint8*)achSrc, sizeof(achSrc)) << endl;
    cout << hex << fnGetCrcCode((uint8*)achSrc, sizeof(achSrc)) << endl;

    cout <<dec<< sizeof(achSrc) << endl << strlen(achSrc) << endl;
    return 0;
}

