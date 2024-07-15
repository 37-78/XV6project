#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/* 把user/grep.c的match抄过来：替换98行 */
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

// 从path中分割filename
char* splitFilename(char* path) {
  char* p;
  for (p=path+strlen(path); p>=path && *p!='/'; p--) { }
  p++;
  return p;
}

void find(char* path, char* file) {
 char buf[512], *p;
 int fd;
 struct dirent de;
 struct stat st;

 // path有误
 if ((fd = open(path, 0)) < 0) {
    fprintf(2, "Error: cannot open %s\n", path);
    return;
 }
 if (fstat(fd, &st) < 0) {
    fprintf(2, "Error: cannot stat %s\n", path);
    close(fd);
    return; 
 }

 // 判断path类型
 switch(st.type) {
  // 文件：这条路到底
  case T_FILE:
    ;
    char* name = splitFilename(path);
    if (name!=0 && match(file, name) == 1) {
      fprintf(1, "%s\n", path);
    }
    break;
  // 目录：继续向下搜索
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      fprintf(1, "Usage: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    // 该目录下的所有条目de(s)
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) //特殊de
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){ //buf里的path有误
        fprintf(2, "Error: cannot stat %s\n", buf);
        continue;
      }
      // 除了'.'和'..'里面向下查找
      if (strcmp(de.name, ".")!=0 && strcmp(de.name, "..")!=0) {
        find(buf, file);
      }
    }
    break;
  }
  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find [path] [filename]\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
