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

all: escalation.imx8mq.bin escalation.stm32mp1.elf

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $^ $@

escalation.imx8mq.elf: escalation.c imx8mq.c
	arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) -T MIMX8MQ6xxxJZ_cm4_ram.ld -DCPU_MIMX8MQ6DVAJZ  $^ -o $@
	arm-none-eabi-size $@

escalation.stm32mp1.elf: escalation.c stm32mp1.c
	arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) -T STM32MP157CAAX_RAM.ld $^ -o $@
	arm-none-eabi-size $@

clean:
	rm -f *.o *.bin *.elf

