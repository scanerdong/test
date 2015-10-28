#ifndef _UNLOCK_QUEUE_H
#define _UNLOCK_QUEUE_H

typedef int (*CallBackFunc)(unsigned char* pkg, unsigned int len, void* param);

class UnlockQueue
{
public:
    UnlockQueue(int nSize);
    virtual ~UnlockQueue();
 
    bool Initialize(CallBackFunc callback, void* param); //�ص��������ص������Ĳ���
 
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
	unsigned int   m_nActualSize; //ʵ������ĳ���,����in/out��λ��ʱ��size����������ʱ��actual_size
    unsigned int   m_nIn;        /* data is added at offset (in % size) */
    unsigned int   m_nOut;        /* data is extracted from off. (out % size) */

	CallBackFunc m_pfCallBack; //�ڻ�ȡ����Ϣ����лص�
	void* m_pParam; //�ص������Ĳ���

	static const unsigned int MAX_USEFUL_LEN; //һ�����������ϵ���󳤶�
};
 
#endif

