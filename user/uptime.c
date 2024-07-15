#include "kernel/types.h"
#include "user/user.h"

int main() {
  fprintf(1, "uptime: %d\n", uptime());
  exit(0);
}
