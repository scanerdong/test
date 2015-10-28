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

/*
int main(int argc, char* argv[])
{   
	void* zmqContext = NULL;
	int iRet;
	
	zmqContext = zmq_ctx_new();
	if(zmqContext == NULL)
	{
		printf("new zmq context failed\n");
		return -1;
	}
	
	iRet = zmq_ctx_set(zmqContext, ZMQ_IO_THREADS, 1);
	if(iRet != 0)
	{
		zmq_ctx_destroy(zmqContext);
		return -2;
	}

	//初始化zmq消息
	zmq_msg_t msg;
	iRet = zmq_msg_init (&msg);

	void *responder = zmq_socket(zmqContext, ZMQ_REP);
	zmq_bind (responder, "tcp://127.0.0.1:5555");
	printf("binding on port 5555.\nwaiting client send message...\n");

	while (1) {
        // Wait for next request from client
        zmq_msg_t request;
        zmq_msg_init (&request);
        char buf[32];
        //zmq_recv(responder,buf, sizeof(buf), 0);
        iRet = zmq_msg_recv(&request, responder, 0);
		printf("recv len,%d\n", iRet);
        int size = zmq_msg_size(&request);
        char *string = (char*)malloc(size + 1);
        memset(string,0,size+1);
        memcpy (string, zmq_msg_data(&request), size);
        printf ("Received Hello string=[%s]\n",string);
        free(string);
        zmq_msg_close(&request);
        // Do some 'work'
        sleep(1);
        // Send reply back to client
        zmq_msg_t reply;
        char res[128]={0};
        snprintf(res,sizeof(res),"reply:%ld",random());
        zmq_msg_init_size (&reply, strlen(res));
        memcpy(zmq_msg_data(&reply), res, strlen(res));
        char buf2[32];
        zmq_send(responder, buf2, sizeof(buf2), 0);
        zmq_msg_close(&reply);
    }

	zmq_close (responder);
	zmq_ctx_destroy (zmqContext);
    return 0;
}
*/

#define PACKET_MSG_SIZE (300)


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

/*注意:运行此程序需要有libpgm才能支持pgm包具体方法如下

ZeroMQ epgm error on Linux: Protocol not supported

I found the solution to this problem.

Apparently when you intend on using zmq for multicasts (pgm or epgm) you have to run the ./configure with pgm enabled then do a make after.

For this libpgm is required

In looking through my extracted tar zmq tar file under the folder /foreign/ there is a libpgm tar file.

So i just ran:

./configure --with-pgm=libpgm-5.2.122

make

sudo make install
This solved the problem and i am able to bind using epgm now.

(obviously the version of libpgm will be the version in your zmq folder)
*/

#if 0
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

	//void* puller = zmq_socket(context, ZMQ_PULL);
	void* puller = zmq_socket(context, ZMQ_SUB);

	//set recv opt
	int optValue = 1000000;
	zmq_setsockopt (puller, ZMQ_RCVHWM, &optValue, sizeof(int));

	zmq_setsockopt (puller, ZMQ_SUBSCRIBE, NULL, 0);

	std::string ip = argv[1];
	std::string port = argv[2];
	std::string url = "tcp://"+ip+":"+port;

    url = "epgm://eth0;192.168.1.107:5555";
    
	printf("%s\n", url.c_str());
    zmq_connect(puller,url.c_str());
	//zmq_bind(puller, url.c_str());
	//zmq_bind(puller, "ipc:///tmp/hahahaha"); //这里创建一个叫/tmp/hahahaha的命名管道来传递信息
	unsigned long long ullRecvPackets=0, ullRecvBytes=0;
    printf("haha\n");
	while(1)
    {
        #if 0
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
        #endif

        char re_string[PACKET_MSG_SIZE] = {0};
		int iRecvSize;
        iRecvSize = zmq_recv(puller, re_string, PACKET_MSG_SIZE, 0);//ZMQ_DONTWAIT
        ullRecvPackets++;
        ullRecvBytes += iRecvSize;

        sleep(1);

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
#endif

int main()
{
    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "epgm://eth0;192.168.1.107:5555");
    if(rc != 0)
    {
        perror("zmq_connect:");
    }
    
    rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE,
                         "", 0);
    assert (rc == 0);
    
    int rcvval = 20000000;
    int m = zmq_setsockopt(subscriber, ZMQ_RCVBUF, &rcvval, sizeof rcvval);
    
    int optval = 65000000;
    m =  zmq_setsockopt(subscriber, ZMQ_RCVHWM, &optval, sizeof(optval));
    
    optval = 1;
    m =  zmq_setsockopt(subscriber, ZMQ_RECOVERY_IVL, &optval, sizeof(optval));
    
    optval = 100;
    m =  zmq_setsockopt(subscriber, ZMQ_RATE, &optval, sizeof(optval));

    int iRecvSize;
    char re_string[PACKET_MSG_SIZE] = {0};
    while(1)
    {
        iRecvSize = zmq_recv(subscriber, re_string, PACKET_MSG_SIZE, 0);//ZMQ_DONTWAIT
        if(iRecvSize<=0) continue;
        
        printf("%s\n", re_string);

        sleep(1);
    }
    
    zmq_close (subscriber);
    zmq_ctx_destroy (context);

}
