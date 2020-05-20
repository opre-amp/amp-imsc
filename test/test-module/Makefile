obj-m += gpio-test.o
gpio-test-objs += test.o irq_hndlr.o ioctl_ops.o
all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(LINUX) modules
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(LINUX) clean
