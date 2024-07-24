#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
  // getcmd()trace的参数有误
  if (argc < 3 || argv[1][0] < '0' || argv[1][0] > '9') {
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // trace()系统调用失败
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  // trace mask xxx: xxx是待trace的cmd
  int i;
  char* nargv[MAXARG];
  for (i = 2; i < argc && i < MAXARG; i++) {
    nargv[i-2] = argv[i];
  }
  exec(nargv[0], nargv);
  exit(0);
}
