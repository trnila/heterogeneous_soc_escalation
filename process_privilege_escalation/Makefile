all: \
	victim_process.arm32 \
	victim_process.aarch64 \
	escalation.stm32mp1.elf \
	escalation.imx8mq.bin

include ../common.mk

escalation.imx8mq.elf: escalation.c $(ROOT_DIR)/firmware/imx8mq.c
	arm-none-eabi-gcc $(FIRMWARE_CFLAGS) $(FIRMWARE_LDFLAGS) $(IMX8MQ_FLAGS) $^ -o $@
	arm-none-eabi-size $@

escalation.stm32mp1.elf: escalation.c $(ROOT_DIR)/firmware/stm32mp1.c
	arm-none-eabi-gcc $(FIRMWARE_CFLAGS) $(FIRMWARE_LDFLAGS) $(STM32MP1_FLAGS) $^ -o $@
	arm-none-eabi-size $@
