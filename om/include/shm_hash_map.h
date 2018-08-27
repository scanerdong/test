/*
 * return  m_ht->find(key, ret);
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


#ifndef __SHM_HASH_MAP_H_
#define __SHM_HASH_MAP_H_

#include "shm_hash_table.h"
#include "shm_profiler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

#include <rte_memzone.h>
#include <rte_string_fns.h>

#ifdef DEBUG_HASH_MAP
#define RETURN_FALSE_IF_NULL(ptr) do {\
    if (ptr == NULL) return false;\
} while (0)
#else
#define RETURN_FALSE_IF_NULL
#endif

#define SHM_NAME_SIZE 32

__SHM_STL_BEGIN

template <typename _Key, typename _Value, typename _HashFunc = hash<_Key>, typename _EqualKey = std::equal_to<_Key> >
class hash_map {
    public:
        typedef _Key key_type;
        typedef _Value value_type;
        typedef _HashFunc hasher;
        typedef _EqualKey key_equal;
        typedef hash_table<key_type, value_type, hasher, key_equal> _Ht;

    public:
		//if dynamic_growth == true; entries is not used.
        hash_map(const char * name, bool dynamic_growth = true, uint32 entries = DEFAULT_ENTRIES_NUM, uint32 buckets = DEFAULT_BUCKETS_NUM, uint32 bucket_age_num=0)
			: m_entries(entries), m_buckets(buckets), m_dynamic_growth(dynamic_growth),m_bucket_age_num(bucket_age_num) {
                     snprintf(m_name, sizeof(m_name), "HT_%s", name);
					 create_or_attach();
                 }

        hash_map(bool dynamic_growth = true, uint32 entries = DEFAULT_ENTRIES_NUM, uint32 buckets = DEFAULT_BUCKETS_NUM, uint32 bucket_age_num=0)
			: m_entries(entries), m_buckets(buckets), m_dynamic_growth(dynamic_growth),m_bucket_age_num(bucket_age_num) {
			         m_name[0] = '\0'; 
					 create_or_attach();
	             }

        ~hash_map() {
            if (rte_eal_process_type() == RTE_PROC_PRIMARY)
                m_ht->~_Ht();

			detach();
        }

        inline bool create_or_attach(void) {
            uint32 shm_size = sizeof(_Ht);
            const rte_proc_type_t proc_type = rte_eal_process_type();
			rte_atomic32_t id;
			id.cnt = ((uint64_t)this);

            if (proc_type == RTE_PROC_PRIMARY) {
                void* buf = NULL;
                if(m_name[0]){
                    const struct rte_memzone * zone = rte_memzone_reserve(&m_name[0], shm_size, 0, RTE_MEMZONE_SIZE_HINT_ONLY);
                    buf = zone->addr;
                }else{
                    snprintf(m_name, sizeof(m_name), "HT_ANO_%d", rte_atomic32_add_return(&id, 1));
                    buf = rte_zmalloc(m_name, shm_size, 0);
                }
                
                m_ht = ::new (buf) _Ht(m_dynamic_growth, m_entries, m_buckets, m_bucket_age_num);
            } else if (proc_type == RTE_PROC_SECONDARY) {
                if(m_name[0]){
                    const struct rte_memzone * zone = rte_memzone_lookup(&m_name[0]);
                    m_ht = static_cast<_Ht*>(zone->addr);
                }else m_ht = NULL;
            } else {
                m_ht = NULL;
            }

            if (m_ht) {
                return true;
            } else {
                return false;
            }
        }

        inline bool detach(void) {
            const rte_proc_type_t proc_type = rte_eal_process_type();
            if (proc_type == RTE_PROC_PRIMARY) {
				rte_memzone_free(rte_memzone_lookup(&m_name[0]));
            }
			m_ht = NULL;
	        return true;
        }

        template <typename _Params, typename _Modifier>
        inline bool find(const key_type &key, _Params& params, _Modifier &action) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->cb(key, params, action, false, true);
        }

        inline bool find(const key_type &key, value_type * ret = NULL) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->find(key, ret);
        }

        inline bool insert(const key_type &key, const value_type &value) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->insert(key, value);
        }

        template <typename _Params, typename _Modifier>
        inline bool insert(const key_type &key, const value_type &value, _Params& params, _Modifier &action) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->insert(key, value, params, action);
        }

        template <typename _Params, typename _Modifier>
        inline bool erase(const key_type &key, _Params& params, _Modifier &action) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->erase(key, params, action);
        }
        inline bool erase(const key_type &key, value_type * ret = NULL) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->erase(key, ret);
        }

        template <typename _Params, typename _Modifier>
        inline bool update(const key_type &key, _Params& params, _Modifier &action) {
            RETURN_FALSE_IF_NULL(m_ht);
            return m_ht->cb(key, params, action, true, true);
        }

		//if writelock=true, write lock on node; else readlock on node
        template <typename _Params, typename _Modifier>
        inline void traversal(_Params& params, _Modifier &action, bool writelock=true) {
            if(m_ht == NULL) return;
            return m_ht->traversal(params, action, writelock);
        }

        template <typename _Params, typename _Modifier>
        inline void removebycb(_Params & params, _Modifier &action) {
            if(m_ht == NULL) return;
            return m_ht->removebycb(params, action);
        }

        template <typename _Params, typename _Modifier>
        inline void clear(_Params& params, _Modifier &action) {
            if(m_ht == NULL) return;
            return m_ht->clear(params, action);
        }

        inline void clear(void) {
            if (m_ht) m_ht->clear();
        }

        #if 0 //不再使用下面这两个函数USE_NODE_OUTSIDE
		//add get node operator here
		template <typename _Params, typename _Modifier>
		inline bool get_node_with_rlock(const key_type &key, _Params& params, _Modifier &action) {
			RETURN_FALSE_IF_NULL(m_ht);
			return m_ht->cb(key, params, action, false, false);
		}
		
		template <typename _Params, typename _Modifier>
		inline bool get_node_with_wlock(const key_type &key, _Params& params, _Modifier &action) {
			RETURN_FALSE_IF_NULL(m_ht);
			return m_ht->cb(key, params, action, true, false);
		}
        #endif
		
        inline void get_hash_map_info(std::ostringstream& os) {
            if (m_ht) {
                m_ht->str(os);
            } else {
                os << "Hash table is not created!" << std::endl;
            }
        }

        inline uint32 capacity(void) const {
            if (m_ht)
                return m_ht->capacity();
            else
                return 0;
        }

        inline uint32 free_entries(void) const {
            if (m_ht)
                return m_ht->free_entries();
            else
                return 0;
        }

        inline uint32 size(void) const {
            if (m_ht)
				return m_ht->capacity() - m_ht->free_entries();
			else 
				return 0;
        }

    private:
		uint32 m_entries;
        uint32 m_buckets;
        char   m_name[SHM_NAME_SIZE];
        _Ht *  m_ht;
		bool   m_dynamic_growth;
        uint32 m_bucket_age_num;
};

#undef SHM_NAME_SIZE

__SHM_STL_END

#endif
