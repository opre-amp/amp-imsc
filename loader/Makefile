obj-m += loader.o
all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(LINUX) modules
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(LINUX) clean
