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

1. 缓冲区提前分配

可以使用堆的空间，也可以使用栈的空间，所以分配方式有如下两种：

class MyClass {…}; 
char *buf=new char[N*sizeof(MyClass)+ sizeof(int) ] ; 或者char buf[N*sizeof(MyClass)+ sizeof(int) ];

2. 对象的构造

MyClass * pClass=new(buf) MyClass;

3. 对象的销毁

一旦这个对象使用完毕，你必须显式的调用类的析构函数进行销毁对象。但此时内存空间不会被释放，以便其他的对象的构造。

pClass->~MyClass();

4. 内存的释放

如果缓冲区在堆中，那么调用delete[] buf;进行内存的释放；如果在栈中，那么在其作用域内有效，跳出作用域，内存自动释放。

*/

/*
注意：

1)        在C++标准中，对于placement operator new []有如下的说明： placement operator new[] needs implementation-defined amount of additional storage to save a size of array. 所以我们必须申请比原始对象大小多出sizeof(int)个字节来存放对象的个数，或者说数组的大小。

2)        使用方法第二步中的new才是placement new，其实是没有申请内存的，只是调用了构造函数，返回一个指向已经分配好的内存的一个指针，所以对象销毁的时候不需要调用delete释放空间，但必须调用析构函数销毁对象。


new的时候，其实做了两件事，一是：调用malloc分配所需内存，二是：调用构造函数。而replacement new只做第二件事

delete的时候，也是做了两件事，一是：调用析造函数，二是：调用free释放内存。

*/
int main(int argc, char *argv[])
{
    char* buf = new char[sizeof(netmap_ring) + 100];

    //pring->netmap_ring::netmap_ring();
    netmap_ring* pring = new (buf) netmap_ring();
    
    
    //可以看到buf和pring是一个地址，也就是说new没分配新的内存，只是相当于调用了构造函数，将对象构造到buf中
    printf("buf addr:%p, ring addr:%p\n", buf, pring); 
    
    printf("%c\n", pring->slot[3]); //这里打印第4个字符K

    //用完后必须手动调用析构函数手动析构该对象，关闭该对象使用的一些资源等
    pring->~netmap_ring();

    delete [] buf; //最后buf必须删除
    
    return 0;

}
