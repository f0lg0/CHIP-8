#include "inc/chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define debug_print(fmt, ...)                         \
    do {                                              \
        if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); \
    } while (0)

int DEBUG = 1;
/*
 * Memory map:
 *
 * Total: 4096 bytes (4K)
 *
 * 0x000 - 0x1FF     INTERPRETER
 * 0x200 - 0xFFF     Program/Data space
 *
 * NOTE:
 * opcodes are stored big-endian!
 *
 * */

/*
==========================================================
# CHIP-8 architecture
==========================================================
*/

// Font set
unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

// Memory
unsigned char memory[4096] = {0};

/*
 * Registers:
 * 16 general purpose 8-bit registers, usually referred to as Vx
 * where x is a hexadecimal digit.
 *
 * a char is 8-bits, so we store 16 of them
 * */
unsigned char V[16] = {0};

/*
 * Special 16-bit register called I
 * It is used to store memory addresses.
 *
 * a short is 16-bits
 * */
unsigned short I = 0;

/*
 * Pseudo-register: PC
 * The program counter is a 16-bit pseudo register used to
 * store the currectly executing address
 * */
unsigned short pc = 0x200;

/*
 * Pseudo-register: SP
 * The stack pointer register is used to point to the topmost level of the stack
 *
 * It is a 8-bit register
 * */
unsigned char sp = 0;

/*
 * The Stack:
 * Array of 16 16-bit values, used to store the address that the interpreter
 * should return to when finished with a subroutine
 * */
unsigned short stack[16] = {0};

// Keypad
unsigned char keypad[16] = {0};

/*
 * The Display:
 * A 64x32 px monochrome display
 * */
unsigned char display[64 * 32] = {0};

// Delay Timer
unsigned char dt = 0;

// Sound Timer
unsigned char st = 0;

// Update display flag
unsigned char draw_flag = 0;

// Play a sound flag
unsigned char sound_flag = 0;

/*
==========================================================
# CHIP-8 logic
==========================================================
*/

/**
 * init_cpu: Initialize CPU by loading fontset into mem
 * @param void
 * @return void
 * */
void init_cpu(void) {
    srand((unsigned int)time(NULL));

    // load fonts into memory
    for (int i = 0; i < 80; i++) {
        memory[i] = fontset[i];
    }
}

/**
 * check_rom: check if provided rom is accessible
 * @param filename The rom filename
 * @return 0 if success, 1 if fail
 * */
