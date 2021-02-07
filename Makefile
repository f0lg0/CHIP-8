.POSIX:
MYCFLAGS  = -I/usr/local/include -Wall -Wextra -pedantic -std=c99 $(CFLAGS)
MYLDFLAGS = -L/usr/local/lib $(LDFLAGS)
MYLDLIBS  = -lm -lSDL2 $(LDLIBS)

sources = src/main.c src/chip8.c src/peripherals.c
headers = src/inc/chip8.h src/inc/peripherals.h

all: bin/emulator.out

bin/emulator.out: $(sources) $(headers)
	@mkdir -p bin
	$(CC) $(MYCFLAGS) $(MYLDFLAGS) -o $@ $(sources) $(MYLDLIBS)

clean:
	rm -rf bin
