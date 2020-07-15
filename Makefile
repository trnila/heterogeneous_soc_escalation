CFLAGS = -Wall
BINS = hijack.aarch64 hijack.arm32 \
			 m4load.aarch64

all: $(BINS) firmware

%.aarch64: %.c
	aarch64-linux-gnu-gcc $^ -o $@ $(CFLAGS)

%.arm32: %.c
	arm-linux-gnueabihf-gcc $^ -o $@ $(CFLAGS)

.PHONY: firmware
firmware:
	make -C firmware

clean:
	rm *.aarch64 *.arm32
	make -C firmware clean

