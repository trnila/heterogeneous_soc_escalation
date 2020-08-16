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

static void default_irqhandler() {
	printf("Unhandled IRQ\r\n");
	for(;;) {
		asm("bkpt #1");
	}
}

void NMI_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void HardFault_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void MemManage_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void BusFault_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void UsageFault_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SVC_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DebugMon_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void PendSV_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SysTick_Handler()  __attribute__ ((weak, alias ("default_irqhandler")));

void WWDG1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void PVD_AVD_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TAMP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RTC_WKUP_ALARM_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RESERVED4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RCC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void ADC1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FDCAN1_IT0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FDCAN2_IT0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FDCAN1_IT1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FDCAN2_IT1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM1_BRK_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM1_UP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM1_TRG_COM_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM1_CC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C1_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C1_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C2_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C2_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USART1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USART2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USART3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI10_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RTC_TIMESTAMP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI11_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM8_BRK_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM8_UP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM8_TRG_COM_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM8_CC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA1_Stream7_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FMC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SDMMC1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void UART4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void UART5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM7_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void ETH1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void ETH1_WKUP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FDCAN_CAL_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI7_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI8_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI9_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMA2_Stream7_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USART6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C3_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C3_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USBH_OHCI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void USBH_EHCI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI12_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI13_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DCMI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void CRYP1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void HASH1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void FPU_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void UART7_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void UART8_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPI6_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SAI1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LTDC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LTDC_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void ADC2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SAI2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void QUADSPI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LPTIM1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void CEC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C4_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C4_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SPDIF_RX_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void OTG_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RESERVED99_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void IPCC_RX0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void IPCC_TX0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DMAMUX1_OVR_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void IPCC_RX1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void IPCC_TX1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void CRYP2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void HASH2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C5_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C5_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void GPU_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT0_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SAI3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM15_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM16_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM17_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM12_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void MDIOS_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI14_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void MDMA_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DSI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SDMMC2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void HSEM_IT2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DFSDM1_FLT5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void EXTI15_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void nCTIIRQ1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void nCTIIRQ2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM13_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void TIM14_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DAC_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RNG1_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RNG2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C6_EV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void I2C6_ER_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SDMMC3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LPTIM2_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LPTIM3_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LPTIM4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void LPTIM5_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void ETH1_LPI_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RESERVED143_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void MPU_SEV_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RCC_WAKEUP_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void SAI4_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void DTS_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void RESERVED148_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));
void WAKEUP_PIN_IRQHandler()  __attribute__ ((weak, alias ("default_irqhandler")));


