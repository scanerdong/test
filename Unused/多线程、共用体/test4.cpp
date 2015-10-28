#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"
using namespace std;

uint8 g_u8PrintCtrl = 0xff;

#define _MULTI_THREADED  
  
void cleanupHandler(void *parm) {  
  printf("Inside cancellation cleanup handler\n");  
}  
  
void *threadfunc(void *parm)  
{  
  unsigned int  i=0;  
  int           rc=0, oldState=0;  
  printf("Entered secondary thread\n");  

  //pthread_detach(pthread_self());
  
  sleep(2);  
  printf("Entered secondary thread\n");  
  while (1) {  
    printf("Secondary thread is now looping\n");  
	
    sleep(1);  
	
   // pthread_testcancel();  
	
	
  } 
  return NULL;  
}  
  
int main(int argc, char **argv)  
{  
  pthread_t             thread;  
  int                   rc=0;  
  void                 *status=NULL;  
  
  printf("Create thread using the NULL attributes\n");  
  rc = pthread_create(&thread, NULL, threadfunc, NULL);  
  
  sleep(5);  
  
  printf("Cancel the thread\n");  
  
  rc = pthread_cancel(thread);  


  sleep(1);
  
  rc = pthread_join(thread, &status);  
  if(rc == 0)
  	{
     cout<< "pthread_join success\n"<<endl;
  }
  
  if (status != PTHREAD_CANCELED) {  
    printf("Thread returned unexpected result!\n");  
    exit(1);  
  }  
  printf("Main completed\n");  
  return 0;  
}  

