ifneq ($(KERNELRELEASE),)
obj-m  := test.o
obj-m  := test2.o

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean


endif
