#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int p1[2],p2[2];
    pipe(p1);
    pipe(p2);
    char buf[5];
    int pid=fork();
    int size;
    int status;
    if(pid==0){
        //子进程
        //读取父进程信息
        close(p1[1]);//关闭写端
        if((size=read(p1[0],buf,sizeof buf))>0){
            printf("%d: received ",getpid());
            write(1,buf,size);
        }else{
            printf("%d: fail to receive",getpid());
        }
        //向父进程写数据
        close(p2[0]);//关闭读端0,从1写端写入
        write(p2[1],"pong\n",5);
        exit(0);
    }
    else if(pid>0){
        //父进程
        close(p1[0]);//关闭读端0,从1写端写入
        write(p1[1],"ping\n",5);
        wait(&status);
        close(p2[1]);
        if((size=read(p2[0],buf,sizeof buf))>0){
            printf("%d: received ",getpid());
            write(1,buf,size);
        }else{
            printf("%d: fail to received",getpid());
        }
        exit(0);
    }else{
        printf("error");
        exit(1);
    }
    exit(0);
};