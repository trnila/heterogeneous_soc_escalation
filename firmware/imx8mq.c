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

  // use muxed RX input
  UART2->UCR3 |= UART_UCR3_RXDMUXSEL_MASK;

  // set reference frekvency divider to 1
  UART2->UFCR = (UART2->UFCR & ~UART_UFCR_RFDIV_MASK) | UART_UFCR_RFDIV(0b101); 
  // set baud rate
  UART2->UBIR = 0;
  UART2->UBMR = uart_refclk / (16UL * uart_bauds);

  // configure parameters and enable
  UART2->UCR2 = UART_UCR2_SRST_MASK
    | UART_UCR2_TXEN_MASK
    | UART_UCR2_RXEN_MASK
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

ssize_t _read(int fd, char* buf, size_t count) {
  while(!(UART2->USR2 & 1)) {}
  *buf = UART2->URXD;
  return 1;
}

extern void __StackTop(void);
extern uint8_t __etext;
extern uint8_t __data_start__;
extern uint8_t __data_end__;
extern uint8_t __bss_start__;
extern uint8_t __bss_end__;
extern int main();

__attribute__((naked))
void Reset_Handler(void) {
  __set_MSP((uint32_t) &__StackTop);

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

static void default_irqhandler() {
  for(;;) {
    printf("Unhandled IRQ\r\n");
    asm("bkpt #1");
  }
}

void NMI_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void HardFault_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void MemManage_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void BusFault_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void UsageFault_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void SVC_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void DebugMon_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void PendSV_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void SysTick_Handler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPR_IRQ_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void DAP_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SDMA1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPU_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SNVS_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void LCDIF_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SPDIF1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void H264_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void VPUDMA_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void QOS_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void WDOG3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HS_CP1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void APBHDMA_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SPDIF2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void BCH_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPMI_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HDMI_IRQ0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HDMI_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HDMI_IRQ2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SNVS_Consolidated_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SNVS_Security_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CSU_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void USDHC1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void USDHC2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void DDC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void DTRC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void UART1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void UART2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void UART3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void UART4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void VP9_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ECSPI1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ECSPI2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ECSPI3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void MIPI_DSI_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2C1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2C2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2C3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2C4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void RDC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void USB1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void USB2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CSI1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CSI2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void MIPI_CSI1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void MIPI_CSI2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT6_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SCTR_IRQ0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SCTR_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void TEMPMON_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2S3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT5_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPT1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT7_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT6_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT5_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_INT0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_Combined_0_15_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO1_Combined_16_31_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO2_Combined_0_15_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO2_Combined_16_31_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO3_Combined_0_15_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO3_Combined_16_31_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO4_Combined_0_15_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO4_Combined_16_31_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO5_Combined_0_15_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPIO5_Combined_16_31_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL2_IRQ0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL2_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL2_IRQ2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL2_IRQ3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void WDOG1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void WDOG2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PWM1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PWM2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PWM3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PWM4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CCM_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CCM_IRQ2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void GPC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void MU_A53_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SRC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2S56_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void RTIC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CPU_PerformanceUnit_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CPU_CTI_Trigger_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SRC_Combined_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2S1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2S2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void MU_M4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void DDR_PerformanceMonitor_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void DDR_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void I2S4_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CPU_Error_AXI_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CPU_Error_L2RAM_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void SDMA2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void Reserved120_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CAAM_IRQ0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CAAM_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void QSPI_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void TZASC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void Reserved125_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void Reserved126_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void Reserved127_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PERFMON1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PERFMON2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CAAM_IRQ2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void CAAM_ERROR_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HS_CP0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void HEVC_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ENET_MAC0_Rx_Tx_Done1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ENET_MAC0_Rx_Tx_Done2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ENET_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void ENET_1588_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL1_IRQ0_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL1_IRQ1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL1_IRQ2_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL1_IRQ3_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void Reserved142_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));
void PCIE_CTRL1_IRQHandler() __attribute__ ((weak, alias ("default_irqhandler")));

