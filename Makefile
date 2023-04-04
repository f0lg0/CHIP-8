.POSIX:
CFLAGS  = -Iinc -I/usr/local/include -Wall -Wextra -pedantic -std=c99 
LDFLAGS = -L/usr/local/lib 
LIBS  = -lm -lSDL2 

sources = src/main.c src/chip8.c src/peripherals.c
objects = build/main.o build/chip8.o build/peripherals.o
headers = inc/chip8.h inc/peripherals.h

all: bin/emulator.out

bin/emulator.out: $(objects) $(headers)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(objects) $(LIBS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) -c $< $(CFLAGS) $(LDFLAGS) $(LIBS) -o$@

clean:
	rm -rf bin build