__attribute__ ((used, section(".isr_vector"))) void (*vectors[])(void) = {
  &_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  0,
  0,
  0,
  0,
  SVC_Handler,
  DebugMon_Handler,
  0,
  PendSV_Handler,
  SysTick_Handler,

  WWDG1_IRQHandler,
  PVD_AVD_IRQHandler,
  TAMP_IRQHandler,
  RTC_WKUP_ALARM_IRQHandler,
  RESERVED4_IRQHandler,
  RCC_IRQHandler,
  EXTI0_IRQHandler,
  EXTI1_IRQHandler,
  EXTI2_IRQHandler,
  EXTI3_IRQHandler,
  EXTI4_IRQHandler,
  DMA1_Stream0_IRQHandler,
  DMA1_Stream1_IRQHandler,
  DMA1_Stream2_IRQHandler,
  DMA1_Stream3_IRQHandler,
  DMA1_Stream4_IRQHandler,
  DMA1_Stream5_IRQHandler,
  DMA1_Stream6_IRQHandler,
  ADC1_IRQHandler,
  FDCAN1_IT0_IRQHandler,
  FDCAN2_IT0_IRQHandler,
  FDCAN1_IT1_IRQHandler,
  FDCAN2_IT1_IRQHandler,
  EXTI5_IRQHandler,
  TIM1_BRK_IRQHandler,
  TIM1_UP_IRQHandler,
  TIM1_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM4_IRQHandler,
  I2C1_EV_IRQHandler,
  I2C1_ER_IRQHandler,
  I2C2_EV_IRQHandler,
  I2C2_ER_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_IRQHandler,
  EXTI10_IRQHandler,
  RTC_TIMESTAMP_IRQHandler,
  EXTI11_IRQHandler,
  TIM8_BRK_IRQHandler,
  TIM8_UP_IRQHandler,
  TIM8_TRG_COM_IRQHandler,
  TIM8_CC_IRQHandler,
  DMA1_Stream7_IRQHandler,
  FMC_IRQHandler,
  SDMMC1_IRQHandler,
  TIM5_IRQHandler,
  SPI3_IRQHandler,
  UART4_IRQHandler,
  UART5_IRQHandler,
  TIM6_IRQHandler,
  TIM7_IRQHandler,
  DMA2_Stream0_IRQHandler,
  DMA2_Stream1_IRQHandler,
  DMA2_Stream2_IRQHandler,
  DMA2_Stream3_IRQHandler,
  DMA2_Stream4_IRQHandler,
  ETH1_IRQHandler,
  ETH1_WKUP_IRQHandler,
  FDCAN_CAL_IRQHandler,
  EXTI6_IRQHandler,
  EXTI7_IRQHandler,
  EXTI8_IRQHandler,
  EXTI9_IRQHandler,
  DMA2_Stream5_IRQHandler,
  DMA2_Stream6_IRQHandler,
  DMA2_Stream7_IRQHandler,
  USART6_IRQHandler,
  I2C3_EV_IRQHandler,
  I2C3_ER_IRQHandler,
  USBH_OHCI_IRQHandler,
  USBH_EHCI_IRQHandler,
  EXTI12_IRQHandler,
  EXTI13_IRQHandler,
  DCMI_IRQHandler,
  CRYP1_IRQHandler,
  HASH1_IRQHandler,
  FPU_IRQHandler,
  UART7_IRQHandler,
  UART8_IRQHandler,
  SPI4_IRQHandler,
  SPI5_IRQHandler,
  SPI6_IRQHandler,
  SAI1_IRQHandler,
  LTDC_IRQHandler,
  LTDC_ER_IRQHandler,
  ADC2_IRQHandler,
  SAI2_IRQHandler,
  QUADSPI_IRQHandler,
  LPTIM1_IRQHandler,
  CEC_IRQHandler,
  I2C4_EV_IRQHandler,
  I2C4_ER_IRQHandler,
  SPDIF_RX_IRQHandler,
  OTG_IRQHandler,
  RESERVED99_IRQHandler,
  IPCC_RX0_IRQHandler,
  IPCC_TX0_IRQHandler,
  DMAMUX1_OVR_IRQHandler,
  IPCC_RX1_IRQHandler,
  IPCC_TX1_IRQHandler,
  CRYP2_IRQHandler,
  HASH2_IRQHandler,
  I2C5_EV_IRQHandler,
  I2C5_ER_IRQHandler,
  GPU_IRQHandler,
  DFSDM1_FLT0_IRQHandler,
  DFSDM1_FLT1_IRQHandler,
  DFSDM1_FLT2_IRQHandler,
  DFSDM1_FLT3_IRQHandler,
  SAI3_IRQHandler,
  DFSDM1_FLT4_IRQHandler,
  TIM15_IRQHandler,
  TIM16_IRQHandler,
  TIM17_IRQHandler,
  TIM12_IRQHandler,
  MDIOS_IRQHandler,
  EXTI14_IRQHandler,
  MDMA_IRQHandler,
  DSI_IRQHandler,
  SDMMC2_IRQHandler,
  HSEM_IT2_IRQHandler,
  DFSDM1_FLT5_IRQHandler,
  EXTI15_IRQHandler,
  nCTIIRQ1_IRQHandler,
  nCTIIRQ2_IRQHandler,
  TIM13_IRQHandler,
  TIM14_IRQHandler,
  DAC_IRQHandler,
  RNG1_IRQHandler,
  RNG2_IRQHandler,
  I2C6_EV_IRQHandler,
  I2C6_ER_IRQHandler,
  SDMMC3_IRQHandler,
  LPTIM2_IRQHandler,
  LPTIM3_IRQHandler,
  LPTIM4_IRQHandler,
  LPTIM5_IRQHandler,
  ETH1_LPI_IRQHandler,
  RESERVED143_IRQHandler,
  MPU_SEV_IRQHandler,
  RCC_WAKEUP_IRQHandler,
  SAI4_IRQHandler,
  DTS_IRQHandler,
  RESERVED148_IRQHandler,
  WAKEUP_PIN_IRQHandler,
};
