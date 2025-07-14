# Turn on optimizations, and disable use of the `.bss` section in
# favor of the `.data` section.
CFLAGS = -Oz -fno-zero-initialized-in-bss

# Disable most RISC-V extensions to test the emulator
CFLAGS += -march=rv32i -mabi=ilp32 -fno-pic
CFLAGS += -nostdlib

# Use a linker script to precisely control how sections are layed out.
LDFLAGS = -Oz -Wl,--script ./linker.ld -nostdlib

first.raw: first
	$(OBJCOPY) -O binary $^ $@

.PHONY: clean
clean:
	$(RM) first.raw first first.o
