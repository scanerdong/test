#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <list>
#include <stack>
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> 
#include <sys/syscall.h>  
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "shm_hash_map.h"

using namespace std;
#define gettid() syscall(__NR_gettid)  
typedef shm_stl::hash_map<unsigned long, unsigned long> TestHashMap;
#define MAX_WORKER_NUM 8

typedef struct
{
  unsigned long seed;
  unsigned int nbuckets;
  unsigned int nitems;
  unsigned int search_iter;
  int careful_delete_tests;
  int bihash_created;
  TestHashMap* test_hash;
  uint8_t add_ready_list[MAX_WORKER_NUM];
  uint8_t search_ready_list[MAX_WORKER_NUM];
  uint8_t del_ready_list[MAX_WORKER_NUM];
  std::vector<uint64_t>* keys_per_thread[MAX_WORKER_NUM];
} test_main_t_aaa;

test_main_t_aaa testmain;
uint8_t works_num = 0;

void addself_global_cgroup()
{
	char* name = "/sys/fs/cgroup/cpuset/tasks";
	FILE* globaltask = NULL;
	globaltask = fopen(name,"a");
	int pid = getpid();
	//printf("pid:%d\n",pid);
	if(globaltask)
	{
		fprintf(globaltask,"%d\n",pid);
		fclose(globaltask);
	}
}

double tsc_to_sec(uint64_t diff_tsc)
{
	return ((double)diff_tsc)/rte_get_timer_hz();
}
void wait_until_all_worker_ready (uint8_t* thread_ready_list)
{
	int i; 
	int all_ready = 0;
	thread_ready_list[rte_lcore_index(rte_lcore_id())] = 1; //I'm ready
	
	while(!all_ready){
		all_ready = 1;
		
		for (i = 0; i < works_num; i++)
		{
			if(!thread_ready_list[i])
			{
				all_ready = 0;
				break;
			}
		}
	}
}

static int
dpdk_thread_task(void* args)
{
	printf("in dpdk_thread_task\n");
	test_main_t_aaa *tm = (test_main_t_aaa *)args;
	int worker_index = rte_lcore_index(rte_lcore_id());
	std::vector<uint64_t> keys = *tm->keys_per_thread[worker_index];
	uint64_t *p;
	uint64_t diff_tsc, begin_tsc;
	int i,j;
	bool ret;
	unsigned long value;
	uint64_t total_searches;
	
	TestHashMap* h = tm->test_hash;
	printf("add %ld items to hashmap\n", tm->nitems);
	wait_until_all_worker_ready (tm->add_ready_list);
	begin_tsc = rte_rdtsc();
	for (i = 0; i < tm->nitems; i++)
	  {
		ret = h->insert(keys[i], i+1);
		if(ret == false)
			printf("insert %lu failed\n", keys[i]);
	  }
	diff_tsc = rte_rdtsc() - begin_tsc;
	printf("add %ld items finished, map size %ld, use time %.5f\n", tm->nitems, h->size(), tsc_to_sec(diff_tsc));
	//----------------------------------------------------------------
	printf( "Search for items %d times...\n", tm->search_iter);
	wait_until_all_worker_ready(tm->search_ready_list);
	begin_tsc = rte_rdtsc();
	
	for (j = 0; j < tm->search_iter; j++)
	  {
		for (i = 0; i < tm->nitems; i++)
	  {
	    ret = h->find(keys[i], &value);
	    if(ret == false)
			printf("find key:%lu failed\n", keys[i]);

		if (value != i + 1)
		  printf("[%d] search for key %ld returned %ld, not %d\n", i,
			 keys[i], value, i+1);
	  }
	  }
	
	diff_tsc = rte_rdtsc() - begin_tsc;
	total_searches = tm->search_iter * tm->nitems;
	
	if (diff_tsc > 0)
	  printf("%.2f searches per second\n",
		   (double)total_searches / tsc_to_sec(diff_tsc));
	
	printf("%ld searches in %.6f seconds\n", total_searches, tsc_to_sec(diff_tsc));
	
	//----------------------------------------------------------------
	printf("Delete items...\n");
	wait_until_all_worker_ready(tm->del_ready_list);
	begin_tsc = rte_rdtsc();
	
	for (i = 0; i < tm->nitems; i++)
	  {
	  	ret = h->erase(keys[i]);
		if(ret == false)
			printf("delete %ld failed\n", keys[i]);	
	  }
	diff_tsc = rte_rdtsc() - begin_tsc;
	printf("After deletions, should be empty...size %ld\n",  h->size());
	printf("%ld delete in %.6f seconds\n", tm->nitems, tsc_to_sec(diff_tsc));
	
	return 0;
}

