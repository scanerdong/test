//����C++���麯�����ڴ��еķֲ�
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include <vector>
#include <iterator>
#include "Singleton.h"
#include "msgproc.h"
#include "ProcThread.h"

using namespace std;


//template<typename T>
void f(int32 i32Param1, int32 i32Param2)
{
cout << "the compare number is "<< i32Param1 << i32Param2 << endl;
}

template<typename T, class F>
const T& max2(const T& i32Param1, const T& i32Param2, F fn)
{
    fn(i32Param1, i32Param2);
    return (i32Param1 > i32Param2)?i32Param1:i32Param2;
}

typedef void (*_pfnHander)(int32 i32Param1, int32 i32Param2);

#if 0
template<class T, class F>
  F for_each2(T first, T last, F f)
  {
      return (i32Param1 > i32Param2)?i32Param1:i32Param2;
  }
#endif

int main()
{
    vector<int> vecTest;
    vector<int>::iterator vecIte1, vecIte2;
    for(int32 i32I = 0; i32I < 10; i32I++)
    {
        vecTest.push_back(i32I);
    }
    vecIte1 = vecTest.begin();
    vecIte2 = vecTest.end();

    cout << "The distance is" << distance(vecIte1, vecIte2) << endl;

    vecIte1 = find(vecIte1, vecIte2, 7);
    cout << &(*vecIte1) << endl;
    vecTest.insert(vecIte1, 45);

    int32 i32Temp;
    vecTest.front() = 342;
    
    cout << *vecTest.begin() << endl << "vector size is " << vecTest.size() <<endl<<  vecTest.capacity() << endl;
    vecTest.reserve(30);
    cout << vecTest.capacity() << endl;

    //------------������ָ���Ӧ�ò���-------------------------
    int32 i32Param1 = 8, i32Param2 = 15;

    //Ϊ�˵��ô˺������������������͵�ָ��
    _pfnHander pfCoutHander;
    
    pfCoutHander = f;
    pfCoutHander(34,6);//�ɹ��ĵ��õ��˺���

    //���������ͱ��ģ���е�һ������������
    cout << max2(i32Param1, i32Param2, f) << endl;

    //����һ����ָ��ȥָ��һ����������ǿ��ת����ָ��
    void* pfn;
    pfn = (void*)f;

    ((void (*)(int32, int32))pfn)(345,643);
	return 0;
	
}

#if 0
// vector::push_back
using namespace std;

int main ()
{
  vector<int> myvector;
  int myint;

  cout << "Please enter some integers (enter 0 to end):\n";

  do {
    cin >> myint;
    myvector.push_back (myint);
  } while (myint);

  cout << "myvector stores " << (int) myvector.size() << " numbers.\n";

  return 0;
}
#endif
