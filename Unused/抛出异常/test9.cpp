//ģ������÷�����������������������������������������������������������������������������
#include "test9.h"

using namespace std;

uint8 g_u8PrintCtrl = 0xff;

int main()
{
    int number;
    cout << "the int number is: ";

    while(cin>>number)
    {
        try
        {
            if((number % 2) == 0)
            {
                throw Base();
            }
            else
            {
                throw Child();
            }
        }

        //ע�⣺�����������catch����������ÿ�ε��쳣���󶼽������ಶ����Ϊ������Բ���������Ķ���
        catch(Child b)
        {
            b.show();
        }
        
        catch(Base b)
        {
            b.show();
        }
    }
 return 0;
}

