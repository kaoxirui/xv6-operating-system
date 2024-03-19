#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path){
    static char buf[DIRSIZ+1];
    char *p;
    
    for(p=path+strlen(path);p>=path&&*p!='/';p--)
    ;
    p++;

    if(strlen(p)>=DIRSIZ)
        return p;
    memmove(buf,p,strlen(p));
    memset(buf+strlen(p),' ',DIRSIZ-strlen(p));
    return buf;
}

void find(char *path,char *target){
    int fd;
    char buf[512],*p;
    struct dirent de;
    struct stat st;
    if((fd=open(path,0))<0){
        fprintf(2,"find: cannot open %s\n",path);
        return;
    }
    if(fstat(fd,&st)<0){
        fprintf(2,"find: cannot stat %s\n",path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE:
            {
            int match=1;
            if(strcmp(fmtname(path),target)!=0){
            match=0;
            }
            if(match){
            printf("%s\n",path);
            }
            close(fd);
            break;
            }
        case T_DIR:
            if(strlen(path)+1+DIRSIZ+1>sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf,path);
            p=buf+strlen(buf);
            *p++='/';
            while(read(fd,&de,sizeof(de))==sizeof(de)){
                if(de.inum==0)
                    continue;
                memmove(p,de.name,DIRSIZ);
                p[DIRSIZ]=0;
                if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
                    continue;
                find(buf,target);
            }
            break;
    }
    close(fd);
}

int main(int argc,char* argv[])
{
    if(argc<2){
        printf("at least two argument!");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}