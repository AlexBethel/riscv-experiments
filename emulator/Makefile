CFLAGS += -g
CFLAGS += -MD

emulator: emulator.o

install:
	install -Dt ${out}/bin emulator

clean:
	$(RM) *.o *.d emulator

-include *.d
