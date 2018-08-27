
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Bruce.Li <jiangwlee@163.com>, 2014
 */


#ifndef __SHM_NODE_POOL_H_
#define __SHM_NODE_POOL_H_

#include <sys/types.h>
#include <memory.h>
#include <iostream>
#include <sstream>
#include <rte_malloc.h>
#include <rte_eal.h>
#include "ns_rw_lock.h"
#include <rte_spinlock.h>
#include "shm_hash_fun.h"
#include "shm_common.h"
#include <pthread.h>

//删除节点前需要确保节点没在外面被单独引用
#if 0 //USE_NODE_OUTSIDE
#define WAIT_FREE_NODE(node) do{\
	rte_rwlock_write_lock(&node->m_node_lock); \
	rte_rwlock_write_unlock(&node->m_node_lock); \
	}while(0);
#else
#define WAIT_FREE_NODE(node) 
#endif

__SHM_STL_BEGIN

using std::ostream;

/* typedef */
typedef u_int32_t sig_t;
typedef u_int32_t uint32;
typedef int32_t   int32;

template <typename _Node>
struct PrintNode {
    void operator() (const _Node & node, ostream & os) {
        os << "[" << node.index() << "] --> ";
    }
};

template <typename _Key, typename _Value>
class Node {
    public:
        Node () : m_next(NULL) {
		}
        ~Node () {}
        
        inline void fill(_Key k, _Value v) {
            m_key = k;
            m_value = v;
        }

        inline void set_next(Node * next) {m_next = next;} //
        inline void set_index(uint32 idx) {m_index = idx;} //只初始化时调用1次，不用加锁

		// 执行回调操作
		// 如果endloop为true表示把这个结点处理完了就结束循环，releaselock为fase表示不释放锁(这样外面也可以用这个节点，用完后手动调用releaselock)
        template <typename _Params, typename _Modifier>
        inline void cb(_Params& params, _Modifier &action, bool& endloop, const bool writelock=true, const bool releaselock=true) {
			if(writelock)
				m_node_lock.ns_write_lock();
			else
				m_node_lock.ns_read_lock();
			
            action(*this, params, endloop);

			if(releaselock)
			{
				if(writelock)
					m_node_lock.ns_write_unlock();
				else
					m_node_lock.ns_read_unlock();
			}
        }

		inline void release_rlock()
		{
			m_node_lock.ns_read_unlock();
		}
		inline void release_wlock()
		{
			m_node_lock.ns_write_unlock();
		}
		
        template <typename _Params, typename _Modifier>
        inline void remove(_Params& params, _Modifier &action, bool& needremove, bool& endloop) { //needremove为true才会真正的remove这个节点，如果endloop为true表示把这个结点处理完了就结束循环
        	m_node_lock.ns_write_lock();
            action(*this, params, needremove, endloop);
			m_node_lock.ns_write_unlock();
        }
		
        _Key key(void) const {return m_key;}
        inline void value(_Value* ret) {
			//m_node_lock.ns_read_lock();
			*ret = m_value;
			//m_node_lock.ns_read_unlock();
		}
		
        //inline sig_t signature(void) const {return m_sig;}
        inline Node * next(void) const {return m_next;}
        inline uint32 index(void) const {return m_index;}

        inline void str(std::ostream &os) {
            //os << "[ <" << m_key << ", " << m_value << ">, " << m_sig << " ] --> " << std::endl; 
        }

    public:
        _Key   m_key;
        _Value m_value; // The member of _Key and _Value should be volatile
        Node * volatile m_next;  // the pointer of next node
        uint32 m_index; // the index of this node in node list, it should never be changed after initialization
        ns_rw_lock m_node_lock; // read/write lock for node 
};

/*
 * @brief : FreeNodePool manages free nodes used by hashmap. A hashmap should always
 *          get a free node from FreeNodePool and return it back when it decides to
 *          erase a node from hashmap.
 *
 *          FreeNodePool can resize itself if all free nodes are exhausted. It creates
 *          a new free node list whose size is double of previous free node list. The
 *          max resize count is 32 by default. It means you can create 32 free node
 *          lists including the first one at most.
 *
 *          All free nodes in free node lists are chained together and be accessed
 *          from m_nodepool_head.
 *
 *          This class provides following methods to programmers:
 *          1. GetNode - Get a free node from FreeNodePool
 *          2. PutNode - Put a node to FreeNodePool
 *          3. PutNodeList - Put a list of nodes to FreeNodePool
 *
 *          Important:
 *          1. Programmers should not free any node outside of FreeNodePool
 *
 *          Following is a chart to illustrate this class:
 *
 *          m_freelist_array --> +-----------------------------------------------+
 *                                |     |     |     |     |     |     |     |     | 
 *                                +-----------------------------------------------+
 *                 [the first list]  |     |   [create the second free list if the first is exhausted]
 *                                   V     +-----> +-------------------------------+
 *           m_nodepool_head -->  +-----+          |   |   |   |   |   |   |   |   |
 *                                |     |          +-------------------------------+
 *                                +-----+            ^
 *                                |     |            |
 *                                +-----+            |
 *                                |     |            |
 *                                +-----+            |
 *                                |     |            | [chain the new created free nodes to m_nodepool_head]
 *                                +-----+            |
 *                                   |_______________|
 *
 * */
