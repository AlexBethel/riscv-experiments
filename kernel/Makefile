CFLAGS += -fno-zero-initialized-in-bss -Oz
CFLAGS += -MD

LDFLAGS += -Wl,--script ./linker.ld -nostdlib

kernel.flat: kernel
	$(OBJCOPY) -O binary $^ $@

kernel: kernel.o entry.o

install:
	install -Dt ${out}/bin kernel

clean:
	$(RM) *.o *.d kernel kernel.flat
