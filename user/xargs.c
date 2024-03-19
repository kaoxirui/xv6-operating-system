#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"


int main(int argc,char* argv[]){
    char stdIn[512];
    int line=0;
    int size=read(0,stdIn,sizeof stdIn);
    char* argument[MAXARG];

    for(int i=0;i<size;i++){
        if(stdIn[i]=='\n')
            line++;
    }
    char buf[line][64];

    int k=0;
    for(int i=0,j=0;i<size;i++){
        buf[j][k]=stdIn[i];
        k++;
        if(stdIn[i]=='\n'){
            buf[j][k-1]='\0';//因为上面k++了
            k=0;
            j++;
        }
    }

    int m;
    for(m=0;m<argc-1;m++){
        argument[m]=argv[m+1];
    }
    int l=0;
    while(l<line){
        argument[m]=buf[l++];//把buf某一行的首地址给了数组指针，第 m 个argument为'\0'，将
        if(fork()==0){
            exec(argument[0],argument);
            exit(0);
        }
        wait(0);   
    }
    exit(0);
}