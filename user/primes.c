#include "kernel/types.h"
#include "user/user.h"

// 递归创建子管道，从父管道传递数据
void child(int* p) {
  close(p[1]);
  int n;

  // 父管道已无数据
  if (read(p[0], &n, sizeof(n)) == 0) {
    close(p[0]);
    exit(0);
  }
  
  // 打印素数
  fprintf(1, "prime %d\n", n);
  
  // 创建子管道、子进程
  int cp[2];
  pipe(cp);
  if (fork() == 0) {
    child(cp);
  }
  else {
    close(cp[0]);
    int cn;
    // 用前面输出的n检测cn
    while (read(p[0], &cn, sizeof(cn)) != 0) {
      if (cn % n != 0) {
        write(cp[1], &cn, sizeof(cn));
      }
    }
    close(cp[1]);
    wait(0);
  }

  close(p[0]);
  exit(0);
}

int main(int argc, char* argv[]) {
  // 创建管道
  int p[2];
  pipe(p);

  // 创建子进程
  if (fork() == 0) {
    child(p); //int*: p = &p[0]
  }
  else {
    close(p[0]);
    int i;
    // 父管道写入初始2～35
    for (i=2; i<=35; i++) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
