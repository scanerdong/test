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


#ifndef __SHM_HASH_TABLE_H_
#define __SHM_HASH_TABLE_H_

#include <sys/types.h>
#include <memory.h>
#include <iostream>
#include <sstream>
#include <rte_malloc.h>
#include <rte_eal.h>
#include <rte_rwlock.h>
#include "shm_hash_fun.h"
#include "shm_common.h"
#include "shm_bucket.h"

using std::ostream;
    
__SHM_STL_BEGIN

template <typename _Key, typename _Value, typename _HashFunc = hash<_Key>, typename _EqualKey = std::equal_to<_Key> >
class hash_table {
    public:
        typedef Node<_Key, _Value> node_type;
        typedef _Key key_type;
        typedef _Value value_type;
        typedef _HashFunc hasher;
        typedef _EqualKey key_equal;
        typedef NodePool<node_type> node_pool_t;
        typedef Bucket<node_type, key_type, value_type, key_equal>  bucket_type;

    public:
        hash_table(bool dynamic_growth = false, uint32 entries = DEFAULT_ENTRIES_NUM, uint32 buckets = DEFAULT_BUCKETS_NUM, uint32 bucket_age_num=0)
            : m_mask(0),m_entries_num(entries), m_buckets_num(buckets), m_bucket_array(NULL), m_dynamic_growth(dynamic_growth),m_bucket_age_num(bucket_age_num),m_bucket_age_index(0) {
                initialize();
            }

        ~hash_table(void) {finalize();}

        bool insert(const key_type & key, const value_type & value) {
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            // Put node to bucket
            return bucket->put(key, value);
        }

        template <typename _Params, typename _Modifier>
        bool insert(const key_type &key, const value_type &value, _Params& params, _Modifier &action) {
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            // Put node to bucket
            return bucket->put( key, value, params, action);
        }
		

        /*
         * @brief
         *  This method takes two parameters :
         *  key is an input parameter for hash table lookup
         *  ret is an output parameter to take the value if the key is in the hash table
         * */
        bool find(const key_type & key, value_type * ret = NULL) const {
            // Get bucket
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            // Search in this bucket
            return bucket->lookup(key, ret); 
        }

        bool erase(const key_type &key, value_type * ret = NULL) {
            // Get bucket
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            return bucket->remove(key, ret);
        }
		
        // Update the value
        template <typename _Params, typename _Modifier>
        bool erase(const key_type & key, _Params & params, _Modifier &action) {
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            return bucket->remove(key, params, action);
        }


        // Update the value
        template <typename _Params, typename _Modifier>
        bool cb(const key_type & key, _Params & params, _Modifier &action, bool writelock=true, bool releaselock=true) {
            sig_t sig = m_hash_func(key);
            bucket_type * bucket = get_bucket_by_sig(sig);

            return bucket->cb(key, params, action, writelock, releaselock);
        }
		
        // traversal all elements
        template <typename _Params, typename _Modifier>
        void traversal(_Params & params, _Modifier &action, bool writelock=true) {
			if (m_bucket_array == NULL)
				return;
			
			for (uint32 i = 0; i < m_buckets_num; ++i) {
				m_bucket_array[i].traversal( params, action, writelock);
			}
        }

        // Clear this hash table
        void clear(void) {
            if (m_bucket_array == NULL)
                return;

            for (uint32 i = 0; i < m_buckets_num; ++i) {
                m_bucket_array[i].clear();
            }
        }
		
        // traversal all elements
        template <typename _Params, typename _Modifier>
        void clear(_Params & params, _Modifier &action) {
			if (m_bucket_array == NULL)
				return;
			
            for (uint32 i = 0; i < m_buckets_num; ++i) {
                m_bucket_array[i].clear( params, action);
            }
        }

        template <typename _Params, typename _Modifier>
        void removebycb(_Params & params, _Modifier &action) {
			if (m_bucket_array == NULL)
				return;

            for (uint32 i = 0; i < m_bucket_age_num; ++i) {
                m_bucket_array[m_bucket_age_index].removebycb( params, action);
                
                m_bucket_age_index++;
                if(m_bucket_age_index >= m_buckets_num){
                    m_bucket_age_index = 0;
                }
            }
        }
		

        uint32 capacity(void) const {
            if (m_pnode_pool == NULL)
                return 0;

			return m_pnode_pool->capacity();
        }

        uint32 free_entries(void) const {
            if (m_pnode_pool == NULL)
                return 0;

			return m_pnode_pool->free_entries();
        }

        void str(ostream & os) const {
            os << "\nHash Table Information : " << std::endl;
            os << "** Total Entries : " << capacity() << std::endl;
            os << "** Free  Entries : " << free_entries() << std::endl;

			
			for (uint32 i = 0; i < m_buckets_num; ++i) {
				m_bucket_array[i].str(os);
			}
			
			
        }

    private:
        bool initialize(void) {
            // Adjust bucket number if necessary
            if (!is_power_of_2(m_buckets_num))
                m_buckets_num = convert_to_power_of_2(m_buckets_num);

            if(m_bucket_age_num == 0 || m_bucket_age_num > m_buckets_num)
            {
                m_bucket_age_num = m_buckets_num;
            }

            m_mask = m_buckets_num - 1;

			//初始化表的m_pnode_pool
            m_pnode_pool = static_cast<node_pool_t*>(rte_zmalloc("htnodepool", sizeof(node_pool_t), 0));
			::new (m_pnode_pool) node_pool_t(m_dynamic_growth, m_entries_num);

            // Allocate memory for bucket 
            char name[] = "bucket_array";
            uint32 bucket_array_size_in_bytes = m_buckets_num * sizeof(bucket_type);
            m_bucket_array = static_cast<bucket_type*>(rte_zmalloc(name, bucket_array_size_in_bytes, 0));
            if (m_bucket_array == NULL) {
                printf("no bucket array\n");
                return false;
            } else {
                // Initialize Buckets
                for (uint32 i = 0; i < m_buckets_num; ++i)
                    ::new (&m_bucket_array[i]) bucket_type(m_pnode_pool);
                return true;
            }
        }

        void finalize(void) {
            if (m_bucket_array) {
                for (uint32 i = 0; i < m_buckets_num; ++i) {
                    bucket_type * bucket = &(m_bucket_array[i]);
                    // call the destructor of this bucket
                    if (bucket) {
                        bucket->~bucket_type();
                    }
                }
                rte_free(m_bucket_array);
                m_bucket_array = NULL;
            }

			if(m_pnode_pool) {
				m_pnode_pool->~node_pool_t();
				rte_free(m_pnode_pool);
				m_pnode_pool = NULL;
			}
        }

        bucket_type * get_bucket_by_sig(sig_t sig) const {
	        return &m_bucket_array[sig & m_mask];
        }


    private:
        hasher       m_hash_func;
        uint32       m_mask;
		
		uint32       m_entries_num;
        uint32       m_buckets_num;
        bucket_type* m_bucket_array;
		
		bool		 m_dynamic_growth;
		node_pool_t* m_pnode_pool;
		uint32 m_bucket_age_num;
        uint32 m_bucket_age_index;
};

__SHM_STL_END

#endif
