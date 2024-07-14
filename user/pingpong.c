#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  // 创建两个管道 
  int p1[2]; int p2[2];
  pipe(p1); pipe(p2);

  char buf[] = {'p'};
  int pid = fork();
  if (pid == 0) {
    // 子进程
    close(p1[0]);
    close(p2[1]);
      if (read(p2[0], buf, 1) == 1) {
        fprintf(1, "%d: received ping\n", getpid());
      }
      write(p1[1], buf, 1);
    close(p1[1]);
    close(p2[0]);
    exit(0);
  }
  else {
    // 父进程
    close(p2[0]);
    close(p1[1]);
      write(p2[1], buf, 1);
      if (read(p1[0], buf, 1) == 1) {
        fprintf(1, "%d: received pong\n", getpid());
      }
    close(p2[1]);
    close(p1[0]);
    exit(0);
  }
}
