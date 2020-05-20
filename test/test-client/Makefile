ARCH = -march=armv8.2-a -mtune=cortex-a53 -marm -mfpu=neon-fp-armv8 -mfloat-abi=hard


TARGET=test-client

$(TARGET): $(TARGET).c
	arm-linux-gnueabihf-gcc $(ARCH) -o $(TARGET) $(TARGET).c

clean: 
	-rm $(TARGET)
