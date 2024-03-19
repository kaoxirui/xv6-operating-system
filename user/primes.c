#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void traversal(int fd){
    int first;
    if(read(fd,&first,4)!=0){//
        printf("primes %d\n",first);
    }else{
        close(fd);
        exit(0);
    }
    int num;
    int pipe2[2];
    pipe(pipe2);
    //close(pipe2[0]);
    while(read(fd,&num,4)!=0){
        if(num%first!=0){
            write(pipe2[1],&num,4);
        }
    }
    close(pipe2[1]);
    if(fork()==0){
        traversal(pipe2[0]);
    }else{
        wait(0);
    }
    exit(0);
}


int main(){
    int p[2];
    pipe(p);
    for(int i=2;i<=35;i++){
        write(p[1],&i,sizeof(int));
    }
    //wreit()的第二个参数是指向要写入数据的缓冲区的指针
    close(p[1]);
    traversal(p[0]);
    exit(0);        
}