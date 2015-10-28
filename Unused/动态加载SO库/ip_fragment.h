#ifndef _NIDS_IP_FRAGMENT_H
#define _NIDS_IP_FRAGMENT_H

#include <stdint.h>
#include <netinet/ip.h>

#define IPF_NOTF 1
#define IPF_NEW  2
#define IPF_ISF  3

struct sk_buff {
  char *data;
  int truesize;
};

struct timer_list {
  struct timer_list *prev;
  struct timer_list *next;
  int expires;
  unsigned long data;
  // struct ipq *frags;
};

struct hostfrags {
  struct ipq *ipqueue;
  int ip_frag_mem;
  u_int ip;
  int hash_index;
  struct hostfrags *prev;
  struct hostfrags *next;
};

/* Describe an IP fragment. */
struct ipfrag {
  int offset;			/* offset of fragment in IP datagram    */
  int end;			/* last byte of data in datagram        */
  int len;			/* length of this fragment              */
  struct sk_buff *skb;		/* complete received fragment           */
  unsigned char *ptr;		/* pointer into real fragment data      */ //指向该分片的数据部分
  struct ipfrag *next;		/* linked list pointers                 */
  struct ipfrag *prev;
};

/* Describe an entry in the "incomplete datagrams" queue. */
struct ipq {
  unsigned char *mac;		/* pointer to MAC header                */
  struct ip *iph;		/* pointer to IP header                 */ //指该队列第一个IP分片的头
  int len;			/* total length of original datagram    */ //组合所有分片后的tot_len
  short ihlen;			/* length of the IP header              */ //指该队列第一个IP分片的头长度
  short maclen;			/* length of the MAC header             */
  struct timer_list timer;	/* when will this queue expire?         */
  struct ipfrag *fragments;	/* linked list of received fragments    */ //收到的分片队列(可能被去掉叠加部分)
  struct hostfrags *hf;
  struct ipq *next;		/* linked list pointers                 */
  struct ipq *prev;
  // struct device *dev;	/* Device - for icmp replies */
};

/*
  Fragment cache limits. We will commit 256K at one time. Should we
  cross that limit we will prune down to 192K. This should cope with
  even the most extreme cases without allowing an attacker to
  measurably harm machine performance.
*/
#define IPFRAG_HIGH_THRESH		(256*1024) //有同一个目的地址的所有IP包占用的内存上限
#define IPFRAG_LOW_THRESH		(192*1024) //超过上限后内存将被清到这个值

//dongying 新增加IP分片处理类
class IpDefragProcessor
{
public:
	IpDefragProcessor(uint32_t u32HashSize);
	~IpDefragProcessor();
	int ip_defrag_stub(struct ip *iph, struct ip **defrag);
	
private:

#define mknew(x)	(x *)test_malloc(sizeof(x))

	void atomic_sub(int ile, int *co);
	void atomic_add(int ile, int *co);
	
	void kfree_skb(struct sk_buff * skb, int type);
	void panic(char *str);
	void add_timer(struct timer_list * x);
	
	void del_timer(struct timer_list * x);
	
	void frag_kfree_skb(struct sk_buff * skb, int type);
	
	void frag_kfree_s(void *ptr, int len);
	
	void* frag_kmalloc(int size, int dummy);
	
	struct ipfrag* ip_frag_create(int offset, int end, struct sk_buff * skb, unsigned char *ptr);
	
	void rmthis_host();
	
	struct ipq* ip_find(struct ip * iph);
	
	void ip_free(struct ipq * qp);
	
	void ip_expire(unsigned long arg);
	
	void ip_evictor(void);
	
 	struct ipq* ip_create(struct ip * iph);
	
	int	ip_done(struct ipq * qp);
	
	char* ip_glue(struct ipq * qp);
		
	char* ip_defrag(struct ip *iph, struct sk_buff *skb);
	
	int hostfrag_find(struct ip * iph);
	
	void hostfrag_create(struct ip * iph);
	
	int jiffies();
	
	char* test_malloc(int x);
	
	int frag_index(struct ip * iph);
	
	void no_mem(char *func);

	struct hostfrags **fragtable;
	struct hostfrags *this_host;
	uint64_t numpack;
	uint32_t hash_size;
	uint32_t timenow;
	uint32_t time0;
	struct timer_list *timer_head, *timer_tail;

	uint32_t dev_addon;
	uint32_t sk_buff_size;
};
#endif /* _NIDS_IP_FRAGMENT_H */

