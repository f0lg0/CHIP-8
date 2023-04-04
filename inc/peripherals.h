#ifndef CHIPEE_PERIPHERALS_H_
#define CHIPEE_PERIPHERALS_H_

void init_display();
void draw(unsigned char* display);
void sdl_ehandler(unsigned char* keypad);
void stop_display();

#endif
