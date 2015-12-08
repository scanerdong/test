#ifndef DPO_OBJ_FACTORY_H_
#define DPO_OBJ_FACTORY_H_


namespace dpo
{

//obj最少8字节，即一个指针大小
template<typename T>
struct obj_factory
{
	typedef T value_type;

	obj_factory(const value_type init_v = value_type()) :
		_mem(0), _head(0), _free_num(0), _init_v(init_v)
	{
	}

	bool open(const size_t num_of_elements)
	{
		_head = 0;
		_mem = new char[sizeof(value_type)* num_of_elements];
		if(_mem) {
			for(size_t n=0; n<num_of_elements; n++) {
				push(_mem + sizeof(value_type) * n);
			}
			return true;
		} else {
			return false;
		}
	}

	void close()
	{
		if(_mem) {
			delete [] _mem;
		}
	}

	value_type* create()
	{
		value_type* p = (value_type*)pop();
		if(p) {
			new(p) value_type(_init_v);
		}
		return p;
	}

	void destory(value_type* e)
	{
		e->~value_type();
		push(e);
	}

	size_t free_num() const
	{
		return _free_num;
	}

protected:
	void push(void *p)
	{
		*(void**)p = _head;
		_head = p;
		_free_num++;
	}

	void* pop()
	{
		void* p = _head;
		if(p) {
			_head = *(void**)p;
			_free_num--;
		}
		return p;
	}

	char* _mem;
	void* _head;
	size_t _free_num;
	const value_type _init_v;
};

}

#endif /* DPO_OBJ_FACTORY_H_ */
