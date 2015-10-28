#include "Common.h"
#include <iostream>
#include <list>
#include "Singleton.h"
using namespace std;

static void PrintBuf()
{
    cout << "in PrintBuf" << endl;
}
class CMainFrameBase
{
    //DECLARE_DEFAULT_COM_FUN;
    PUBLIC:
    virtual _enResult Init();
    
    CMainFrameBase() {cout << "CMainFrameBase Construct " << endl; };
    virtual ~CMainFrameBase() {cout << "CMainFrameBase Destruct " << endl; };
PROTECTED:    

    virtual _enResult InitSoft();
    //virtual _enResult InitHardware();    
    
    //virtual _enResult TerminateSoftware();
    
    //virtual _enResult TerminateHardware();    
};

_enResult CMainFrameBase::Init()
{
    cout << "CMainFrameBase Init " << endl; 
    InitSoft();
    PrintBuf();
}

_enResult CMainFrameBase::InitSoft()
{
    cout << "CMainFrameBase InitSoft " << endl; 
}

class CFrameBase
{
PUBLIC:    
    virtual ~CFrameBase(){}
    
PROTECTED:
    CFrameBase(){}
    
    std::list<CMainFrameBase*> m_pclMainFrameBaseList;

};


class CMainFrameProject : PUBLIC CMainFrameBase
{
PUBLIC:       
    virtual _enResult InitSoft();
    
    CMainFrameProject() {cout << "CMainFrameProject Construct " << endl; };
    ~CMainFrameProject() {cout << "CMainFrameProject Destruct " << endl; };
    //_enResult Init();

    std::list<CMainFrameBase*> m_pclMainFrameBaseList;

    virtual _enResult InitHardware();

    
    //virtual _enResult TerminateHardware();    
};
_enResult CMainFrameProject::InitHardware()
{
    cout << "CMainFrameProject InitHardware" << endl;
}
_enResult CMainFrameProject::InitSoft()
{
    cout << "CMainFrameProject InitSoft " << __FUNCTION__ << endl; 
}
/*
_enResult CMainFrameProject::Init()
{
	cout << "CMainFrameProject Init " << endl; 
}
*/
#if 0
typedef struct in_addr 　　
{ 　　
    union
    { 　　
        struct 　　
        { 　　
            unsigned char s_b1,s_b2,s_b3,s_b4; 　　
        } S_un_b; 　　

        struct 　　
        { 　　
            unsigned short s_w1,s_w2; 　　
        } S_un_w; 　　

        unsigned long S_addr; 　　
        
    } S_un; 　　
} IN_ADDR; 

//sockaddr_in（在netinet/in.h中定义）： 　　
struct sockaddr_in 
{ 　　
    short int sin_family; /* Address family 代表协议族：AF_INET  */ 　　
    unsigned short int sin_port; /* Port number 端口号 （使用网络字节顺序） */ 　　
    struct in_addr sin_addr; /* Internet address   网络字节顺序存储IP地址 使用in_addr这个数据结构 */ 　　
    unsigned char sin_zero[8]; /* Same size as struct sockaddr  保留的空字节。 */ 　　
}; 　　
#endif



class GamePlayer 
{
public:
    GamePlayer(int* scores);
    int* GetScores();
private:
    static const int NUM_TURNS = 5; // constant eclaration 
    int m_scores[NUM_TURNS];      // use of constant
    
    int* p;
};
GamePlayer::GamePlayer(int* scores):p(scores)
{
    
}
int* GamePlayer::GetScores()
{
    return (int* )m_scores;
}


int main ()
{
  size_t sz;
  string str ("I like to code in C");
  cout << str << endl;

  /*sz=str.size();

  str.reserve (50);
  cout << str << endl;

  cout << str.size() << endl;
  cout << str.max_size() << endl;
  cout << str.capacity() << endl;

  str.clear();
  
  cout << str.size() << endl;
  cout << str.max_size() << endl;
  cout << str.capacity() << endl;
  
  
  CMainFrameBase* pclMainFrameBase;
  
  cout << endl;
  pclMainFrameBase = TSingleton<CMainFrameBase>::Instance();
  pclMainFrameBase->Init();
  
  delete TSingleton<CMainFrameBase>::Instance();
  
  cout << endl;
  pclMainFrameBase = TSingleton<CMainFrameProject>::Instance();
  pclMainFrameBase->Init();
  cout << endl;
  
  cout << endl;
  
  pclMainFrameBase = TSingleton<CMainFrameProject>::Instance();
  pclMainFrameBase->Init();
  
  cout << endl;
  
  delete TSingleton<CMainFrameProject>::Instance();
    */
  int scores[5] = {1,2,3,4,5};
  GamePlayer GamePlayer1((int* )scores);
  
  GamePlayer1.GetScores();

  
  return 0;
}















//虚析构函数是为了解决这样的一个问题：基类的指针指向派生类对象，并用基类的指针删除派生类对象。
















