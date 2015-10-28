#include "Common.h"
#include "Singleton.h"
#include "Test.h"
#include <sys/time.h>
#include <unistd.h>

uint8 g_u8PrintCtrl = 0xff;
/*

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

//分离字符串的源码
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

std::string& replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value)
{
	for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length())
	{
		if((pos=str.find(old_value,pos))!=string::npos)
		{
			str.replace(pos,old_value.length(),new_value);
		}else{
			break;
		}
	}     
	return	str;
}
void ltrim(std::string &str,std::string trimStr)
{
    str.erase(0, str.find_first_not_of(trimStr));
}

void rtrim(std::string &str,std::string trimStr)
{
    str.erase(str.find_last_not_of(trimStr)+1, string::npos);
}

void trim(std::string& str,std::string trimStr = "")
{
    ltrim(str,trimStr);
    rtrim(str,trimStr);
}

void ConvertStrToArray(std::string &strValue, _strVector &aVector)
{
    int32 i32Value = 0;
    int32 iPos=0;
    int32 i32Len;
    char aChar='0';

    trim(strValue);
    i32Len = strValue.length();
    aChar = strValue[i32Len - 1];
    if(aChar != ',') {
        strValue = strValue + ",";
    }

    iPos = strValue.find_first_of(",");
    std::string subString;

    aVector.clear();
    while(iPos != std::string::npos){
        subString = strValue.substr(0,iPos);
        trim(subString);
        aVector.push_back(subString);
        strValue.erase(0,iPos+1);
        iPos = strValue.find_first_of(",");
    }
}
void IsNan(float64 f64Test)
{
    if(f64Test == f64Test)
        cout << "is" << endl;
    else
        cout << "no" << endl;
}

#define FIFO_TYPE_RX	2
typedef struct
{
    char a;
    char b;
    char c;
    char d;
}_stTest;

FILE* pFile = NULL;

void OpenSyncFile(FILE** ppSyncFile)
{
	*ppSyncFile= fopen("./sync_data.txt","w");
    
	if(*ppSyncFile == NULL)
	{
		printf("Open File[sync_data.txt] error\n");
	}
}

void CloseSyncFile(FILE* pSyncFile)
{
	if(pSyncFile == NULL) return;
    
    fclose(pSyncFile);
    pSyncFile = NULL;
}

void WriteSyncInfo(FILE* pSyncFile, uint32 u32Value)
{
	char	achBuf[128] = {0};
    int32 i32Ret;
    printf("1111111\n");
    if(pSyncFile == NULL) return;	
    printf("22222222\n");

	snprintf(achBuf, sizeof(achBuf), "%d\n", u32Value);
    fwrite(achBuf, strlen(achBuf), 1, pSyncFile);
    fwrite(achBuf, strlen(achBuf), 1, stdout);
    //fputs(achBuf,pSyncFile);
    //fputs(achBuf,stdout);
}
int main()
{
    printf("%lf\n",atof("   -2.53d\n"));

    time_t u32TimeStamp;
    tm* pstTime;
    timeval stTimeVal;
    //timezone stTimeZone;
    int32 i32Ret;
    
    time(&u32TimeStamp);
    pstTime = gmtime(&u32TimeStamp);

    printf("local time:%d/%d/%d %d:%d:%d\n", pstTime->tm_year + 1900, pstTime->tm_mon + 1, pstTime->tm_mday,
        pstTime->tm_hour + 8, pstTime->tm_min, pstTime->tm_sec);

    stTimeVal.tv_sec = u32TimeStamp + 8*3600;
    stTimeVal.tv_usec = 0;
    //stTimeZone.tz_minuteswest = 8*60;
    //stTimeZone.tz_dsttime = DST_NONE;
    //i32Ret = settimeofday(&stTimeVal, NULL);
    printf("%d\n", i32Ret);
    perror("fail:");
    printf("----------------------------\n");
    if(memcmp("haah","haha",4) == 0)
    {
        printf("same\n");
    }
    else
    {printf("not same\n");}

    
}
#include<string.h> 
int main()
{ 
    
    char s[]="ab-cd : ef;gh :i-jkl;mnop;qrs-tu: vwx-y;z";
    char *delim = "-:";
    char *p;
    double f64Num;
    char* pchS = "s5456.95";
    
    printf("%s ", strtok(s,delim));
    
    while((p=strtok(NULL,delim)))
        printf("%s  ",p);
    
    printf("\n"); 

    f64Num = strtod(s, &p);
    
    printf("the num is:%f, fail str:%s\n", f64Num, p);

printf("%#x\n", 0xffffffff);
    return 0;
}

#include<stdlib.h> 
void my_exit(void) 
{ 
 printf("before exit () !\n");

} 
int main() 
{
    uint32 u32CfgValue = 1815375;
    
    printf("u32CfgValue:%u\n", u32CfgValue);
    if(u32CfgValue%125 == 0)
        {
            printf("same\n");
        }
        else
        {printf("not same\n");}
}

#include<unistd.h>
int main()
{
 struct timeval tv;
 struct timezone tz;
 gettimeofday (&tv , &tz); 
 printf("tv_sec; %d\n", tv.tv_sec);
 printf("tv_usec; %d\n",tv.tv_usec);

 printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
 
 printf("tz_dsttime, %d\n",tz.tz_dsttime);
 
}
#include<unistd.h>
int main()
{
 struct timeval tv;
 struct timezone tz;
 gettimeofday (&tv , &tz); 
 printf("tv_sec; %d\n", tv.tv_sec);
 printf("tv_usec; %d\n",tv.tv_usec);

 printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
 
 printf("tz_dsttime, %d\n",tz.tz_dsttime);
 
}
*/
     
