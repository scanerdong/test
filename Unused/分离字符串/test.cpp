#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <list>
#include "Singleton.h"
#include "Test.h"
#include <vector>

uint8 g_u8PrintCtrl = 0xff;

typedef std::vector<string> _strVector;

class GpsMsgMgr
{
    public:
        GpsMsgMgr();
        void SplitMsg(const string& strSrc);
        void Dump();
    private:
        _strVector m_strVector;
};

GpsMsgMgr::GpsMsgMgr()
{
    
}

//·ÖÀë×Ö·û´®µÄÔ´Âë
void GpsMsgMgr::SplitMsg(const string& strSrc)
{
    size_t szPosBegin = 0;
    size_t szPosEnd =0;

    while(szPosEnd != string::npos)
    {
        szPosEnd = strSrc.find(",", szPosBegin);
        m_strVector.push_back(strSrc.substr(szPosBegin, szPosEnd - szPosBegin));
        cout << "the pos end is " << szPosEnd << "pos begin is " << szPosBegin << endl;
        szPosBegin = szPosEnd + 1;

    }

}

void GpsMsgMgr::Dump()
{
    _strVector::iterator strVecIte;

    for(strVecIte = m_strVector.begin(); strVecIte != m_strVector.end(); strVecIte++)
    {
        if(*strVecIte == "")
            cout << "blank" << endl;
        else
        {
            cout << *strVecIte << endl;
        }
   }
}
int main()
{
    GpsMsgMgr clGpsMsgMgr;
    string strA = "$GPIS,,,23443.54,1.6,3,5,,,haha,";
    string strB = "$PLL,fdadf,,23443.54,1.6,3,5,fdsf,,fdsf,fdsf,testb,";
    string strC = "$GPIS,,,23443.54,1.6,3,5,,,,,*67";
    string strD = "$GPIS,,,23443.54,1.6,3,5,,,,,*67";
    string strE = "$GPIS,,,23443.54,1.6,3,5,,,,,*67";

    clGpsMsgMgr.SplitMsg(strA);    clGpsMsgMgr.SplitMsg(strB);

    clGpsMsgMgr.Dump();
    
}

