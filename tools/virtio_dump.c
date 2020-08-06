#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdint.h>
#include <inttypes.h>

#define PAGE_SIZE 4095

struct  vring_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
};

struct vring_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[];
};

struct vring_used_elem
{
  uint32_t id;
  uint32_t len;
};

struct vring_used
{
  uint16_t flags;
  uint16_t idx;
  struct vring_used_elem ring[];
};

void dump(int fd, uint64_t begin_addr, uint16_t num_buffers, int dump_content) {
  uint8_t *shm = mmap(0, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, begin_addr);
  if(shm == MAP_FAILED) {
    fprintf(stderr, "failed to map: %lx\n", begin_addr);
    return;
  }

  uint32_t align = 16;
  struct vring_desc *descs = (struct vring_desc*) shm;
  struct vring_avail *avail = (struct vring_avail*) (shm + num_buffers * sizeof(struct vring_desc));
  struct vring_used *used = (struct vring_used*) ((((uintptr_t) &avail->ring[num_buffers]) + sizeof(uint16_t) + align - 1) & ~(align - 1));

  for(int i = 0; i < num_buffers; i++) {
    printf("addr = 0x%"PRIx64" len = %"PRIu32" flags = %"PRIu16" next = %"PRIu16"\n", descs[i].addr, descs[i].len, descs[i].flags, descs[i].next); 

    char a[128], b[128];
    char *ta = a, *tb = b;
    a[0] = b[0] = 0;
    if(descs[i].addr && dump_content && descs[i].len > 0) {
      char *ptr = mmap(0, descs[i].len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, descs[i].addr & ~PAGE_SIZE) + ((uint64_t) descs[i].addr & PAGE_SIZE);
      if(ptr != MAP_FAILED) {
        for(int j = 0; j < descs[i].len; j++) {
          ta += sprintf(ta, "%02X ", ptr[j]);
          tb += sprintf(tb, "%c", isprint(ptr[j]) ? ptr[j] : '.');
          if((j + 1) % 16 == 0) {
            printf("%s    %s\n", a, b);
            a[0] = b[0] = 0;
            ta = a;
            tb = b;
            if((j + 1) / 16 == 1) {
              uint32_t *fields = (uint32_t *) ptr;
              printf(
                  "   src=0x%x dst=0x%x res=0x%x flags=0x%x\n",
                  fields[0], fields[1], fields[2], fields[3]
                  );
            }
          }
        }
        if(descs[i].len % 16) {
          printf("%s    %s\n", a, b);
        }
      } else {
        printf("Failed to mmap 0x%"PRIx64": %s\n", descs[i].addr, strerror(errno));
      }
      munmap(ptr, descs[i].len);
      printf("\n");
    }
  }

  printf("AVAILABLE\n");
  printf("\nflags = %" PRIu16 " idx = %" PRIu16 "\n", avail->flags, avail->idx);
  for(int i = 0; i < num_buffers; i++) {
    printf("  [%d] = %" PRIu16 "\n", i, avail->ring[i]);
  }
  printf("\n");

  printf("USED\n");
  printf("\nflags = %" PRIu16 " idx = %" PRIu16 "\n", used->flags, used->idx);
  for(int i = 0; i < num_buffers; i++) {
    printf("  [%d] = {idx = %d, len = %d}\n", i, used->ring[i].id, used->ring[i].len);
  }
  printf("\n");
}


int main(int argc, char** argv) {
  int dump_content = 1;

  if(argc <= 2) {
    fprintf(stderr, "Usage: %s 0xstartaddr num_buffers\n", argv[0]);
    return 1;
  }

  char *end;
  char *arg = argv[1];
  uint32_t start = strtol(arg, &end, 16);
  if(arg + strlen(arg) != end) {
    fprintf(stderr, "Failed to parse address: %s\n", arg);
    return 1;
  }

  arg = argv[2];
  uint16_t num_buffers = strtol(arg, &end, 10);
  if(arg + strlen(arg) != end) {
    fprintf(stderr, "Failed to parse num_buffers: %s\n", arg);
    return 1;
  }


  for(int i = 3; i < argc; i++) {
    if(strcmp(argv[i], "--no-content") == 0) {
      dump_content = 0;
    } else {
      fprintf(stderr, "Unknown parameter: %s\n", argv[i]);
      exit(1);
    }
  }

  int fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) {
    return 1;
  }

  dump(fd, start, num_buffers, dump_content);
  return 0;
}
