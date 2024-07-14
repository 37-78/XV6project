#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 执行每行命令
void run(char* op, char** args) {
  if (fork() == 0) {
    exec(op, args);
    exit(0);
  }
  return;
}

int main(int argc, char* argv[]) {
  // 缓冲区：放所有额外参数
  char buf[2048];
  char* p = buf; char* last_p = buf;
  
  // 每次run的args的缓冲区
  char* argsbuf[128];
  char** args = argsbuf;

  // 先把xargs echo hello放进argsbuf
  for (int i=1; i<argc; i++) {
    *args = argv[i];
    args++;
  }

  // 从stdin逐字符读入buf
  char** pa = args;
  while (read(0, p, sizeof(char)) != 0) {
    if (*p==' ' || *p=='\n') {
      // 空格:'\0'标识一个参数结束，放入argsbuf的一行
      *p = '\0';
      *pa = last_p;
      pa++;
      last_p = p + 1;

      // 还换行：去执行完整一行命令
      if (*p=='\n') {
	*pa = '\0'; //argsbuf下一待赋值行='\0'：标识参数列表结束
        run(argv[1], argsbuf);
	pa = args; //返回args：覆盖下一参数列表
      }
    }
    p++;
  }

  // 最后一行末尾没有'\n'
  if (pa != args) {
    *p = '\0';
    *pa = last_p;
    pa++;
    *pa = '\0';
    run(argv[1], argsbuf);
  }
  
  // 等所有run的子进程都结束
  while (wait(0) != -1) {};
  exit(0);
}
