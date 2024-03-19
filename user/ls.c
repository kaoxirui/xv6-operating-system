#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  //字符指针里面存放的是常量字符串首字符的地址，可以通过地址找到字符串每个字符。
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  //目录文件（directory file）的概念：这种文件包含了其他文件的名字以及指向与这些文件有关的信息的指针
  struct dirent de;
  /*目录项，包含存储文件内容的inum号和文件名
  struct dirent {
  ushort inum;
  char name[DIRSIZ];
};
  */
  struct stat st;
/*存储文件基本信息
struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes
};

*/
  if((fd = open(path, 0)) < 0){//打开path所指向的文件，返回打开对应文件的文件描述符
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){//读取fd所指文件的信息
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  /*
    #define T_DIR     1   // Directory
    #define T_FILE    2   // File
    #define T_DEVICE  3   // Device
  */
  switch(st.type){// 判断文件类型，即判断参数path所指向的文件的类型
  case T_FILE:// 若path指向的是“文件”，则报错，因为ls的功能是输出指定“目录”下的所有目录项
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;
    //fmtname()输出文件本身的名字

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);//复制path到buf
    p = buf+strlen(buf);//指针p指向了buf中文件路径的末尾
    *p++ = '/';
    //可以读取fd中的dirent序列
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      /*
      fd是打开的目录，从目录中每次读取一个de，直到read读取失败为止，相当于遍历了目录项
      */
      if(de.inum == 0)
        continue;
        // 若de.inum为0，则选择跳过不打印。结合ls的使用场景，什么情况下的目录项不打印呢？
        /* 在lab9 file system中有关于de.inum的进一步使用，可以发现de.inum是指保存文件内容的inode号
        若inode号为0，意味着文件没有保存，是无效文件*/
      memmove(p, de.name, DIRSIZ);// 将从de.name开始的内容写DIRSIZ个长度到p指针所指位置
      p[DIRSIZ] = 0;// 在末尾写0，表示字符串的结束（在DIRSIZ处写0，相当于是对齐后输出了）
      if(stat(buf, &st) < 0){//若无法读取文件
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);//打印出文件信息
    }
    break;
  }
  close(fd);//关闭文件
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