template <typename _Node>
class NodePool {
    public:
        typedef _Node node_type;
        static const uint32 MAX_RESIZE_COUNT = 1;
        static const uint32 DEFAULT_LIST_SIZE = 4096;  // The default size of the first free list

        NodePool(bool dynamic_growth=false, uint32 size = DEFAULT_LIST_SIZE)
            : m_init_size(size)
            , m_capacity(0)
            , m_free_entries(0)
            , m_freelist_num(0)
            , m_next_freelist_size(size)
            , m_nodepool_head(NULL)
            , m_dynamic_growth(dynamic_growth) {
	            //pthread_spin_init(&m_pool_lock,PTHREAD_PROCESS_PRIVATE);
                for (uint32 i = 0; i < m_freelist_num; ++i)
                    m_freelist_array[i] = NULL;

				if(m_dynamic_growth==false)
	                resize();
            }

        ~NodePool() {
			if(m_dynamic_growth==false)
			{
	            for (uint32 i = 0; i < m_freelist_num; ++i) {
	                void * free_list = (void *)(m_freelist_array[i]);
	                if (free_list != NULL) {
	                    rte_free(free_list);
	                    m_freelist_array[i] = NULL;
	                }
	            }
			}

            m_capacity = 0;
            m_free_entries = 0;
            m_freelist_num = 0;
            m_next_freelist_size = m_init_size;
            m_nodepool_head = NULL;
            //pthread_spin_destroy(&m_pool_lock);
	}

        // Get a free node
        inline node_type * get_node(void) {
        	if(m_dynamic_growth)
    		{
    			return get_node_from_hugepage();
    		}else{
    			return get_node_from_pool();
			}
        }

        // Return a node to free list
        inline void put_node(node_type * node) {
        	if(m_dynamic_growth)
    		{
    			return put_node_to_hugepage(node);
    		}else{
    			return put_node_to_pool(node);
			}
        }

        #if 0
        // Return nodes in a bucket to free list
        inline void put_nodelist(node_type *start, node_type *end, uint32 size) {
        	if(m_dynamic_growth)
    		{
    			return put_nodelist_to_hugepage(start, end, size);
    		}else{
    			return put_nodelist_to_pool(start, end, size);
			}
        }
        #endif

        // Following three methods do not use lock
        inline uint32 capacity(void) const {return m_capacity;}
        inline uint32 free_entries(void) const {return m_free_entries;}
        inline bool is_dynamic_growth(void) const {return m_dynamic_growth;}
        inline void print_info(void) const {
            std::ostringstream os;
	        print_info(os);
            std::cout << os.str().c_str() << std::endl;
        }

		inline void print_info(std::ostream & os) const {
            str(os);

            os << "\nFree Node Pool : " << std::endl;
            node_type * start = m_nodepool_head;
            PrintNode<node_type> action;
            uint32 cnt = 0;
            while (start && cnt < m_free_entries) {
                action(*start, os);
                start = start->next();
                ++cnt;
            }
        }

    private:
		
		inline node_type* get_node_from_pool(void) {
	        //pthread_spin_lock(&m_pool_lock);
			m_pool_lock.ns_write_lock();
            if (m_nodepool_head == NULL)
                resize();

            if (m_nodepool_head == NULL)
        	{
        		//pthread_spin_unlock(&m_pool_lock);
				m_pool_lock.ns_write_unlock();
                return NULL;
        	}
			
            // get the first free node
            node_type * head = m_nodepool_head;
            m_nodepool_head = head->next();

            --m_free_entries;
			//pthread_spin_unlock(&m_pool_lock);
			m_pool_lock.ns_write_unlock();
			
            construct_node(head);
			
            return head;
		}
		
        inline void put_node_to_pool(node_type * node) {
            if (node == NULL)
                return;

			WAIT_FREE_NODE(node)

			//pthread_spin_lock(&m_pool_lock);
			m_pool_lock.ns_write_lock();
            // Put this node at the front of m_free_node_list
            node->set_next(m_nodepool_head); 
            m_nodepool_head = node;

            ++m_free_entries;
			//pthread_spin_unlock(&m_pool_lock);
			m_pool_lock.ns_write_unlock();
        }

