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


#define PACKET_MSG_SIZE (65535)
#define PACKET_DATA_SIZE (65535)


    char *
getTimeStr (char *buf, time_t t)
{
    time_t the_time = t;
    struct tm *tm_ptr = localtime (&the_time);
    sprintf (buf, "%02d%02d%02d%02d%02d%02d", tm_ptr->tm_year,
            tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour,
            tm_ptr->tm_min, tm_ptr->tm_sec);
    return buf;
}

#include<map>
#include<string>
#include<iostream>
using namespace std;

#if 1
int main(int argc, char *argv[])
{
	static struct timeval stLogTime;
	static unsigned long u64LastLogUsec;
	unsigned long long u64CurrLogUsec;
	char achTime[100];

	if(argc < 3)
	{
			printf("usage: om_11 ip port");
			return 0;
	}

	
	gettimeofday(&stLogTime,NULL);
	u64LastLogUsec = stLogTime.tv_sec*1000*1000 + stLogTime.tv_usec;

	void* context = zmq_init(2); //  Socket to talk to server	  
	printf("prepare pull messages\n");

	void* puller = zmq_socket(context, ZMQ_PULL);
	//void* puller = zmq_socket(context, ZMQ_SUB);

	//set recv opt
	int optValue = 1000000;
	zmq_setsockopt (puller, ZMQ_RCVHWM, &optValue, sizeof(int));

	//zmq_setsockopt (puller, ZMQ_SUBSCRIBE, NULL, 0);

	std::string ip = argv[1];
	std::string port = argv[2];
	std::string url = "tcp://"+ip+":"+port;

    //url = "ipc:///tmp/iups_u1";
    
	printf("%s\n", url.c_str());
    //zmq_connect(puller,url.c_str());
	zmq_bind(puller, url.c_str());
	//zmq_bind(puller, "ipc:///tmp/hahahaha"); //这里创建一个叫/tmp/hahahaha的命名管道来传递信息
	unsigned long long ullRecvPackets=0, ullRecvBytes=0;
    char re_string[PACKET_MSG_SIZE] = {0};
	while(1)
    {
		int iRecvSize;
        iRecvSize = zmq_recv(puller, re_string, PACKET_MSG_SIZE, 0);//ZMQ_DONTWAIT
        ullRecvPackets++;
        ullRecvBytes += iRecvSize;

		gettimeofday(&stLogTime,NULL);
		u64CurrLogUsec = stLogTime.tv_sec*1000*1000 + stLogTime.tv_usec;

        //sleep(1);
        
		//超过了记录总包数的时间,10秒钟打印一次
		if(u64CurrLogUsec - u64LastLogUsec >= 10*1000*1000)
		{
			u64LastLogUsec = u64CurrLogUsec;
            printf("time[%s], puller ullRecvPackets: %llu, ullRecvBytes: %llu\n",getTimeStr(achTime,time(NULL)), ullRecvPackets, ullRecvBytes);
		}
    }
   
    zmq_close(puller);
    zmq_term(context);
}
#endif
#if 0
//push
int main(int argc, char *argv[])
{
	static struct timeval stLogTime;
	static unsigned long u64LastLogUsec;
	unsigned long long u64CurrLogUsec;
	char achTime[100];

	if(argc < 3)
	{
		printf("usage om_11 ip port");
		return 0;
	}

	std::string ip = argv[1];
	std::string port = argv[2];

	std::string url = "tcp://"+ip+":"+port;
	std::cout << url << std::endl;
	void* context = zmq_init(2); //  Socket to talk to server	  
	printf("prepare push messages\n");
	
	void* pusher = zmq_socket(context, ZMQ_PUSH);

	//set socket option
	int optValue = 10000;
	zmq_setsockopt(pusher, ZMQ_SNDHWM, &optValue, sizeof(int));

	zmq_connect(pusher, url.c_str());//"tcp://172.16.36.31:7000"
	
	//zmq_connect(pusher, "ipc:///tmp/hahahaha");//这里创建一个叫/tmp/hahahaha的命名管道来传递信息

	unsigned long long i=1, err = 0;
	char pchMsg[PACKET_DATA_SIZE];
	
	while(1)
	{
		//char* pchMsg = (char*)malloc(sizeof(char)*PACKET_DATA_SIZE);
		//memset(pchMsg, i, PACKET_DATA_SIZE);
		//snprintf(pchMsg, PACKET_DATA_SIZE, "send %lld packets!", i);
		//zmq_msg_t msg;
		
		//zmq_msg_init_data(&msg, pchMsg, PACKET_DATA_SIZE, freeMsg, NULL);
		/*zmq_msg_init_data(&msg, pchMsg, PACKET_DATA_SIZE, freeMsg, NULL);
		zmq_msg_send(&msg,pusher,0);//ZMQ_DONTWAIT
		zmq_msg_close(&msg);*/

		if(zmq_send(pusher, pchMsg, PACKET_DATA_SIZE, ZMQ_DONTWAIT) < 0)
		{
            err++;
        }
		
		gettimeofday(&stLogTime,NULL);
		u64CurrLogUsec = stLogTime.tv_sec*1000*1000 + stLogTime.tv_usec;
		
		//超过了记录总包数的时间
		if(u64CurrLogUsec - u64LastLogUsec >= 10*1000*1000)
		{
			u64LastLogUsec = u64CurrLogUsec;
			printf("time[%s] send %lld packets, err %lld packets\n", getTimeStr(achTime,time(NULL)), i, err);
		}
		i++;
		//sleep(1);
	}

	zmq_close(pusher);
	zmq_term(context);
}
#endif
