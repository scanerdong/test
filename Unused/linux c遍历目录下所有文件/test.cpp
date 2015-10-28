/* 
    ���ܣ���ʾ����Linux������C���Եݹ����ָ��Ŀ¼�µ���Ŀ¼(��������Ŀ¼)���ļ� 
*/  
#include <stdio.h>  
#include <dirent.h>  
#include <string>  
#include <stdlib.h>
  
void List(char *path)  
{  
    struct dirent *ent = NULL;  
    DIR *pDir;  
    char achPath[512]={0};
    
    if((pDir = opendir(path)) != NULL)  
    {  
        while(NULL != (ent = readdir(pDir)))  
        {  
            snprintf(achPath, 512,"%s/%s", path, ent->d_name ); //��·��
            
            if(ent->d_type == 8)                 // d_type��8-�ļ���4-Ŀ¼  
                printf("File:\t%s\n", achPath);  
            else if(ent->d_type == 4 && ent->d_name[0] != '.')//������.��ͷ��Ŀ¼������ǰ���ϼ�������Ŀ¼
            {  
                printf("\n[Dir]:\t%s\n", achPath);  
                List(achPath);                   // �ݹ������Ŀ¼  
                printf("return [%s]\n\n", achPath);  
            }  
        }  
        closedir(pDir);  
    }  
    else
	{
        printf("Open Dir-[%s] failed.\n", path);  
        perror("open dir");
	}
}  

int main(int argc, char* argv[])
{
    
	
    List(argv[1]);  

    return 0;
}
