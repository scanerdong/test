#include <zmq.h>
#include <string.h> 
#include <stdio.h> 
#include <unistd.h>  
#include <stdlib.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

/*
int main () 
{     
 void *context = zmq_init (1); //  Socket to talk to server     
 printf ("Connecting to hello world server...\n");     
 void *requester = zmq_socket (context, ZMQ_REQ);     
 zmq_connect (requester, "tcp://127.0.0.1:5555");       
 int request_nbr;     
 char ach[10] = "hello";
 for (request_nbr = 0; request_nbr != 10; request_nbr++) 
 {         
  zmq_msg_t request;         
  zmq_msg_init_data (&request, ach, 6, NULL, NULL);         
  printf ("Sending request %d...\n", request_nbr);         
  //zmq_send (requester, &request, 0,0); 
  zmq_msg_send(&request, requester, 0);
  printf("send over")      ; 
  zmq_msg_close (&request);           
  
  zmq_msg_t reply;         
  zmq_msg_init (&reply);         
  //zmq_recv (requester, &reply, 0,0);    
  zmq_msg_recv(&reply, requester, 0);
  printf ("Received reply %d: [%s]\n", request_nbr, (char *) zmq_msg_data (&reply));         
  zmq_msg_close (&reply);     
 }     
 zmq_close (requester);     
 zmq_term (context);     
 return 0; 
}
*/

#define PACKET_DATA_SIZE (300)
void freeMsg(void* data, void* hint)
{
	//printf("call user free\n");
	//free(data);
}

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
	void* context = zmq_init(1); //  Socket to talk to server	  
	void* context2 = zmq_init(2); //  Socket to talk to server	  
	printf("prepare push messages\n");
	
	void* pusher = zmq_socket(context, ZMQ_PUSH);
	void* pusher2 = zmq_socket(context2, ZMQ_PUSH);
	void* pusher22 = zmq_socket(context2, ZMQ_PUSH);

	//set socket option
	int optValue = 10;
	zmq_setsockopt(pusher, ZMQ_SNDHWM, &optValue, sizeof(int));

	zmq_connect(pusher, url.c_str());//"tcp://172.16.36.31:7000"
	
	zmq_connect(pusher2, "ipc:///tmp/hahahaha");//这里创建一个叫/tmp/hahahaha的命名管道来传递信息
	zmq_connect(pusher22, "ipc:///tmp/hahahaha");//这里创建一个叫/tmp/hahahaha的命名管道来传递信息

	unsigned long long i=1;
	char pchMsg[PACKET_DATA_SIZE];
	
	while(1)
	{
		//char* pchMsg = (char*)malloc(sizeof(char)*PACKET_DATA_SIZE);
		//memset(pchMsg, i, PACKET_DATA_SIZE);
		//snprintf(pchMsg, PACKET_DATA_SIZE, "send %lld packets!", i);
		zmq_msg_t msg;
		
		//zmq_msg_init_data(&msg, pchMsg, PACKET_DATA_SIZE, freeMsg, NULL);
		zmq_msg_init_data(&msg, pchMsg, PACKET_DATA_SIZE, freeMsg, NULL);
		zmq_msg_send(&msg,pusher,ZMQ_DONTWAIT);//ZMQ_DONTWAIT
		zmq_msg_close(&msg);

		int ret = zmq_send(pusher, pchMsg, PACKET_DATA_SIZE, ZMQ_DONTWAIT); 
                printf("send ret %d\n",ret);
		zmq_send(pusher2, pchMsg, PACKET_DATA_SIZE, ZMQ_DONTWAIT);
		zmq_send(pusher22, pchMsg, PACKET_DATA_SIZE, ZMQ_DONTWAIT);
		
		gettimeofday(&stLogTime,NULL);
		u64CurrLogUsec = stLogTime.tv_sec*1000*1000 + stLogTime.tv_usec;
		
		//超过了记录总包数的时间
		if(u64CurrLogUsec - u64LastLogUsec >= 10*1000*1000)
		{
			u64LastLogUsec = u64CurrLogUsec;
			printf("time[%s] send %lld packets\n", getTimeStr(achTime,time(NULL)), i);
		}
		i++;
		sleep(1);
	}

	zmq_close(pusher);
	zmq_close(pusher2);
	zmq_term(context);
}
