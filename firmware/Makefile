CFLAGS=-mcpu=cortex-m4 \
			 -Wall \
			 -ffreestanding \
			 -fno-builtin \
			 -lnosys \
			 -nostartfiles \
			 -Os \
			 -I CMSIS/Include/ \
			 -I MIMX8MQ6/

LDFLAGS=-lc \
			 -flto \
			 --specs=nosys.specs \
			 --specs=nano.specs 

all: root.imx8mq.bin root.stm32mp1.elf

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $^ $@

root.imx8mq.elf: root.c imx8mq.c
	arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) -T MIMX8MQ6xxxJZ_cm4_ram.ld -DCPU_MIMX8MQ6DVAJZ  $^ -o $@
	arm-none-eabi-size $@

root.stm32mp1.elf: root.c stm32mp1.c
	arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) -T STM32MP157CAAX_RAM.ld $^ -o $@
	arm-none-eabi-size $@

clean:
	rm -f *.o *.bin *.elf

