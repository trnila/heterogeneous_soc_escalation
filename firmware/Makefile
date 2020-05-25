CC=arm-none-eabi-gcc

CFLAGS=-mcpu=cortex-m4 \
			 -Wall \
			 -DCPU_MIMX8MQ6DVAJZ \
			 -ffreestanding \
			 -fno-builtin \
			 -lnosys \
			 -nostartfiles \
			 -Os \
			 -I CMSIS/Include/ \
			 -I MIMX8MQ6/

LDFLAGS=-T MIMX8MQ6xxxJZ_cm4_ram.ld \
			 -lc \
			 -flto \
			 --specs=nosys.specs \
			 --specs=nano.specs 

all: root.bin

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $^ $@
	arm-none-eabi-size $^

root.elf: root.o imx8mq.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o *.bin *.elf

