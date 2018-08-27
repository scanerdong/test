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


#ifndef __SHM_BUCKET_H_
#define __SHM_BUCKET_H_

#include <sys/types.h>
#include <memory.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "shm_node_pool.h"
#include "shm_profiler.h"
#include "ns_rw_lock.h"
using std::ostream;
    
__SHM_STL_BEGIN

template <typename _Node, typename _Key, typename _Value, typename _KeyEqual>
class Bucket {
    public:
        typedef _Node node_t;
        typedef _Key  key_t;
        typedef _Value value_t;
        typedef NodePool<node_t> node_pool_t;

    public:
        Bucket (node_pool_t* pnode_pool)
            : m_pnode_pool(pnode_pool), m_size(0), m_head(NULL){
                
            }
        ~Bucket () {clear();}

        inline void clear(void) {
            if (m_head == NULL) {
                m_size = 0;
                return;
            }

            m_lock.ns_write_lock();
            
            node_t * current = m_head;
			
            while (current) {
				m_head = current->next(); //delete head
				m_pnode_pool->put_node(current);
				current = m_head;
				--m_size;
            }

            m_lock.ns_write_unlock();
            return;
        }

        template <typename _Params, typename _Modifier>
        inline void clear(_Params &params, _Modifier &action) {
            if (m_head == NULL) {
                m_size = 0;
                return;
            }

            m_lock.ns_write_lock();
			
            node_t * current = m_head;
			bool endloop = false;
			
            while (current) {
				current->cb(params, action, endloop);
				
				m_head = current->next(); //delete head
				m_pnode_pool->put_node(current);
				current = m_head;
				--m_size;
				
				if(endloop) break;
            }

            m_lock.ns_write_unlock();
            return;
        }

        // Put a node at the head of this bucket
        inline bool put(const key_t &key, const value_t &value) {
            bool ret = true;
            node_t * node = NULL;

            m_lock.ns_write_lock();

            // check if this key is already in this bucket
            if (find_node(key)) {
                ret = false;
                m_lock.ns_write_unlock();
				return ret;
            }

            node = m_pnode_pool->get_node();
            if (node == NULL) {
                ret = false;
                m_lock.ns_write_unlock();
				return ret;
            }
            node->fill(key, value);
            node->set_next(m_head);
            m_head = node;
            ++m_size;
            
            m_lock.ns_write_unlock();
            return ret;
        }

        template <typename _Params, typename _Modifier>
        inline bool put(const key_t &key, const value_t &value, _Params& params, _Modifier &action) {
			bool ret = true;
			node_t * node = NULL;

			m_lock.ns_write_lock();

			// check if this key is already in this bucket
			if (find_node(key)) {
				ret = false;
                m_lock.ns_write_unlock();
				return ret;
			}

			node = m_pnode_pool->get_node();
			if (node == NULL) {
				ret = false;
                m_lock.ns_write_unlock();
				return ret;
			}
			node->fill(key, value);
			node->set_next(m_head);
			m_head = node;
			++m_size;
			bool endloop = false;
			node->cb(params, action, endloop, true, true);
			m_lock.ns_write_unlock();
			return ret;
        }


        // Lookup a node by signature and key
        inline bool lookup(const key_t &key, value_t * ret) {
            m_lock.ns_read_lock();

            node_t* node = find_node(key);
            if (node && ret) node->value(ret);

            m_lock.ns_read_unlock();

            if (node)
                return true;
            else
                return false;
        }

        // Remove a node from this bucket
        inline bool remove(const key_t &key, value_t * ret) {
	        m_lock.ns_write_lock();

            bool found = false;

			node_t * pre = NULL;
            node_t * current = m_head;
			
            while (current) {
                if (m_equal_to(key, current->key())) {
					found = true;
					
					if (ret)
						current->value(ret);
					
					if(current == m_head) {
						m_head = current->next(); //delete head
						m_pnode_pool->put_node(current);
						current = m_head;
					}
					else{
						pre->set_next(current->next());
						m_pnode_pool->put_node(current);
						current = pre->next();
					}
					
					--m_size;
					break;
				}else{
					pre = current;
					current = pre->next();
				}
			}

            m_lock.ns_write_unlock();

            if (found)
                return true;
            else
                return false;
        }

