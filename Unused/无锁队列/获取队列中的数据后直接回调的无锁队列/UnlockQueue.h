#ifndef _UNLOCK_QUEUE_H
#define _UNLOCK_QUEUE_H

typedef int (*CallBackFunc)(unsigned char* pkg, unsigned int len, void* param);

class UnlockQueue
{
public:
    UnlockQueue(int nSize);
    virtual ~UnlockQueue();
 
    bool Initialize(CallBackFunc callback, void* param); //回调函数，回调函数的参数
 
    unsigned int Put(const unsigned char *pBuffer, unsigned int nLen);
    int HandleOnePacket();
 
    inline void Clean() { m_nIn = m_nOut = 0; }
    inline unsigned int GetDataLen() const { return  m_nIn - m_nOut; }
	inline bool IsEmpty() { return m_nIn == m_nOut; }
	inline bool IsFull() { return  (m_nSize - m_nIn + m_nOut == 0); }
 
private:
    inline bool is_power_of_2(unsigned long n) { return (n != 0 && ((n & (n - 1)) == 0)); };
    inline unsigned long roundup_power_of_two(unsigned long val);
 
private:
    unsigned char *m_pBuffer;    /* the buffer holding the data */
    unsigned int   m_nSize;        /* the size of the allocated buffer */
	unsigned int   m_nActualSize; //实际申请的长度,计算in/out的位置时用size，拷贝数据时用actual_size
    unsigned int   m_nIn;        /* data is added at offset (in % size) */
    unsigned int   m_nOut;        /* data is extracted from off. (out % size) */

	CallBackFunc m_pfCallBack; //在获取到消息后进行回调
	void* m_pParam; //回调函数的参数

	static const unsigned int MAX_USEFUL_LEN; //一包数据能用上的最大长度
};
 
#endif

