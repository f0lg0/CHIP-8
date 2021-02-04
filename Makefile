LDFLAGS=-lm -lSDL2 -L/usr/local/lib

all:
	mkdir bin
	$(CC) ./src/main.c ./src/chip8.c ./src/peripherals.c -o ./bin/emulator.out -Wall -Wextra -pedantic -std=c99 $(LDFLAGS)

clean:
	rm -rf bin