        // update a node in this bucket
        template <typename _Params, typename _Modifier>
        inline bool remove(const key_t &key, _Params &params, _Modifier &action) {
            m_lock.ns_write_lock();

            bool ret = false;
			
			node_t * pre = NULL;
            node_t * current = m_head;
			bool endloop = false;
			bool needremove;
            while (current) {
                if (m_equal_to(key, current->key())) {
					needremove = false;
					current->remove(params, action, needremove, endloop);
					if(!needremove){
						break;
					}
					
					ret = true;
					
					if(current == m_head) {
						m_head = current->next(); //delete head
						m_pnode_pool->put_node(current);
						current = m_head;
					}
					else{
						pre->set_next(current->next());
						m_pnode_pool->put_node(current);
						current = pre->next();
					}
					
					--m_size;
					
					break;
                }else{
					pre = current;
	                current = pre->next();
            	}
            }
            
            m_lock.ns_write_unlock();
            return ret;
        } 
		

        // update a node in this bucket
        template <typename _Params, typename _Modifier>
        inline bool cb(const key_t &key, _Params &params, _Modifier &action, const bool writelock=true, const bool releaselock=true) {
            m_lock.ns_read_lock();

            bool ret = false;

            node_t * current = m_head;
			bool endloop = false;
			
            while (current) {
                if (m_equal_to(key, current->key())) {
					current->cb(params, action, endloop, writelock, releaselock);
					ret = true;
					break;
                }

                current = current->next();
            }
            
            m_lock.ns_read_unlock();
            return ret;
        } 
		
        // traversal all elements in this bucket
        template <typename _Params, typename _Modifier>
        inline void traversal(_Params &params, _Modifier &action, bool writelock=true) {
            m_lock.ns_read_lock();

            node_t * current = m_head;
			bool endloop = false;
            while (current) {
				current->cb(params, action, endloop, writelock);
                current = current->next();
				if(endloop) break;
            }
            
            m_lock.ns_read_unlock();
        } 


        //
        template <typename _Params, typename _Modifier>
        inline void removebycb(_Params &params, _Modifier &action) {
	        m_lock.ns_write_lock();
			bool ret = false;
			
			node_t * pre = NULL;
			node_t * current = m_head;
			bool endloop = false;
			bool needremove;
			while (current) {
				needremove = false;
				current->remove(params, action, needremove, endloop);
				if(!needremove){
					pre = current;
					current = pre->next();
					continue;
				}
				
				ret = true;
				
				if(current == m_head) {
					m_head = current->next(); //delete head
					m_pnode_pool->put_node(current);
					current = m_head;
				}
				else{
					pre->set_next(current->next());
					m_pnode_pool->put_node(current);
					current = pre->next();
				}
				
				--m_size;
				
				if(endloop) break;
			}
			
			m_lock.ns_write_unlock();
			return ;
        } 


        inline uint32  size(void) const {return m_size;}

        inline void str(ostream &os) const {
            os << "\nBucket Size : " << m_size << std::endl;
            node_t* curr = m_head;
            while (curr) {
                curr->str(os);
                curr = curr->next();
            }
        }

    private:
        inline node_t * find_node(const key_t &key) const {
            // Search in this bucket
            node_t * current = m_head;
            while (current) {
                if (m_equal_to(key, current->key())) {
                    break;
                }

                current = current->next();
            }

            return current;
        }

    public:
        node_pool_t* m_pnode_pool;
        volatile uint32 m_size; // the size of this bucket
        node_t * volatile m_head; // the pointer of the first node in this bucket
        _KeyEqual m_equal_to;
        ns_rw_lock m_lock;
}; 

__SHM_STL_END

#endif
