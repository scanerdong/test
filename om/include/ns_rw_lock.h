/*
 *
 */

#ifndef _NS_RW_LOCK_H
#define _NS_RW_LOCK_H
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in_systm.h>
#include<ctype.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sched.h>  
////�������ȼ���  �ڶ��Ƚ϶�ĳ������ܸ���
class ns_rw_lock {
public:
	unsigned int  cnt;
public:
	ns_rw_lock()
		: cnt(0) {
	}
	;
	inline void ns_read_lock() {


		while (__sync_add_and_fetch(&cnt, 1) & 0x80000000)
		{
			asm volatile(
			"lock ;decl %[cnt]"
			: [cnt] "=m" (cnt)   
			: "m" (cnt)          
			);
			sched_yield();
		}
	}
	inline bool ns_try_read_lock() {


		if (__sync_add_and_fetch(&cnt, 1) & 0x80000000)
		{
			asm volatile(
			"lock ;decl %[cnt]"
			: [cnt] "=m" (cnt)   
			: "m" (cnt)          
			);
			return false;
		}
		return true;
	}
	
	inline void ns_read_unlock() {
		asm volatile(
			
				"lock ;decl %[cnt]"
				: [cnt] "=m" (cnt) 
				: "m" (cnt)          
				);
	}
	inline void ns_write_lock() {
		while (__sync_val_compare_and_swap(&cnt, 0, 0x80000000) != 0)//��ȡ���Ժ����дcnt
		{
			sched_yield();
		}
	}
	inline bool ns_try_write_lock() {
		if (__sync_val_compare_and_swap(&cnt, 0, 0x80000000) != 0)//��ȡ���Ժ����дcnt
		{
			return false;
		}
		return true;
		
	}
	inline void ns_write_unlock() {
		while (__sync_val_compare_and_swap(&cnt, 0x80000000, 0) != 0x80000000)//��ȡ���Ժ����дcnt
		{
			sched_yield();
		}
	}
};

////д�����ȼ���  ��д�Ƚ϶�ĳ������ܸ���
class ns_wr_lock {
	unsigned int  cnt;
public:
	ns_wr_lock()
		: cnt(0) {
	}
	;
	inline void ns_read_lock() {
		
		while (1)
		{
			if (cnt & 0x80000000) {sched_yield();continue;}
			;
			   
			if (__sync_add_and_fetch(&cnt, 1) & 0x80000000)
			{
				asm volatile(
				"lock;decl %[cnt]"
				: [cnt] "=m" (cnt)   
				: "m" (cnt)          
				);
				
				sched_yield();
				continue;
			}
			break;
		}

	}
	
	inline void ns_read_unlock() {
		asm volatile(
				"lock ;decl %[cnt]"
				: [cnt] "=m" (cnt) 
				: "m" (cnt)          
				);
	}
	inline void ns_write_lock() {
		while (__sync_val_compare_and_swap(&cnt, 0, 0x80000000) != 0)//��ȡ���Ժ����дcnt
		{
			sched_yield();
		}
	}
	inline void ns_write_unlock() {
	 	
		while (__sync_val_compare_and_swap(&cnt, 0x80000000, 0) != 0x80000000)//��ȡ���Ժ����дcnt
		{
			sched_yield();
		}
	}
};

#endif

