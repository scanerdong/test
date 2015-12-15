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
using namespace std;

struct Node
{
    Node(int v) : next(NULL)
    {this->v = v;}
    
    
    int v;
    Node* next;
};

struct List
{
    List()
        :p(NULL),size(0)
    {}
    
    void append(Node* node){
        if(!p)
        {p = node;return;}
        
        Node* tail = p;
        while(tail->next) tail = tail->next;
        
        tail->next = node;
        size++;
    }
    
    Node* p;
    int size;
};

//list的合并
void merge1(List* a, List* b)
{
    Node* ahead = a->p;
    Node* bhead = b->p;
    Node node(0);
    Node* r = &node;
    
    while(ahead && bhead)
    {
        if(ahead->v <= bhead->v)
        {
            r->next = ahead;
            ahead = ahead->next;
            
        }else{
            r->next = bhead;
            bhead = bhead->next;
        }

        r = r->next;
    }

    if(ahead != NULL) {r->next = ahead;}
    if(bhead != NULL) {r->next = bhead;}
    a->p = node.next;
}

#if 0
int main()
{
    List* a = new List();
    List* b = new List();
    a->append(new Node(1));
    a->append(new Node(3));
    a->append(new Node(5));
    a->append(new Node(7));
    a->append(new Node(9));
    
    b->append(new Node(2));
    b->append(new Node(4));
    b->append(new Node(6));
    b->append(new Node(8));
    b->append(new Node(10));
    b->append(new Node(11));
    b->append(new Node(12));
    b->append(new Node(13));
    b->append(new Node(14));
    b->append(new Node(15));
    b->append(new Node(16));
    b->append(new Node(17));
    
    a->append(new Node(17));
    a->append(new Node(17));
    a->append(new Node(17));
    a->append(new Node(18));
        
    merge1(a,b);
    
    Node* cur = a->p;
    while(cur){
        printf("%d ", cur->v);
        cur = cur->next;
    }
    printf("\n");
    return 0;
}
#endif

/**
 * merge_sort: 非递归实现 --迭代
 * 非递归思想: 将数组中的相邻元素两两配对。用merge函数将他们排序，
 * 构成n/2组长度为2的排序好的子数组段，然后再将他们排序成长度为4的子数组段，
 * 如此继续下去，直至整个数组排好序。
**/

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------
// merge_sort(): 非递归实现-自底向上
// 将原数组划分为left[min...max) 和 right[min...max)两部分
void merge_sort(int *list, int length)
{
    int i, left_min, left_max, right_min, right_max, next;
    int *tmp = (int*)malloc(sizeof(int) * length);

    if (tmp == NULL)
    {
        fputs("Error: out of memory\n", stderr);
        abort();
    }

    for (i = 1; i < length; i *= 2) // i为步长，1,2,4,8……
    {
        for (left_min = 0; left_min < length - i; left_min = right_max)//不断的将相邻的两个队列进行归并
        {
            right_min = left_max = left_min + i;
            right_max = left_max + i;

            if (right_max > length)
                right_max = length;

            printf("merge [%d, %d) [%d, %d)\n", left_min, left_max, right_min, right_max);

            next = 0;
            while (left_min < left_max && right_min < right_max)
                tmp[next++] = list[left_min] > list[right_min] ? list[right_min++] : list[left_min++];

            while (left_min < left_max)
                list[--right_min] = list[--left_max];

            while (next > 0)
                list[--right_min] = tmp[--next];
        }
    }

    free(tmp);

}

//---------------------------------------
//递归方式归并排序
//合并[first,mid]，[mid+1,last]
void merge(int* a, int first, int mid, int last, int* tmp)
{
	int i = first;
	int j = mid+1;
	int n = 0;
	
	while(i <= mid && j <=last){
		if(a[i] > a[j]){
			tmp[n++] = a[j++];
		}else{
			tmp[n++] = a[i++];
		}
	}
	
	while(i <= mid){
		tmp[n++] = a[i++];
	}
	
	while(j <= last){
		tmp[n++] = a[j++];
	}
	
	for(i = 0; i < n; i++){
		a[first + i] = tmp[i];
	}
}

void sort(int* a, int first, int last, int* tmp)
{
	if(first < last)
	{
		int mid = (first + last)/2;
		sort(a, first, mid, tmp);
		sort(a, mid+1, last, tmp);
		merge(a, first, mid, last, tmp);
        printf("merge [%d, %d] [%d, %d]\n", first, mid, mid+1, last);
	}
}

bool merge_sort1(int* a, int len)
{
	int* p = new int[len];
	if(p==NULL) return false;
	
	sort(a, 0, len-1, p);
	
	delete [] p;
	return true;
}

//-------------------------------------
//将merge_sort1改成非递归
void merge_sort2(int *list, int length)
{
    int i, left_min, left_max, right_min, right_max, next;
    int *tmp = (int*)malloc(sizeof(int) * length);

    if (tmp == NULL)
    {
        fputs("Error: out of memory\n", stderr);
        abort();
    }

    for (i = 1; i < length; i *= 2) // i为步长，1,2,4,8……
    {
        for (left_min = 0; left_min < length - i; left_min = right_max)
        {
            right_min = left_max = left_min + i;
            right_max = left_max + i;

            if (right_max > length)
                right_max = length;

            printf("merge [%d, %d) [%d, %d)\n", left_min, left_max, right_min, right_max);
            merge(list, left_min, left_max-1, right_max-1, tmp);

        }
    }

    free(tmp);

}


int Func(int* A, int nSize)
{
	int first_0_pos = nSize;
	int i,j,tmp;
	int done;
	
	if(nSize<1) return -1;
	
	while(first_0_pos>0 && A[first_0_pos-1]==0) //从后向前移到第一个非0位置
	{
		first_0_pos--; 
	}
	
	if(first_0_pos == 0) return 0;
	
	for(i=0; i<first_0_pos; i++){
		if(A[i]==0)
		{
			A[i] = A[first_0_pos-1];
			A[first_0_pos-1] = 0;
			first_0_pos--;
		}
	}
	
	//对[0,first_0_pos)之间冒泡排序
	for(i=0;i<first_0_pos;i++)
	{
		done = 1;
		for(j=0;j<first_0_pos-i-1;j++)
		{
			if(A[j]>A[j+1]){
				tmp = A[j];
				A[j] = A[j+1];
				A[j+1] = tmp;
				done = 0;
			}
		}
		
		if(done == 1) break;
	}
	
	return first_0_pos;
}



void f()
{
    int n5;
    int times = 0;
    int sum;
    for(n5=0; n5<=20; n5++)
    {
        sum = n5*5;

        //最多可用的2块数
        times += sum/2;//最多可以有多少次2块
        times++; //全为1块
    }
    
    printf("%d\n", times);

    times = 0;
    int x,y,z;
    for (x=0; x<=100; x++) 
        for (y=0; y<=50; y++) 
            for (z=0; z<=20; z++) 
                if ((x+2*y+5*z)==100) times++;
    
    printf("%d\n", times);
}

int main(void)
{
    int a[30] = { 0, 0, 0, 5, 2, 4, 7, 1, 0, 0, 3, 2, 6, 0, 8, 12, 9, 71,88 , 0, 99, 94, 44, 33, 22, 77, 88, 0, 0, 0};
    //merge_sort2(a, 30);
    //std::sort(a,a+30);
    int ret  = Func(a,30);
    for(int i = 0; i<30; i++){
        printf("%d ", a[i]);
    }

    printf("ret=%d\n", ret);

    f();
    
    return 0;
}



