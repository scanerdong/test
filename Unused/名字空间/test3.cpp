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
        using namespace savitch2;//使用savictch2、std、全局三个命名空间
        greeting();
    }
*/
    savitch2::greeting();//提示：使用上面的注释部分也是一样的效果


    {
        using namespace savitch1; //使用savitch1、std、全局三个命名空间
        greeting();  //如果将g_greeting与此同名，则会在此处产生冲突，因为在此处savitch1和std两个名字空间都是有效的。
    }

    g_greeting(); //使用了std一个标准命名空间
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

