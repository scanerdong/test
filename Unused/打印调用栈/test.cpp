using namespace std;


#include <stdlib.h>
#include <signal.h>
#include <execinfo.h> //此文件中包含了打印调用栈的函数backtrace
#include <stdio.h>

/* Obtain a backtrace and print it to stdout. */
void
print_trace (void)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;
  char achCmd[50][20] = {0};
  char achAddr[20] = {0};
  unsigned int u32Addr;
  
  size = backtrace (array, 10);//这里获取每一层调用栈的指针（即入栈的函数地址）
  strings = backtrace_symbols (array, size);//这里根据函数地址翻译成相应的字串语句，组成数组返回

  printf("\nthe calling stack is:\n-----------------%zd stack frames.--------------------\n",size);

  for (i = 0; i < size; i++)
  {
     printf("%p\n", array[i]);
     printf ("%s\n", strings[i]);
     sscanf(strings[i], "[%x]", &u32Addr);
     printf ("%#x\n", u32Addr);
     sprintf(achCmd[i], "addr2line -e %s %#x", "test", u32Addr);
     
     system(achCmd[i]);
  }
  printf("------------------end of stack----------------------\n");

  free (strings);
}

/* A dummy function to make the backtrace more interesting. */
void dummy_function (int sig)
{
  if(sig==SIGSEGV)
          printf("\n------------segmentation fault\n");
  else if(sig==SIGBUS)
          printf("\n------------bus error\n");
  else if(sig==SIGTRAP)
          printf("\n------------process is trapped\n");
  print_trace ();
  exit(1);
}

void fault()
{
        int *p=0;
        *p=1;
}

void  func1()
{
        fault();
}
void  func2()
{
        func1();
}
void  func3()
{
        func2();
}


void install_sigaction()
{
  struct sigaction sact;

  sigemptyset(&sact.sa_mask);
  sact.sa_flags = 0;
  sact.sa_handler = dummy_function;
  sigaction(SIGTRAP, &sact, NULL);
  sigaction(SIGSEGV, &sact, NULL);
  sigaction(SIGBUS, &sact, NULL);
  //dummy_function ();
  func3();
}

int
main (void)
{
        install_sigaction();
  return 0;
}

