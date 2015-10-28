
#include<map>
#include<string>
#include<iostream>
using namespace std;
int main()
{
    map<string,int>  m;
    m["a"]=1;
    m["c"]=3;
    
    m["b"]=2;
    m["z"]=14;
    m["x"]=7;
    map<string,int>::iterator it;

    //输出是字母顺序abcxz，因为map是红黑树实现的，所以元素是有序的
    for(it=m.begin();it!=m.end();++it)
        cout<<"key: "<<it->first <<" value: "<<it->second<<endl;
    return 0;
}