uint64_t random_u64 (uint64_t * seed)
{
  *seed = 6364136223846793005ULL * *seed + 1442695040888963407ULL;
  return *seed;
}

int main(int argc, char *argv[])
{
	
	pthread_t threadId1,threadId2;
	int i32Ret;
	unsigned long i;
	char* pname = NULL;
	unsigned lcore_id, rx_lcore_id;
	//addself_global_cgroup();
	uint64_t work_cpu_mask;
	
	int retval = rte_eal_init(argc, argv);
	if (retval < 0) {
        printf("%s rte_eal_init() Fail!\n", __FUNCTION__);
		return -1;
    }

	for(i = 1; i<argc-1; i++)
	{
		if(strncmp(argv[i],"-c", 2) == 0){
			if(strlen(argv[i]) == 2){
				work_cpu_mask = strtoul(argv[i+1], NULL, 0);
			}else{
				work_cpu_mask = strtoul(argv[i]+2, NULL, 0);
			}
		}
	}
	
	//先创建一个unorderd_map，把所有的key保存起来
	std::unordered_map<uint64_t, uint64_t> umap;
	test_main_t_aaa* tm = &testmain;
	memset(&testmain, 0, sizeof(testmain));
	tm->seed = 0xdeaddabe;
	tm->nbuckets = 256*1024;
	//tm->table_size = 200*1024*1024; //200M
	tm->nitems = 1024*1024;
	tm->search_iter = 10; //搜索迭代次数  
	tm->careful_delete_tests = 1;
	
	printf("work_cpu_mask:0x%x\n", work_cpu_mask);
	for(i = 0; i < 64; i++)
	{
		if(work_cpu_mask&(1UL<<i))
		{
			tm->keys_per_thread[works_num] = new std::vector<uint64_t>();
			works_num++;
		}
	}
	
	for (i = 0; i < tm->nitems*works_num; i++)
	  {
		uint64_t rndkey;
	
		again:
		rndkey = random_u64 (&tm->seed);
		
		if(umap.find(rndkey) != umap.end())
			goto again; //确保每个key都是独一无二

		
		umap[rndkey] = i+1;
		
		tm->keys_per_thread[i%works_num]->push_back(rndkey);
	  }
	//创建待测hashmap
	TestHashMap test_hash("testhash",false,tm->nitems*works_num,tm->nbuckets*works_num,1000);
	tm->test_hash = &test_hash;

	tm->bihash_created = 1;
	
	/* launch per-lcore init on every lcore */
	rte_eal_mp_remote_launch(dpdk_thread_task, tm, CALL_MASTER);
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		if (rte_eal_wait_lcore(lcore_id) < 0) {
			break;
		}
	}


	return 0;
}

#if 0
void* ThreadTask(void* pArg)
{
    char* name = (char*)pArg;
	prctl(PR_SET_NAME,name);
	delete [] name;
	
    pthread_detach(pthread_self());
	
	
    for(;;)
    {
        
    }

    return NULL;
}


int main(int argc, char *argv[])
{
	
	pthread_t threadId1,threadId2;
	int i32Ret;
	unsigned long i;
	char* pname = NULL;
	unsigned int work_cpu_mask;
	//addself_global_cgroup();
	
	int retval = rte_eal_init(argc, argv);
	if (retval < 0) {
        printf("%s rte_eal_init() Fail!\n", __FUNCTION__);
		return -1;
    }

	if(argc < 2){
		printf("usage: ./%s work_cpu_mask\n", argv[0]);
		exit(-1);
	}
	work_cpu_mask = strtoul(argv[1], NULL, 0);
	for(i = 0; i < 8*sizeof(work_cpu_mask); i++)
	{
		if(work_cpu_mask&(1<<i))
		{
			printf("start work thread on %ld cpu\n", i);
			pthread_attr_t * p_attr = NULL;
				
			pthread_attr_t attr;
			cpu_set_t cpuset;
			
			CPU_ZERO(&cpuset);
			CPU_SET(i, &cpuset);
			pthread_attr_init(&attr);
			pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);
			p_attr = &attr;
			
			
			pname = new char[128];
			snprintf(pname, 128, "work%d", i);
			
			i32Ret = pthread_create(&threadId1, p_attr, ThreadTask, (void*)pname);
			if(i32Ret != 0)
			{
				printf("Create ThreadTask error\n");
			}
		}
			
	}
	
	
	while (1)
	{
		sleep(1);
	}


	return 0;
}

#endif

