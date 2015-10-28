/* 
    功能：演示了在Linux下利用C语言递归遍历指定目录下的子目录(不含隐藏目录)和文件 
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
            snprintf(achPath, 512,"%s/%s", path, ent->d_name ); //总路径
            
            if(ent->d_type == 8)                 // d_type：8-文件，4-目录  
                printf("File:\t%s\n", achPath);  
            else if(ent->d_type == 4 && ent->d_name[0] != '.')//忽略以.开头的目录，即当前、上级、隐藏目录
            {  
                printf("\n[Dir]:\t%s\n", achPath);  
                List(achPath);                   // 递归遍历子目录  
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
