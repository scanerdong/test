#include <stdio.h>
#include <iostream>

using namespace std;

namespace savitch1
{
    void greeting();
}

namespace savitch2
{
    void greeting();
}

void g_greeting();

int main()
{
/*
    {
        using namespace savitch2;//ʹ��savictch2��std��ȫ�����������ռ�
        greeting();
    }
*/
    savitch2::greeting();//��ʾ��ʹ�������ע�Ͳ���Ҳ��һ����Ч��


    {
        using namespace savitch1; //ʹ��savitch1��std��ȫ�����������ռ�
        greeting();  //�����g_greeting���ͬ��������ڴ˴�������ͻ����Ϊ�ڴ˴�savitch1��std�������ֿռ䶼����Ч�ġ�
    }

    g_greeting(); //ʹ����stdһ����׼�����ռ�
    return 0;
}

namespace savitch1
{
    void greeting()
    {
        cout << "Hello from namespace savitch1.\n";
    }
}

namespace savitch2
{
    void greeting()
    {
        cout << "Greetings from namespace savitch2.\n";
    }
}

void g_greeting()
{
    cout << "A Big Global Hello!\n";
}

