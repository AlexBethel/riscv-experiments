CFLAGS += -fno-zero-initialized-in-bss -Oz -flto
CFLAGS += -MD

LDFLAGS += -Wl,--script ./linker.ld -nostdlib -flto
LDLIBS += -lgcc

kernel.flat: kernel
	$(OBJCOPY) -O binary $^ $@

kernel: kernel.o entry.o

install:
	install -Dt ${out}/bin kernel
	install -Dt ${out}/bin kernel.flat

clean:
	$(RM) *.o *.d kernel kernel.flat

-include *.d
