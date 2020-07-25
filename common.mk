ROOT_DIR:=$(dir $(lastword $(MAKEFILE_LIST)))

FIRMWARE_CFLAGS=-mcpu=cortex-m4 \
			 -ffreestanding \
			 -fno-builtin \
			 -lnosys \
			 -nostartfiles \
			 -g \
			 -I $(ROOT_DIR)/firmware/CMSIS/Include/ \
			 -I $(ROOT_DIR)/firmware/MIMX8MQ6/

FIRMWARE_LDFLAGS=-lc \
			 -flto \
			 --specs=nosys.specs \
			 --specs=nano.specs 

IMX8MQ_FLAGS=-T $(ROOT_DIR)/firmware/MIMX8MQ6xxxJZ_cm4_ram.ld -DCPU_MIMX8MQ6DVAJZ 
STM32MP1_FLAGS=-T $(ROOT_DIR)/firmware/STM32MP157CAAX_RAM.ld 

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $^ $@

%.imx8mq.elf: %.imx8mq.c $(ROOT_DIR)/firmware/imx8mq.c
	arm-none-eabi-gcc $(FIRMWARE_CFLAGS) $(FIRMWARE_LDFLAGS) $(IMX8MQ_FLAGS) $^ -o $@
	arm-none-eabi-size $@

%.stm32mp1.elf: %.stm32mp1.c $(ROOT_DIR)/firmware/stm32mp1.c
	arm-none-eabi-gcc $(FIRMWARE_CFLAGS) $(FIRMWARE_LDFLAGS) $(STM32MP1_FLAGS) $^ -o $@
	arm-none-eabi-size $@

%.aarch64: %.c
	aarch64-linux-gnu-gcc $^ -o $@ $(CFLAGS)

%.arm32: %.c
	arm-linux-gnueabihf-gcc $^ -o $@ $(CFLAGS)

clean:
	rm -f *.o *.bin *.elf *.arm32 *.aarch64
