# Disable most RISC-V extensions to test the emulator
CFLAGS = -march=rv32i -mabi=ilp32

%.hex: %.o
	$(OBJDUMP) -d $^
	$(OBJCOPY) -j .text -O binary $^ $@
