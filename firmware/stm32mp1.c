#include <stdio.h>

char rproc_trace[2048];
static int rproc_trace_offset;

int _write(int fd, char *buf, int len) {
	for(int i = 0; i < len; i++) {
		rproc_trace[rproc_trace_offset] = buf[i];
		rproc_trace[rproc_trace_offset++ + 1] = '\0';

		if (rproc_trace_offset + 1 >= sizeof(rproc_trace)) {
			rproc_trace_offset = 0;
		}
	}
	return len;
}

struct rsc_trace {
	uint32_t type;
	uint32_t da;
	uint32_t len;
	uint32_t reserved;
	uint8_t name[32];
};

struct rsc_table {
	uint32_t version;
	uint32_t num;
	uint32_t reserved[2];
	uint32_t offset[1];
	struct rsc_trace trace;
};

static struct rsc_table __attribute__((__section__(".resource_table"))) __attribute__((__used__)) resource_table = {
	.version = 1,
	.num = 1,
	.reserved = {0, 0},
	.offset = {
		offsetof(struct rsc_table, trace),
	},
	.trace = {
		.type = 2,
		.da = (uint32_t) rproc_trace,
		.len = sizeof(rproc_trace),
		.reserved = 0,
		.name = "cm4_log"
	},
};

extern void _estack(void);
extern uint8_t _sidata;
extern uint8_t _sdata;
extern uint8_t _edata;
extern uint8_t __bss_start__;
extern uint8_t __bss_end__;
extern int main();

void Reset_Handler() {
 // copy .data section from flash to sram
  uint8_t *src = &_sidata;
  uint8_t *dst = &_sdata;
  while(dst < &_edata) {
    *dst++ = *src++;
  }

	// clear BSS
	dst = &__bss_start__;
	while(dst < &__bss_end__) {
		*dst++ = 0;
	}

  main();
  for(;;);
}

__attribute__ ((used, section(".isr_vector"))) void (*vectors[])(void) = {
  &_estack,
  Reset_Handler,
};
