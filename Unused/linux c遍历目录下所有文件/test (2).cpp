/* 
    ���ܣ���ʾ����Linux������C���Եݹ����ָ��Ŀ¼�µ���Ŀ¼(��������Ŀ¼)���ļ� 
*/  
#include <stdio.h>  
#include <dirent.h>  
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

bool deleteFileOutOfDate(char* filePathAndName, time_t expireIntervalSec)//�ļ�·����expireIntervalSec:�������������Ϊ����
{
    struct stat buf;
    int ret;
    time_t curr;
    
    ret = stat(filePathAndName, &buf);
    if(ret != 0)
    {
        perror("stat out of date file:");
        return false;
    }    

    curr = time(NULL);
    if(S_ISREG(buf.st_mode))//��һ���ļ�
    {
         if(curr - buf.st_mtime > expireIntervalSec)
         {
            printf("remove out of date file %s \n", filePathAndName);
            remove(filePathAndName);
         }
    }

    return true;
}

void searchOutOfDatePath(char *path)  
{  
    struct dirent *ent = NULL;  
    DIR *pDir;  
    char achPath[512]={0};
    unsigned int fileNumOfPath = 0;//��Ŀ¼�µ��ļ���
    
    if((pDir = opendir(path)) != NULL)  
    {  
        while(NULL != (ent = readdir(pDir)))  
        {  
            snprintf(achPath, 512,"%s/%s", path, ent->d_name ); //��·��
            fileNumOfPath++;
            
            if(ent->d_type == 8)// d_type��8-�ļ���4-Ŀ¼ 
            {                  
                printf("File:\t%s\n", achPath); 
                //����ļ��Ƿ����
                deleteFileOutOfDate(achPath, 120);
            } 
            else if(ent->d_type == 4 && ent->d_name[0] != '.')//������.��ͷ��Ŀ¼������ǰ���ϼ�������Ŀ¼
            {  
                printf("\n[Dir]:\t%s\n", achPath);  
                searchOutOfDatePath(achPath);                   // �ݹ������Ŀ¼  
                printf("return [%s]\n\n", achPath);  
            }  
        }  

        closedir(pDir);  

        if(fileNumOfPath == 2)//���ڵĿ�Ŀ¼��ɾ���Ŀ¼��2��ʾĿ¼��ֻ��.��..
        {
            achPath[strlen(achPath)-2] = '\0';
            printf("delete empty directory %s\n", achPath);
            if(0 != remove(achPath))
            {
                perror("delete empty directory");
            }
        }
    }  
    else
	{
        printf("Open Dir-[%s] failed.\n", path);  
        perror("open dir");
	}
}  

#include <memory.h>
int main(int argc, char* argv[])
{   	
    //searchOutOfDatePath(argv[1]);
	printf("haha");
	char ach[10];
	char* pch = "dfssdf";
	memcpy(ach, pch, strlen(pch)+1);
	printf(ach);
    return 0;
}
