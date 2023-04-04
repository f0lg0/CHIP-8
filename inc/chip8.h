#ifndef CHIP8_H_
#define CHIP8_H_

extern unsigned char fontset[80];
extern unsigned char memory[4096];
extern unsigned char V[16];
extern unsigned short I;
extern unsigned short pc;
extern unsigned char sp;
extern unsigned short stack[16];
extern unsigned char keypad[16];
extern unsigned char display[64 * 32];
extern unsigned char dt;
extern unsigned char st;
extern unsigned char draw_flag;
extern unsigned char sound_flag;

void init_cpu(void);
int load_rom(char* filename);
void emulate_cycle(void);

#endif
