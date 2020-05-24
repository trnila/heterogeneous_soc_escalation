CFLAGS=-Wall

hijack: hijack.c
	aarch64-linux-gnu-gcc $^ -o $@ ${CFLAGS}

modules:
	( \
		export CROSS_COMPILE=aarch64-linux-gnu- ARCH=arm64 KDIR=~/linuxes/linux-fslc && \
		make -C kernel_modules/su && \
		make -C kernel_modules/find_proc  \
	)

upload: hijack modules
	scp $(shell find -name "*.ko") hijack imx8:/tmp/