#include<time.h> 
#include<stdlib.h> 

static int32 s_i32IfFpgaDev   =   -1;      // IF_FPGA的文件标识符

static int32 fnDevReopenDevFile(const char *pchFileName)
{
    int32 i32FileId = 0;

    if (pchFileName == NULL)
        return FAIL;

    i32FileId = open(pchFileName, O_RDWR);

    if (-1 == i32FileId)
    {
        OM_ERROR("Open dev \"%s\" failure!\n",pchFileName);
    }

    return i32FileId;
}

int32 fnDevIfFpgaRead(uint16 u16Addr,uint16 *pu16Value)
{
    int32 i32Temp = 0;   /* 用于保存IOCTL函数的返回值 */

    if (pu16Value == NULL)
        return DEV_INVALID_PARAM;

    /* 第一次对设备描述符进行判断，如果设备没有打开或上次打开失败 */
    /* 则重新打开一次 */
    if (s_i32IfFpgaDev < 0)
    {
        s_i32IfFpgaDev = fnDevReopenDevFile("/dev/nts_fpga");
        //s_i32IfFpgaDev = open("/dev/nts_fpga", O_RDWR);
    }

    /* 如果重新打开还是失败，则退出 */
    if (s_i32IfFpgaDev < 0)
    {
        return DEV_FILE_OPEN_FAILED;
    }

    i32Temp = ioctl(s_i32IfFpgaDev, IOC_FPGA_READ_WORD, u16Addr);

    printf("i32Temp:%d, s_i32IfFpgaDev:%d\n",i32Temp, s_i32IfFpgaDev);

    if (0 > i32Temp)
    {
        return DEV_OP_FAIL;
    }

    *pu16Value = i32Temp;

    return DEV_SUCCEED;
}

int main()
{
    int i,j;
    uint32 tag;
    
    char achCmd[100]={0};
    char achBuf[100]={0};
    uint16 u16Addr, u16Value;
    int32 i32Ret;
    
    srand((int)time(0));
    
    while(1)
    {
        //
        j=1+(int)(50.0*rand()/(RAND_MAX+1.0));
        /*
        if((++tag)%2 == 0)
        {
            j *= -1;
        }
        */
        printf("%d\n", j);
        
        /*
        sprintf(achCmd, "test_fpga nr 111");
        system(achCmd);
        for(i = 0; i<10; i++)
        {
            fgets(achBuf, sizeof(achBuf), stdout);

            printf("get line buf %s\n", achBuf);

            if(!memcmp(achBuf, "0x0111", 6))
            {
                break;
            }
        }
        sscanf(achBuf, "%x : %x", &u16Addr, &u16Value);
        */

        //
        i32Ret = fnDevIfFpgaRead(0x111, &u16Value);
        if(i32Ret != DEV_SUCCEED)
        {
            printf("IF fpga read error, error no:%d\n", i32Ret);
        }
        else
        {
            sprintf(achCmd, "test_fpga nw 111 %x", u16Value + j);
            system(achCmd);
        }

        //再写高位
        i32Ret = fnDevIfFpgaRead(0x110, &u16Value);
        if(i32Ret != DEV_SUCCEED)
        {
            printf("IF fpga read error, error no:%d\n", i32Ret);
        }
        else
        {
            sprintf(achCmd, "test_fpga nw 110 %x", u16Value);
            system(achCmd);
        }
        usleep(500*1000);

    }

    return 0;
}


