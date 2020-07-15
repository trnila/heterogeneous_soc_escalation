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

const uint32_t CCM_TARGET_ROOT1 = 0x30388080;
const uint32_t CCM_TARGET_ROOT_ENABLE_SHIFT = 28;
const uint32_t SRC_M4RCR = 0x3039000C;
const uint32_t TCM = 0x7e0000;
const uint32_t TCM_SIZE = 256 * 1024;
const uint32_t M4_CTRL_MASK = 0b1111;
const uint32_t M4_STOP      = 0b1100;
const uint32_t M4_START     = 0b1010;

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
    if(argc != 2) {
      fprintf(stderr, "Usage: %s firmware.bin\n", argv[0]);
      exit(1);
    }

    int memfd = open("/dev/mem", O_RDWR | O_CLOEXEC);
    if(memfd < 0) {
      perror("open(/dev/mem): ");
      exit(1);
    }

    // map SRC_M4RCR and TCM memory
    volatile uint32_t *reg = (volatile uint32_t*) map_mem(memfd, SRC_M4RCR, getpagesize());
    volatile uint8_t *tcm = map_mem(memfd, TCM, TCM_SIZE);
    volatile uint32_t *ccm = (volatile uint32_t*) map_mem(memfd, 0x30388080, getpagesize());
    close(memfd);

    // enable M4 clk
    *ccm |= 1 << CCM_TARGET_ROOT_ENABLE_SHIFT;

    int fd = open(argv[1], O_RDONLY);
    if(!fd) {
      perror("open");
      exit(1);
    }
    off_t fsize = lseek(fd, 0, SEEK_END);
    if(fsize >= TCM_SIZE) {
      fprintf(stderr, "Firmware is too large. %lu > %u\n", fsize, TCM_SIZE);
      exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    // stop m4 core
    *reg = (*reg & ~M4_CTRL_MASK) | M4_STOP;

    // load firmware
    uint8_t firmware[TCM_SIZE];
    size_t len = read(fd, firmware, sizeof(firmware));
    close(fd);
    for(size_t i = 0; i < len; i++) {
      tcm[i] = firmware[i];
    }

    // start m4 core
    *reg = (*reg & ~M4_CTRL_MASK) | M4_START;

    return 0;
}
