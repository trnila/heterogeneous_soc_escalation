#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


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
    int memfd = open("/dev/mem", O_RDWR | O_CLOEXEC);
    if(memfd < 0) {
      perror("open(/dev/mem): ");
      return 1;
    }

    // trustzone disallows to start core from the linux via RCC_MP_GCR register
    // volatile uint32_t *reg = (volatile uint32_t*) map_mem(memfd, 0x50000000+0x10c, getpagesize());
    // *reg = 1;

    volatile uint8_t *mem = (volatile uint8_t*) map_mem(memfd, 0x10000000, getpagesize());

    uint8_t instructions[] = {
      0x2a, 0x2b, // cmp r3, #42 ; 0x2a
      0x03, 0xd1, // bne.n 1000001c <main+0x1c>
    };
    uint8_t *begin = (uint8_t*) memmem(mem, getpagesize(), instructions, sizeof(instructions));
    if(!begin) {
      printf("instructions not found!\n");
      return 1;
    }

    begin[3] = 0xd0; // change bne to beq instruction
    return 0;
}
