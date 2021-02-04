#include <stdio.h>
#include <unistd.h>

#include "inc/chip8.h"
#include "inc/peripherals.h"

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

    if ((check_rom(rom_filename) == 1)) {
        printf("[FAILED] Errors while loading rom!\n");
        return 1;
    }

    load_rom(rom_filename);
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
