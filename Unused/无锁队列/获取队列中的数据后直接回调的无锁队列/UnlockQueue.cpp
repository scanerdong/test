#include "UnlockQueue.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const unsigned int UnlockQueue::MAX_USEFUL_LEN = 0xffff; //必须大于len字段，即4个字节，不然len字段会拆分放到数组的头和尾

//存入数据类型为LV格式，即4字节len+变长value，len=value的长度+本身的长度4
UnlockQueue::UnlockQueue(int nSize)                                 
:m_pBuffer(NULL)                                                  
,m_nSize(nSize)                                                   
,m_nIn(0)                                                         
,m_nOut(0)                                                        
{                                                                 
    //round up to the next power of 2                             
    if (!is_power_of_2(nSize))                                    
    {                                                             
        m_nSize = roundup_power_of_two(nSize);                    
        m_nActualSize = m_nSize + MAX_USEFUL_LEN;
    }                                                             
}                                                                 

UnlockQueue::~UnlockQueue()                                       
{                                                                 
    if(NULL != m_pBuffer)                                         
    {                                                             
        delete[] m_pBuffer;                                       
        m_pBuffer = NULL;                                         
    }                                                             
}                                                                 
                                                                  
bool UnlockQueue::Initialize(CallBackFunc callback, void* param)                                    
{                                                                 
    m_pBuffer = new unsigned char[m_nActualSize]; //new出来的数组加上最大解析的长度，以避免数据包有用数据部分被分放到数组的末尾和开始                  
    if (!m_pBuffer)                                               
    {                                                             
        return false;                                             
    }                                                             
                                                                  
    m_nIn = m_nOut = 0;     

    if(callback == NULL) return false;
    
    m_pfCallBack = callback;
    m_pParam = param;

    return true;                                                  
}                                                                 
                                                                  
unsigned long UnlockQueue::roundup_power_of_two(unsigned long val)
{                                                                 
    if((val & (val-1)) == 0)                                      
        return val;                                               
                                                                  
    unsigned long maxulong = (unsigned long)((unsigned long)~0);  
    unsigned long andv = ~(maxulong&(maxulong>>1));               
    while((andv & val) == 0)                                      
        andv = andv>>1;                                           
                                                                  
    return andv<<1;                                               
}                                                                 

  #include <assert.h>
//确保存入的是一整包数据
unsigned int UnlockQueue::Put(const unsigned char *buffer, unsigned int len)
{                                                                        
    unsigned int l;
    
    //如果放不下这包数据，直接返回错误
    if(len > m_nSize - m_nIn + m_nOut)
    {
        printf("the ring is full\n");
        return -1;
    }

    //len = std::min(len, m_nSize - m_nIn + m_nOut);                       
                                                                         
    /*                                                                   
     * Ensure that we sample the m_nOut index -before- we                
     * start putting bytes into the UnlockQueue.                         
     */                                                                  
    __sync_synchronize();                                                
                                                                         
    /* first put the data starting from fifo->in to buffer end */        
    l = std::min(len, m_nActualSize - (m_nIn  & (m_nSize - 1)));               
    memcpy(m_pBuffer + (m_nIn & (m_nSize - 1)), buffer, l);              
                                                                         
    /* then put the rest (if any) at the beginning of the buffer */      
    memcpy(m_pBuffer, buffer + l, len - l);                              
                                                                         
    /*                                                                   
     * Ensure that we add the bytes to the kfifo -before-                
     * we update the fifo->in index.                                     
     */                                                                  
    __sync_synchronize();                                                
                                                                         
    m_nIn += len;                                                        
                                                                         
    return len;                                                          
}                                                                        

//必须确保参数len大于一包数据的长度
int UnlockQueue::HandleOnePacket()   
{                                                                        
    unsigned int l;
    unsigned int u32PkgLen;    
    unsigned char au8PkgLen[4];
    int ret;
        
    if(IsEmpty())
    {
        return 0;
    }

    //取出一整包数据的前4个字节(长度)
    l = std::min(4U, m_nActualSize - (m_nOut & (m_nSize - 1)));
    memcpy(au8PkgLen, m_pBuffer + (m_nOut & (m_nSize - 1)), l);
    memcpy(au8PkgLen + l, m_pBuffer, 4 - l);

    u32PkgLen = *(unsigned int*)au8PkgLen;                                                                             
    /*                                                                   
     * Ensure that we sample the fifo->in index -before- we              
     * start removing bytes from the kfifo.                              
     */                                                                  
    __sync_synchronize();                                                

    //回调
    /* first get the data from fifo->out until the end of the buffer */  
    l = std::min(u32PkgLen, m_nActualSize - (m_nOut & (m_nSize - 1)));

    //当一个数据包大于MAX_USEFUL_LEN时，传入的大小将被截断成MAX_USEFUL_LEN
    ret =  (*m_pfCallBack)(m_pBuffer + (m_nOut & (m_nSize - 1)), l, m_pParam);
    if(ret != 0)
    {
        printf("handle data error\n");
    }
    
    /*                                                                   
     * Ensure that we remove the bytes from the kfifo -before-           
     * we update the fifo->out index.                                    
     */                                                                  
    __sync_synchronize();                                                

    m_nOut += u32PkgLen;                                                       

    return ret;
}                                                                        