__attribute__ ((used, section(".isr_vector"))) void (*vectors[])(void) = {
  &__StackTop,
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
  GPR_IRQ_IRQHandler,
  DAP_IRQHandler,
  SDMA1_IRQHandler,
  GPU_IRQHandler,
  SNVS_IRQHandler,
  LCDIF_IRQHandler,
  SPDIF1_IRQHandler,
  H264_IRQHandler,
  VPUDMA_IRQHandler,
  QOS_IRQHandler,
  WDOG3_IRQHandler,
  HS_CP1_IRQHandler,
  APBHDMA_IRQHandler,
  SPDIF2_IRQHandler,
  BCH_IRQHandler,
  GPMI_IRQHandler,
  HDMI_IRQ0_IRQHandler,
  HDMI_IRQ1_IRQHandler,
  HDMI_IRQ2_IRQHandler,
  SNVS_Consolidated_IRQHandler,
  SNVS_Security_IRQHandler,
  CSU_IRQHandler,
  USDHC1_IRQHandler,
  USDHC2_IRQHandler,
  DDC_IRQHandler,
  DTRC_IRQHandler,
  UART1_IRQHandler,
  UART2_IRQHandler,
  UART3_IRQHandler,
  UART4_IRQHandler,
  VP9_IRQHandler,
  ECSPI1_IRQHandler,
  ECSPI2_IRQHandler,
  ECSPI3_IRQHandler,
  MIPI_DSI_IRQHandler,
  I2C1_IRQHandler,
  I2C2_IRQHandler,
  I2C3_IRQHandler,
  I2C4_IRQHandler,
  RDC_IRQHandler,
  USB1_IRQHandler,
  USB2_IRQHandler,
  CSI1_IRQHandler,
  CSI2_IRQHandler,
  MIPI_CSI1_IRQHandler,
  MIPI_CSI2_IRQHandler,
  GPT6_IRQHandler,
  SCTR_IRQ0_IRQHandler,
  SCTR_IRQ1_IRQHandler,
  TEMPMON_IRQHandler,
  I2S3_IRQHandler,
  GPT5_IRQHandler,
  GPT4_IRQHandler,
  GPT3_IRQHandler,
  GPT2_IRQHandler,
  GPT1_IRQHandler,
  GPIO1_INT7_IRQHandler,
  GPIO1_INT6_IRQHandler,
  GPIO1_INT5_IRQHandler,
  GPIO1_INT4_IRQHandler,
  GPIO1_INT3_IRQHandler,
  GPIO1_INT2_IRQHandler,
  GPIO1_INT1_IRQHandler,
  GPIO1_INT0_IRQHandler,
  GPIO1_Combined_0_15_IRQHandler,
  GPIO1_Combined_16_31_IRQHandler,
  GPIO2_Combined_0_15_IRQHandler,
  GPIO2_Combined_16_31_IRQHandler,
  GPIO3_Combined_0_15_IRQHandler,
  GPIO3_Combined_16_31_IRQHandler,
  GPIO4_Combined_0_15_IRQHandler,
  GPIO4_Combined_16_31_IRQHandler,
  GPIO5_Combined_0_15_IRQHandler,
  GPIO5_Combined_16_31_IRQHandler,
  PCIE_CTRL2_IRQ0_IRQHandler,
  PCIE_CTRL2_IRQ1_IRQHandler,
  PCIE_CTRL2_IRQ2_IRQHandler,
  PCIE_CTRL2_IRQ3_IRQHandler,
  WDOG1_IRQHandler,
  WDOG2_IRQHandler,
  PCIE_CTRL2_IRQHandler,
  PWM1_IRQHandler,
  PWM2_IRQHandler,
  PWM3_IRQHandler,
  PWM4_IRQHandler,
  CCM_IRQ1_IRQHandler,
  CCM_IRQ2_IRQHandler,
  GPC_IRQHandler,
  MU_A53_IRQHandler,
  SRC_IRQHandler,
  I2S56_IRQHandler,
  RTIC_IRQHandler,
  CPU_PerformanceUnit_IRQHandler,
  CPU_CTI_Trigger_IRQHandler,
  SRC_Combined_IRQHandler,
  I2S1_IRQHandler,
  I2S2_IRQHandler,
  MU_M4_IRQHandler,
  DDR_PerformanceMonitor_IRQHandler,
  DDR_IRQHandler,
  I2S4_IRQHandler,
  CPU_Error_AXI_IRQHandler,
  CPU_Error_L2RAM_IRQHandler,
  SDMA2_IRQHandler,
  Reserved120_IRQHandler,
  CAAM_IRQ0_IRQHandler,
  CAAM_IRQ1_IRQHandler,
  QSPI_IRQHandler,
  TZASC_IRQHandler,
  Reserved125_IRQHandler,
  Reserved126_IRQHandler,
  Reserved127_IRQHandler,
  PERFMON1_IRQHandler,
  PERFMON2_IRQHandler,
  CAAM_IRQ2_IRQHandler,
  CAAM_ERROR_IRQHandler,
  HS_CP0_IRQHandler,
  HEVC_IRQHandler,
  ENET_MAC0_Rx_Tx_Done1_IRQHandler,
  ENET_MAC0_Rx_Tx_Done2_IRQHandler,
  ENET_IRQHandler,
  ENET_1588_IRQHandler,
  PCIE_CTRL1_IRQ0_IRQHandler,
  PCIE_CTRL1_IRQ1_IRQHandler,
  PCIE_CTRL1_IRQ2_IRQHandler,
  PCIE_CTRL1_IRQ3_IRQHandler,
  Reserved142_IRQHandler,
  PCIE_CTRL1_IRQHandler,
};
