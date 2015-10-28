//模板类的用法。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
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

        //注意：如果这里两个catch语句调换，则每次的异常对象都将被基类捕获，因为基类可以捕获派生类的对象
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

