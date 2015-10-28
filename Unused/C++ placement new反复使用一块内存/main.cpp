#include <malloc.h>
#include <stdio.h>  
#include <memory.h>
#include <zmq.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <assert.h>
#include <stdint.h>

typedef unsigned int uint32;

void trim(std::string& str, const std::string trim_chars)
{
	// trim left
	size_t pos = str.find_first_not_of(trim_chars);
	str.erase(0, pos);

	// trim right
	pos = str.find_last_not_of(trim_chars);
	if ( pos != std::string::npos ) {
		str.erase(pos+1);
	}
	return;
}

std::string token(std::string& str, const std::string seperators)
{
	size_t pos = str.find_first_of(seperators);
	std::string word;

	if (pos != std::string::npos) {
		word = str.substr(0, pos);
		str.erase(0, pos+1);
	} else {
		word = str;
		str.clear();
	}

	trim(word, seperators + " \t\n\r");
	trim(str, seperators + " \t\n\r");
	return word;
}

uint32 ip_from_ascii(const std::string& ip_str)
{
	uint32 ip = 0;
	std::string tmp_ip = ip_str;
	for(size_t n=0; n<4; n++) {
		std::string ip_part = token(tmp_ip, ".");
		ip = (ip<<8) + atoi(ip_part.c_str());
	}

	return ip;
}

#include <arpa/inet.h>

/*
 * struct netmap_ring
 *
 * Netmap representation of a TX or RX ring (also known as "queue").
 * This is a queue implemented as a fixed-size circular array.
 * At the software level the important fields are: head, cur, tail.
 *
 * In TX rings:
 *
 *	head	first slot available for transmission.
 *	cur	wakeup point. select() and poll() will unblock
 *		when 'tail' moves past 'cur'
 *	tail	(readonly) first slot reserved to the kernel
 *
 *	[head .. tail-1] can be used for new packets to send;
 *	'head' and 'cur' must be incremented as slots are filled
 *	    with new packets to be sent;
 *	'cur' can be moved further ahead if we need more space
 *	for new transmissions. XXX todo (2014-03-12)
 *
 * In RX rings:
 *
 *	head	first valid received packet
 *	cur	wakeup point. select() and poll() will unblock
 *		when 'tail' moves past 'cur'
 *	tail	(readonly) first slot reserved to the kernel
 *
 *	[head .. tail-1] contain received packets;
 *	'head' and 'cur' must be incremented as slots are consumed
 *		and can be returned to the kernel;
 *	'cur' can be moved further ahead if we want to wait for
 *		new packets without returning the previous ones.
 *
 * DATA OWNERSHIP/LOCKING:
 *	The netmap_ring, and all slots and buffers in the range
 *	[head .. tail-1] are owned by the user program;
 *	the kernel only accesses them during a netmap system call
 *	and in the user thread context.
 *
 *	Other slots and buffers are reserved for use by the kernel
 */
 #pragma pack(1)
class netmap_ring {

    public:

	//struct timeval	ts;		/* (k) time of last *sync() */
    uint32 len;
	/* the slots follow. This struct has variable size */
	char slot[0];	/* array of slots. */
    netmap_ring(){printf("call instractor\n");strcpy(slot, "hijklmnopqrstuvwxyz");}
    
    ~netmap_ring(){printf("call destractor\n");}
};
#pragma pack()

/*

1. ��������ǰ����

����ʹ�öѵĿռ䣬Ҳ����ʹ��ջ�Ŀռ䣬���Է��䷽ʽ���������֣�

class MyClass {��}; 
char *buf=new char[N*sizeof(MyClass)+ sizeof(int) ] ; ����char buf[N*sizeof(MyClass)+ sizeof(int) ];

2. ����Ĺ���

MyClass * pClass=new(buf) MyClass;

3. ���������

һ���������ʹ����ϣ��������ʽ�ĵ���������������������ٶ��󡣵���ʱ�ڴ�ռ䲻�ᱻ�ͷţ��Ա������Ķ���Ĺ��졣

pClass->~MyClass();

4. �ڴ���ͷ�

����������ڶ��У���ô����delete[] buf;�����ڴ���ͷţ������ջ�У���ô��������������Ч�������������ڴ��Զ��ͷš�

*/

/*
ע�⣺

1)        ��C++��׼�У�����placement operator new []�����µ�˵���� placement operator new[] needs implementation-defined amount of additional storage to save a size of array. �������Ǳ��������ԭʼ�����С���sizeof(int)���ֽ�����Ŷ���ĸ���������˵����Ĵ�С��

2)        ʹ�÷����ڶ����е�new����placement new����ʵ��û�������ڴ�ģ�ֻ�ǵ����˹��캯��������һ��ָ���Ѿ�����õ��ڴ��һ��ָ�룬���Զ������ٵ�ʱ����Ҫ����delete�ͷſռ䣬��������������������ٶ���


new��ʱ����ʵ���������£�һ�ǣ�����malloc���������ڴ棬���ǣ����ù��캯������replacement newֻ���ڶ�����

delete��ʱ��Ҳ�����������£�һ�ǣ��������캯�������ǣ�����free�ͷ��ڴ档

*/
int main(int argc, char *argv[])
{
    char* buf = new char[sizeof(netmap_ring) + 100];

    //pring->netmap_ring::netmap_ring();
    netmap_ring* pring = new (buf) netmap_ring();
    
    
    //���Կ���buf��pring��һ����ַ��Ҳ����˵newû�����µ��ڴ棬ֻ���൱�ڵ����˹��캯�����������쵽buf��
    printf("buf addr:%p, ring addr:%p\n", buf, pring); 
    
    printf("%c\n", pring->slot[3]); //�����ӡ��4���ַ�K

    //���������ֶ��������������ֶ������ö��󣬹رոö���ʹ�õ�һЩ��Դ��
    pring->~netmap_ring();

    delete [] buf; //���buf����ɾ��
    
    return 0;

}
