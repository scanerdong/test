#ifndef __DL_DPIDEF__
#define __DL_DPIDEF__

#ifdef __cplusplus
#define DLDPI_API extern "C"
#else
#define DLDPI_API
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <errno.h>

#ifndef WIN32
#include <sys/wait.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <string.h>
#else
#include <windows.h>
#include <map>
#endif

using namespace std;
typedef unsigned short ushort;

class Base
{
public:
virtual void p();
virtual  void set(int n);
private:

int m_n;
};


/**
 * CCreateDpiObj define
 * 目的：加载动态库，创建对象，销毁对象。
 * 可适用于所有提供DL_CreateObject和DL_FreeObject动态库的使用
 */
class CCreateLibObj
{
public:
	CCreateLibObj(string dpilibname)
	{
		m_handle=0;
#ifndef WIN32	
		m_handle = dlopen(dpilibname.data(), RTLD_NOW);
#else
		m_handle=LoadLibrary(dpilibname.data());
#endif
		if (!m_handle) {
			printf("Load dpi lib:%s failed!\r\n",dpilibname.c_str());
			exit(0);
		}

	}

	~CCreateLibObj()
	{
		if(m_handle)
#ifndef WIN32
			dlclose(m_handle);
#else
			FreeLibrary(m_handle);	
#endif
		m_handle=0;
	};

	void* test_create()
	{
		if(m_handle==0)
			return NULL;
		
		void* (*test)(void);
		

#ifndef WIN32
		char *error;
		test = (void* (*)(void))dlsym(m_handle, "create_obj");
		if(	(error = dlerror()) != NULL) {
			printf("dlsym error:%s\r\n",error);
			exit(0);
		}
#else
		test=(void* (*)(void))GetProcAddress(m_handle,"create_obj");   
#endif
		return (*test)();
		
	};	

	
	void test_free(void* obj)
		{
			if(m_handle==0)
				return ;
			
			void (*free)(void* );
			
				char *error;
			free = (void (*)(void* ))dlsym(m_handle, "free_obj");
			if( (error = dlerror()) != NULL) {
				printf("dlsym error:%s\r\n",error);
				exit(0);
			}

			
			free(obj);
		};	
private:
#ifndef WIN32
	void *m_handle;
#else
	HINSTANCE m_handle;
#endif
};

#endif//__DL_DPIDEF__