int check_rom(char* filename) {
    FILE* fp;

    if ((fp = fopen(filename, "rb"))) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * load_rom: load the provided rom to memory
 * @param filename The rom filename
 * @return void
 * */
void load_rom(char* filename) {
    FILE* fp = fopen(filename, "rb");

    // read program size into memory
    fread(memory + 0x200, 1, sizeof(memory) - 0x200, fp);

    fclose(fp);
}

/**
 * emulate_cycle: run chip-8 instructions
 * @param void
 * @return void
 *
 * 1) Fetch the operation code
 *      - fetch one opcode from the memory at the location specified by the
 * program counter. Each array address contains one byte, an opcode is 2 bytes
 * long so we need to fetch 2 consecutive bytes and merge them to get the actual
 * opcode.
 *          - for example:
 *              0xA2F0 --> pc = 0xA2, pc + 1 = 0xF0
 *              opcode = pc << 8 | pc + 1
 *                  - shifting A0 left by 8 bits, which adds 8 zeros (0xA200)
 *                  - bitwise OR to merge them
 *
 * 2) Decode the operation code
 *      - look up to the optable to see what the op means
 *          - for example:
 *              0xA2F0 --> ANNN: sets I to the address NNN (0x2F0)
 * 3) Execute the operation code
 *      - execute the parsed op
 *          - for example:
 *              0xA2F0 --> store 0x2F0 into the I register, as only 12-bits are
 * containing the value we need to store, we use a bitwise AND (with the value
 * 0x0FFF) to get rid of the first four bits.
 *                  - I =  opcode & 0x0FFF
 *                    pc += 2 --> every instruction is 2 bytes long
 * 4) Update timers
 *      - count to zero at 60hz if they are set to a number greater than 0
 * */
void emulate_cycle(void) {
    draw_flag = 0;
    sound_flag = 0;

    unsigned short op = memory[pc] << 8 | memory[pc + 1];

    // Vx register, we are basically "grabbing" the x present in some
    // instructions like 3XNN
    unsigned short x = (op & 0x0F00) >> 8;

    // Vy register, we are basically "grabbing" the y present in some
    // instructions like 5XY0
    unsigned short y = (op & 0x00F0) >> 4;

    switch (op & 0xF000) {
        // we need extra checking
        case 0x0000:
            switch (op & 0x00FF) {
                // 00E0: Clears the screen
                case 0x00E0:
                    debug_print("[OK] 0x%X: 00E0\n", op);
                    for (int i = 0; i < 64 * 32; i++) {
                        display[i] = 0;
                    }
                    pc += 2;
                    break;
                // 00EE: Returns from a subroutine
                case 0x00EE:
                    debug_print("[OK] 0x%X: 00EE\n", op);
                    pc = stack[sp];
                    sp--;
                    pc += 2;
                    break;
                default:
                    debug_print("[FAILED] Unknown opcode: 0x%X\n", op);
                    break;
            }
            break;

        // 1NNN: Jumps to address NNN
        case 0x1000:
            debug_print("[OK] 0x%X: 1NNN\n", op);
            pc = op & 0x0FFF;
            break;

        // 2NNN: Calls subroutine at NNN
        case 0x2000:
            debug_print("[OK] 0x%X: 2NNN\n", op);

            /*
             * We need to jump to NNN so we should store the current
             * address of the program counter in the stack. But before storing
             * we increment the stack pointer to prevent overwriting the current
             * stack. After correctly storing the address we can set the pc to
             * the address NNN. Since we are calling a subroutine at a specific
             * address we don't have to increase the program counter by two.
             * */
            sp += 1;
            stack[sp] = pc;
            pc = op & 0x0FFF;  // getting the NNN
            break;

        // 3XNN: Skips the next instruction if Vx equals NN
        case 0x3000:
            debug_print("[OK] 0x%X: 3XNN\n", op);

            // (big-endian) a right shift by 8 increases the byte addr by 1
            if (V[x] == (op & 0x00FF)) {
                pc += 2;
            }

            pc += 2;
            break;

        // 4XNN: Skips the next instruction if Vx !equal NN
        case 0x4000:
            debug_print("[OK] 0x%X: 4XNN\n", op);

            if (V[x] != (op & 0x00FF)) {
                pc += 2;
            }

            pc += 2;
            break;

        // 5XY0: Skips the next instruction if Vx equals Vy
        case 0x5000:
            debug_print("[OK] 0x%X: 5XY0\n", op);

            if (V[x] == V[y]) {
                pc += 2;
            }

            pc += 2;
            break;

        // 6XNN: Sets Vx to NN
        case 0x6000:
            debug_print("[OK] 0x%X: 6XNN\n", op);

            V[x] = (op & 0x00FF);
            pc += 2;
            break;

        // 7XNN: Adds NN to Vx
        case 0x7000:
            debug_print("[OK] 0x%X: 7XNN\n", op);

            V[x] += op & 0x00FF;
            pc += 2;
            break;

        // 8XYn: Multiple instructions where n is a number 0-7 or E
        case 0x8000:
            switch (op & 0x000F) {
                // 8XY0: Sets Vx to the value of Vy
                case 0x0000:
                    debug_print("[OK] 0x%X: 8XY0\n", op);

                    V[x] = V[y];
                    pc += 2;
                    break;

                // 8XY1: Sets Vx to Vx | Vy
                case 0x0001:
                    debug_print("[OK] 0x%X: 8XY1\n", op);

                    V[x] = (V[x] | V[y]);
                    pc += 2;
                    break;

                // 8XY2: Sets Vx to Vx & Vy
                case 0x0002:
                    debug_print("[OK] 0x%X: 8XY2\n", op);

                    V[x] = (V[x] & V[y]);
                    pc += 2;
                    break;

                // 8XY3: Sets vx to Vx
                case 0x0003:
                    debug_print("[OK] 0x%X: 8XY3\n", op);

                    V[x] = (V[x] ^ V[y]);
                    pc += 2;
                    break;

                // 8XY4: Adds Vy to Vx. Vf is set to 1 when there's a carry
                case 0x0004:
                    debug_print("[OK] 0x%X: 8XY4\n", op);

                    V[0xF] = (V[x] + V[y] > 0xFF) ? 1 : 0;
                    V[x] += V[y];

                    pc += 2;
                    break;

                // 8XY5: Vy is substracted from Vx. Vf is set to 0 when there's
                // a borrow
                case 0x0005:
                    debug_print("[OK] 0x%X: 8XY5\n", op);

                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];

                    pc += 2;
                    break;

                // 8XY6: Stores the least significant bit of Vx in Vf and then
                // shifts Vx to the right by 1
                case 0x0006:
                    debug_print("[OK] 0x%X: 8XY6\n", op);

                    V[0xF] = V[x] & 0x1;
                    V[x] = (V[x] >> 1);

                    pc += 2;
                    break;

                // 8XY7: Sets Vx to Vy minus Vx. Vf is set to 0 when there's a
                // borrow.
                case 0x0007:
                    debug_print("[OK] 0x%X: 8XY7\n", op);

                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];

                    pc += 2;
                    break;

                // 8XYE: Stores the most significant bit of Vx in Vf and shifts
                // Vx to the left by 1
                case 0x000E:
                    debug_print("[OK] 0x%X: 8XYE\n", op);

                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] = (V[x] << 1);

                    pc += 2;
                    break;

                default:
                    printf("[FAILED] Unknown op: 0x%X\n", op);
                    break;
            }
            break;

        // 9XY0: SKips the next instruction if Vx !equal Vy
        case 0x9000:
            debug_print("[OK] 0x%X: 9XY0", op);

            if (V[x] != V[y]) {
                pc += 2;
            }

            pc += 2;
            break;

        // ANNN: Sets I to the address NNN
        case 0xA000:
            debug_print("[OK] 0x%X: ANNN\n", op);

            I = op & 0x0FFF;
            pc += 2;
            break;

        // BNNN: Jumps to the address NNN plus V0
        case 0xB000:
            debug_print("[OK] 0x%X: BNNN\n", op);

            pc = (op & 0x0FFF) + V[0];
            break;

        // CXNN: Sets Vx to the result of a bitwise and operation on a random
        // number and NN
        case 0xC000:
            debug_print("[OK] 0x%X: CXNN\n", op);

            V[x] = (rand() % 256) & (op & 0x00FF);
            pc += 2;
            break;

        /*
         * DXYN:
         *
         * Draws a 8px * (N+1)px sprite at (V[x], Vy)
         * Each row of 8 pixels is read as bit-coded starting
         * from memory location I; I value doesn't change
         * after the execution of this instruction.
         * As described above, V[F] is set to 1
         * if any screen pixels are flipped from set
         * to unset when the sprite is
         * drawn, and to 0 if that doesn't happen.
         */
        case 0xD000:
            debug_print("[OK] 0x%X: DXYN\n", op);
            draw_flag = 1;

            unsigned short height = op & 0x000F;
            unsigned short px;

            // set collision flag to 0
            V[0xF] = 0;

            // loop over each row
            for (int yline = 0; yline < height; yline++) {
                // fetch the pixel value from the memory starting at location I
                px = memory[I + yline];

                // loop over 8 bits of one row
                for (int xline = 0; xline < 8; xline++) {
                    // check if current evaluated pixel is set to 1 (0x80 >>
                    // xline scnas throught the byte, one bit at the time)
                    if ((px & (0x80 >> xline)) != 0) {
                        // if drawing causes any pixel to be erased set the
                        // collision flag to 1
                        if (display[(V[x] + xline + ((V[y] + yline) * 64))] ==
                            1) {
                            V[0xF] = 1;
                        }

                        // set pixel value by using XOR
                        display[V[x] + xline + ((V[y] + yline) * 64)] ^= 1;
                    }
                }
            }

            pc += 2;
            break;

        // 2 instructions: 9E and A1
        case 0xE000:
            switch (op & 0x00FF) {
                // EX9E: Skips the next instruction if the key store in Vx is
                // pressed
                case 0x009E:
                    debug_print("[OK] 0x%X: EX9E\n", op);
                    if (keypad[V[x]]) {
                        pc += 2;
                    }

                    pc += 2;
                    break;

                // EXA1: Skips the next instruction if the key store in Vx isn't
                // pressed
                case 0x00A1:
                    debug_print("[OK] 0x%X: EXA1\n", op);
                    if (!keypad[V[x]]) {
                        pc += 2;
                    }

                    pc += 2;
                    break;

                default:
                    printf("[FAILED] Unknown op: 0x%X", op);
            }
            break;

        // Set of 7 instructions starting with FX
        case 0xF000:
            switch (op & 0x00FF) {
                // FX07: Sets Vx to the value of the delay timer
                case 0x0007:
                    debug_print("[OK] 0x%X: FX07\n", op);
                    V[x] = dt;

                    pc += 2;
                    break;

                // FX0A: A key press is awaited and then stored in Vx (blocking)
                case 0x000A:
                    debug_print("[OK] 0x%X: FX0A\n", op);

                    for (int i = 0; i < 16; i++) {
                        if (keypad[i]) {
                            V[x] = i;
                            pc += 2;
                            break;
                        }
                    }
                    break;

                // FX15: Sets the delay timer to Vx
                case 0x0015:
                    debug_print("[OK] 0x%X: FX15\n", op);

                    dt = V[x];
                    pc += 2;
                    break;

                // FX18: Sets the sound timer to Vx
                case 0x0018:
                    debug_print("[OK] 0x%X: FX18\n", op);

                    st = V[x];
                    pc += 2;
                    break;

                // FX1E: Adds Vx to I
                case 0x001E:
                    debug_print("[OK] 0x%X: FX1E\n", op);

                    I += V[x];
                    pc += 2;
                    break;

                // FX29: Sets I to the location of the sprite for the character
                // in Vx
                case 0x0029:
                    debug_print("[OK] 0x%X: FX29\n", op);

                    // each digit is 5 bytes long
                    I = V[x] * 5;
                    pc += 2;
                    break;

                /*
                 * FX33:
                 *
                 * Stores the binary-coded decimal representation
                 * of VX, with the most significant of three digits
                 * at the address in I, the middle digit at I plus
                 * 1, and the least significant digit at I plus 2.
                 * (In other words, take the decimal representation
                 * of VX, place the hundreds digit in memory
                 * at location in I, the tens digit at
                 * location I+1, and the ones digit at
                 * location I+2.)
                 * */
                case 0x0033:
                    debug_print("[OK] 0x%X: FX33\n", op);

                    memory[I] = (V[x] % 1000) / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = (V[x] % 10);

                    pc += 2;
                    break;

                // FX55: Stores V0 through Vx (Vx included) in memory starting
                // at addr I.
                case 0x0055:
                    debug_print("[OK] 0x%X: FX55\n", op);

                    for (int i = 0; i <= x; i++) {
                        V[i] = memory[I + i];
                    }

                    pc += 2;
                    break;

                // Fills V0 through Vx (Vx included) with values from memory
                // starting at addr I.
                case 0x0065:
                    debug_print("[OK] 0x%X: FX65\n", op);

                    for (int i = 0; i <= x; i++) {
                        V[i] = memory[I + i];
                    }

                    pc += 2;
                    break;

                default:
                    printf("[FAILED] Unknown op: 0x%X\n", op);
                    break;
            }
            break;

        default:
            debug_print("[FAILED] Unknown opcode: 0x%X\n", op);
            break;
    }

    /*
     * Update timers:
     *
     * Decrement timers if they are > 0
     * */
    if (dt > 0) dt -= 1;
    if (st > 0) {
        sound_flag = 1;
        printf("BEEP\n");
        st -= 1;
    }
}
