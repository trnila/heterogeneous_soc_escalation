CFLAGS = -Wall
BINS = hijack m4load
SSH_DST ?= imx8

hijack: hijack.c
	aarch64-linux-gnu-gcc $^ -o $@ ${CFLAGS}

m4load: m4load.c
	aarch64-linux-gnu-gcc $^ -o $@ ${CFLAGS}

modules:
	( \
		export CROSS_COMPILE=aarch64-linux-gnu- ARCH=arm64 KDIR=~/linuxes/linux-fslc && \
		make -C kernel_modules/su && \
		make -C kernel_modules/find_proc  \
	)

upload: $(BINS) modules
	scp $(shell find -name "*.ko") $(BINS) ${SSH_DST}:/tmp/