		#if 0
        void put_nodelist_to_hugepage(node_type *start, node_type *end, uint32 size) {
            // If start or end is NULL, do nothing
            if (!start || !end)
                return;

			node_type * tmp; 
			while(start != end->next())
			{
				WAIT_FREE_NODE(start)
				
				tmp = start;
				rte_free(tmp);
				rte_atomic32_dec((rte_atomic32_t*)&m_capacity);
				start = start->next();
			}
        }
		#endif

		inline node_type* get_node_from_hugepage(void) {
            node_type * new_node = static_cast<node_type *>(rte_zmalloc(NULL, sizeof(node_type), 0));
            if (new_node == NULL)
                return NULL;
			construct_node(new_node);
			rte_atomic32_inc((rte_atomic32_t*)&m_capacity);
			return new_node;
		}

        inline void put_node_to_hugepage(node_type * node) {
            if (node == NULL)
                return;

			WAIT_FREE_NODE(node)
			
			rte_free(node);
			rte_atomic32_dec((rte_atomic32_t*)&m_capacity);
        }
		
        #if 0
        void put_nodelist_to_pool(node_type *start, node_type *end, uint32 size) {
            // If start or end is NULL, do nothing
            if (!start || !end)
                return;
			pthread_spin_lock(&m_pool_lock);
            return_nodelist(start, end, size);
			pthread_spin_unlock(&m_pool_lock);
        }
		#endif

        // Create a new free node list and chain it to free node pool
        inline void resize(void) {
        
            // Have reached the maxinum size
            if (m_freelist_num >= MAX_RESIZE_COUNT)
        	{
        		//printf("error! exceed max capacity %d\n", m_capacity);
                return;
        	}
            // Create a new free list
            uint32 node_cnt = m_next_freelist_size;
            uint32 list_size_in_byte = node_cnt * sizeof(node_type);
            std::ostringstream name;
            name << "NodePool_FreeList_" << m_freelist_num;
            node_type * new_list = static_cast<node_type *>(rte_zmalloc(name.str().c_str(), list_size_in_byte, 0));
            if (new_list == NULL)
                return;

            // Now we have created the new free list successfully, add it to m_freelist_array
            // and free node pool
            initialize_freenode_list(new_list, node_cnt, m_capacity);
            m_freelist_array[m_freelist_num] = new_list;
            node_type * end_of_list = &new_list[node_cnt - 1];
            return_nodelist(new_list, end_of_list, node_cnt); // PutNodeList will calculate m_free_entries

            // Calculate new capacity, free_list_num and next_free_list_size 
            m_capacity += node_cnt;
            m_freelist_num++;
            m_next_freelist_size = node_cnt << 1;

			//printf("dynamic growth capacity %d, m_freelist_num %d, m_next_freelist_size %d\n", 
			//	m_capacity, m_freelist_num, m_next_freelist_size);
        }

        inline void initialize_freenode_list(node_type *list, uint32 size, uint32 index_start) {
            uint32 i = 0;
            for ( ; i < size - 1; ++i) {
                // call the constructor of node_type
                node_type * node = &list[i];
                construct_node(node);
                node->set_next(&list[i + 1]);
                node->set_index(index_start++);
            }

            // Set the index of last node
            list[i].set_index(index_start);
        }

        // This private function does not use lock
        inline void return_nodelist(node_type *start, node_type *end, uint32 size) {
            // If start or end is NULL, do nothing
            if (!start || !end)
                return;

            // Put the node list decribed by start and end at the front of m_nodepool_head
            end->set_next(m_nodepool_head);
            m_nodepool_head = start;
            m_free_entries += size;
        }

        inline void construct_node(node_type *node) {::new ((void *)node) node_type;}

        inline void str(std::ostream &os) const {
            os << "Node Pool Status : " << std::endl;
            os << "Capacity      : " << m_capacity << std::endl;
            os << "Free entries  : " << m_free_entries << std::endl;
            os << "Free list num : " << m_freelist_num << std::endl;
        }

    private:
        volatile uint32     m_init_size;
        volatile uint32     m_capacity;           // the capacity of this free node pool
        volatile uint32     m_free_entries;       // the count of available free nodes in this pool
        volatile uint32     m_freelist_num;       // how many free lists we have now
        volatile uint32     m_next_freelist_size; // the size of next free list
        node_type * volatile m_nodepool_head;      // the head of free node pool
        node_type * volatile m_freelist_array[MAX_RESIZE_COUNT]; // free lists
        
        //pthread_spinlock_t m_pool_lock; // read/write lock for pool
        ns_rw_lock m_pool_lock;
        const bool m_dynamic_growth; // true:memmory is dynamic growth; false:static malloc all needed memmory
};

__SHM_STL_END

#endif
