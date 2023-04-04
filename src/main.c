#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "chip8.h"
#include "peripherals.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: emulator rom.ch8\n");
        return 1;
    }

    printf("[PENDING] Initializing CHIP-8 arch...\n");
    init_cpu();
    printf("[OK] Done!\n");

    char* rom_filename = argv[1];
    printf("[PENDING] Loading rom %s...\n", rom_filename);

    int status = load_rom(rom_filename);

    if (status == -1) {
        printf("[FAILED] fread() failure: the return value was not equal to the rom file size.\n");
        return 1;
    } else if (status != 0) {
        perror("Error while loading rom");
        return 1;
    }

    printf("[OK] Rom loaded successfully!\n");

    init_display();
    printf("[OK] Display successfully initialized.\n");

    while (1) {
        emulate_cycle();
        sdl_ehandler(keypad);

        if (should_quit()) {
            break;
        }

        if (draw_flag) {
            draw(display);
        }

        usleep(1500);
    }

    stop_display();
    return 0;
}
