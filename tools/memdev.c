#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

uint8_t* map_mem(int fd, uint32_t addr, uint32_t size) {
  uint32_t mask = getpagesize() - 1;
  assert((size & mask) == 0);
  uint8_t *mem = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~mask);
  if(mem == MAP_FAILED) {
    fprintf(stderr, "Failed to map 0x%x: %s\n", addr, strerror(errno));
    exit(1);
  }
  return mem + (addr & mask);
}

int main(int argc, char** argv) {
  if(argc <= 1) {
    fprintf(stderr, "Usage: %s addr [count] [value]\n", argv[0]);
    exit(1);
  }

  uint32_t count = 4;

  char *end;
  char *arg = argv[1];
  uint32_t addr = strtol(arg, &end, 16);
  if(arg + strlen(arg) != end) {
    fprintf(stderr, "Failed to parse address: %s\n", arg);
    return 1;
  }

  if(argc > 2) {
    arg = argv[2];
    count = strtol(arg, &end, 10);
    if(arg + strlen(arg) != end) {
      fprintf(stderr, "Failed to parse width: %s\n", arg);
      return 1;
    }
  }

  int memfd = open("/dev/mem", O_RDWR | O_CLOEXEC);
  if(memfd < 0) {
    perror("open(/dev/mem): ");
    exit(1);
  }

  volatile uint32_t *base = (volatile uint32_t*) map_mem(memfd, addr, getpagesize());

  if(argc >= 3) {
    printf("not implemented yet!\n");
  } else {
    for(int i = 0; i < count / 4; i++) {
      uint32_t val = *base;
      write(1, &val, sizeof(base));
      base++;
    }
  }


  return 0;
}
