#include <malloc.h>
#include <stdio.h>  
#include <memory.h>
#include <zmq.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <map>
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

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

	//set recv opt
	int optValue = 1000000;
	zmq_setsockopt (puller, ZMQ_RCVHWM, &optValue, sizeof(int));

	std::string ip = argv[1];
	std::string port = argv[2];
	std::string url = "tcp://"+ip+":"+port;
	printf("%s\n", url.c_str());
	zmq_bind(puller, url.c_str());

	unsigned long long ullRecvPackets=0, ullRecvBytes=0;
	while(1)
    {
        zmq_pollitem_t items[1];
        items[0].socket = puller;
        items[0].fd = 0;
        items[0].events = ZMQ_POLLIN;

		// Poll for events indefinitely
        int nRet = zmq_poll(items,1,-1);
        if(0 == nRet)
            continue;

        //zmq_msg_t msg;
        char re_string[PACKET_MSG_SIZE] = {0};
		int iRecvSize;
		
        if(items[0].revents > 0)
        {
            //zmq_msg_init(&msg);
            //zmq_msg_recv(&msg,puller,0);
			iRecvSize = zmq_recv(puller, re_string, PACKET_MSG_SIZE, 0);//ZMQ_DONTWAIT
			if(iRecvSize<=0) continue;
			ullRecvPackets++;
			ullRecvBytes += iRecvSize;
            //memcpy(re_string,(char*)zmq_msg_data(&msg),zmq_msg_size(&msg));
            //zmq_msg_close(&msg);
			//sleep(1);
        }


		gettimeofday(&stLogTime,NULL);
		u64CurrLogUsec = stLogTime.tv_sec*1000*1000 + stLogTime.tv_usec;
		
		//超过了记录总包数的时间
		if(u64CurrLogUsec - u64LastLogUsec >= 10*1000*1000)
		{
			u64LastLogUsec = u64CurrLogUsec;
            printf("time[%s], puller ullRecvPackets: %llu, ullRecvBytes: %llu\n",getTimeStr(achTime,time(NULL)), ullRecvPackets, ullRecvBytes);
		}
    }
   
    zmq_close(puller);
    zmq_term(context);
}

