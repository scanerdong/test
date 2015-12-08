#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include "DFilter.h"
#include <boost/bind.hpp>
#include <vector>

using namespace std;

int transfer(char* pchSrcFileName, char* pchDstFileName)
{
    static const unsigned int MAX_BYTE_PER_LINE = 200+2;
    char buf[MAX_BYTE_PER_LINE];
    char* comment_begin = "/*";
    char* comment_end = "*/";
    bool bInCommentArea = false;
    char* start_pos;
    char* end_pos;
    char *str_start,*str_end;
    FILE* pfSrc = fopen(pchSrcFileName, "r");
    FILE* pfDst = fopen(pchDstFileName, "w");
    if(pfSrc==NULL || pfDst==NULL) return -1;

    while(fgets(buf+2, sizeof(buf)-2, pfSrc) != NULL){
        if(bInCommentArea){
            start_pos = strstr(buf+2,comment_end);
            if(start_pos != NULL){
                bInCommentArea = false;

                //如果endpos后面还有东西，则输出换行符，否则输出空格
                end_pos = start_pos;
                end_pos+=2;
                
                while(*end_pos==' ' || *end_pos=='\t' || *end_pos=='\n') ++end_pos;
                if(*end_pos=='\0'){
                    *start_pos = ' ';
                    *(start_pos+1) = ' ';
                }else{
                    *start_pos = ' ';
                    *(start_pos+1) = '\n';
                }
                
            }
            *buf = '/';
            *(buf+1) = '/';
            fputs(buf, pfDst);
            
        }else{
            str_start = strstr(buf+2,"\"");
            if(str_start != NULL){
                str_end = strstr(str_start+1,"\"");
                if(str_end == NULL){
                }
                    
            }
            start_pos = strstr(buf+2,comment_begin);
            if(start_pos != NULL){
                bInCommentArea = true;
                *start_pos = '/';
                *(start_pos+1) = '/';
                
                start_pos = strstr(start_pos+2,comment_end);
                if(start_pos != NULL){
                    bInCommentArea = false;

                    //如果endpos后面还有东西，则输出换行符，否则输出空格
                    end_pos = start_pos;
                    end_pos+=2;
                    
                    while(*end_pos==' ' || *end_pos=='\t' || *end_pos=='\n') ++end_pos;
                    if(*end_pos=='\0'){
                        *start_pos = ' ';
                        *(start_pos+1) = ' ';
                    }else{
                        *start_pos = ' ';
                        *(start_pos+1) = '\n';
                    }

                }
            }
            fputs(buf+2, pfDst);
        }
    }

    fclose(pfSrc);
    fclose(pfDst);
    return 0;
}



//函数模板  
template<class T>  
bool IsEqual(T t1,T t2){  
    return t1==t2;  
}  
  
template<> //函数模板特化  
bool IsEqual(char *t1,char *t2){  
    return strcmp(t1,t2)==0;  
}  
  
//类模板  
template<class T>  
class Compare{  
public:  
    virtual bool IsEqual(T t1,T t2){  
        return t1==t2;  
    }  
};  
  
//类模板的特化  
template<>  
class Compare<char*>{  
public:  
    virtual bool IsEqual(char *t1,char *t2){  
        return strcmp(t1,t2)==0;  
    }  
};  

template <class T1, class T2 >
class C
{

};

template <class T1>
class C<T1, double>
{

};

template < class T >
class A : public C<T,double>
{};

template < class T >
class A<std::vector<T> > : public C<T,double>
{};


template<int Base, int Exponent>
class XY
{
public:
enum { result_ = Base * XY<Base, Exponent-1>::result_ };
};
//用于终结递归的局部特化版
template<int Base>
class XY<Base, 0>
{
public:
enum { result_ = 1 };
};

class Integer
{
public:
    Integer(){cout << "Integer"<<endl;}
    ~Integer(){cout << "~Integer"<<endl;}
    int i;
    
};

#include <mysql/mysql.h>
int main(int argc, char* argv[])  
{
    if (mysql_library_init(0, NULL, NULL)) {
      fprintf(stderr, "could not initialize MySQL library\n");
      exit(1);
    }

    
    MYSQL my_connection;
    int res;
 
    mysql_init(&my_connection);
    if (mysql_real_connect(&my_connection, "127.0.0.1",
                "root", "123456", "dong", 0, NULL, 0)) {
        printf("Connection success character_set:%s\n", mysql_character_set_name(&my_connection));
        res = mysql_query(&my_connection, "insert into student(id,name,address) values(101,'heihei', 'shanghai')");
        if (!res) {
            printf("Inserted %lu rows\n",
                    (unsigned long)mysql_affected_rows(&my_connection));
        } else {
            fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }

        //查询所有id>20
        res = mysql_query(&my_connection, "select * from student where id>20");
        if (!res) {
            MYSQL_RES* result;
            MYSQL_ROW row;
            result = mysql_store_result(&my_connection);
            if(!result){
                fprintf(stderr, "get result error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
                mysql_close(&my_connection);
            }else{
                unsigned int num_fields = mysql_num_fields(result);
                
                while ((row = mysql_fetch_row(result)))
                {
                   unsigned long *lengths;
                   lengths = mysql_fetch_lengths(result);
                   for(unsigned int i = 0; i < num_fields; i++)
                   {
                       printf("len:%u value:[%s] ", lengths[i],
                              row[i] ? row[i] : "NULL");
                   }
                   printf("\n");
                }

                //free result set
                mysql_free_result(result);
            }
        } else {
            fprintf(stderr, "select error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
 
        mysql_close(&my_connection);
    } else {
        fprintf(stderr, "Connection failed\n");
        if (mysql_error(&my_connection)) {
            fprintf(stderr, "Connection error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
    }
 
    mysql_library_end();
    
    return 0;  
}  


