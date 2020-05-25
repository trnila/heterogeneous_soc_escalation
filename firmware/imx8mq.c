#include "MIMX8MQ6_cm4.h"
#include <stdio.h>
#include "fsl_iomuxc.h"

void uart_init() {
  uint32_t uart_refclk = 25000000;
  uint32_t uart_bauds = 115200;

  IOMUXC_SetPinMux(IOMUXC_UART2_RXD_UART2_RX, 0U);
  IOMUXC_SetPinConfig(IOMUXC_UART2_RXD_UART2_RX, 
      IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
      IOMUXC_SW_PAD_CTL_PAD_SRE(1U) |
      IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
  IOMUXC_SetPinMux(IOMUXC_UART2_TXD_UART2_TX, 0U);
  IOMUXC_SetPinConfig(IOMUXC_UART2_TXD_UART2_TX, 
      IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
      IOMUXC_SW_PAD_CTL_PAD_SRE(1U) |
      IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);

  // ungate UART2 for all domains
  CCM->CCGR[74].CCGR_SET = 0x3333U;
  // Ungate UART2 root clock and set different input source
  CCM->ROOT[95].TARGET_ROOT = CCM_TARGET_ROOT_MUX(0b000 /* 25M_REF_CLK */)
    | CCM_TARGET_ROOT_ENABLE_MASK;

  // disable UART
  UART2->UCR1 &= ~UART_UCR1_UARTEN_MASK;

  // reset UART
  UART2->UCR2 &= ~UART_UCR2_SRST_MASK;

  // set reference frekvency divider to 1
  UART2->UFCR = (UART2->UFCR & ~UART_UFCR_RFDIV_MASK) | UART_UFCR_RFDIV(0b101); 
  // set baud rate
  UART2->UBIR = 0;
  UART2->UBMR = uart_refclk / (16UL * uart_bauds);

  // configure parameters and enable
  UART2->UCR2 = UART_UCR2_SRST_MASK
    | UART_UCR2_TXEN_MASK
    | UART_UCR2_WS_MASK
    | UART_UCR2_IRTS_MASK;
  UART2->UCR1 = UART_UCR1_UARTEN_MASK;
}

// printf retarget
size_t _write(int fd, char *buffer, size_t len) {
  for(int i = 0; i < len; i++) {
    while(!(UART2->USR2 & UART_USR2_TXFE_MASK));
    UART2->UTXD = buffer[i];
  }
  return len;
}

extern void __StackTop(void);
extern uint8_t __etext;
extern uint8_t __data_start__;
extern uint8_t __data_end__;
extern uint8_t __bss_start__;
extern uint8_t __bss_end__;
extern int main();

void Reset_Handler(void) {
 // copy .data section from flash to sram
  uint8_t *src = &__etext;
  uint8_t *dst = &__data_start__;
  while(dst < &__data_end__) {
    *dst++ = *src++;
  }

	// clear BSS
	dst = &__bss_start__;
	while(dst < &__bss_end__) {
		*dst++ = 0;
	}

  uart_init();
	main();
	asm("bkpt #1");
	for(;;);
}

__attribute__ ((used, section(".isr_vector"))) void (*vectors[])(void) = {
  &__StackTop,
  Reset_Handler,
};
