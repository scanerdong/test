#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "test.h"
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>


using namespace std;

void BubbleSort(int32* pi32Array, int32 i32Len)
{
    int i,j,tmp,change;
    

    for(i=0; i<i32Len; i++)
    {
        change = 0;
        
        for(j=0;j<i32Len-i-1;j++)
        {
            if(pi32Array[j] > pi32Array[j+1])
            {
                tmp = pi32Array[j];
                pi32Array[j] = pi32Array[j+1];
                pi32Array[j+1] = tmp;
                change = 1;
            }
        }

        if(change == 0) return;
    }
}

void InsertSort(int32* pi32Array, int32 i32Len)
{
    int i,j,min,tmp;
    

    for(i=0; i<i32Len; i++)//有序区从0开始
    {
        //扫描无序区，将找到的最小值不断加入有序区的末尾
        min = i;//假设最小值为第i个
        for(j=i;j<i32Len;j++)
        {
            if(pi32Array[j] < pi32Array[min])
            {
                min = j;
            }
        }

        tmp = pi32Array[min];
        pi32Array[min] = pi32Array[i];
        pi32Array[i] = tmp;
    }
}

int32 BinSearch(int32* pi32Array, int32 i32Len, int32 k)
{
    int32 mid,low,high;
    low = 0; high = i32Len - 1;
    
    while(low <= high)
    {
        mid = (low + high) / 2;
        if(pi32Array[mid] == k) return mid;
        if(pi32Array[mid] > k) high = mid-1;
        else low = mid + 1;
    }

    return -1;
}

int main (void)
{
    int32 ai32Array[] = {-5, -8,-3, 45, 88, -47, 9, 5, 4,151,-7,88, 15, 94, 547, 124};
    InsertSort(ai32Array, sizeof(ai32Array)/sizeof(int32));

    for(int32 i=0; i<sizeof(ai32Array)/sizeof(int32); i++)
    {
        printf("%d ", ai32Array[i]);
    }

    //2分查找,返回找到的值的下标
    int32 k;
    k = BinSearch(ai32Array, sizeof(ai32Array)/sizeof(int32), 124);//设要找的值为124
    printf("\nk:%d\n", k);
    return 0;
}

