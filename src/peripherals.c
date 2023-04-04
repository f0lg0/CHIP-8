#include "peripherals.h"

#include <SDL2/SDL.h>

SDL_Window* screen;

// struct that handles all rendering
SDL_Renderer* renderer;

/**
 * Mapping Keyboard Keys
 *
 * A new layout is being used, not the original one:
 *
 * 1    2   3   4
 * q    w   e   r
 * a    s   d   f
 * z    x   c   v
 */
SDL_Scancode keymappings[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};

int QUIT = 0;

/**
 * init_display: initialize SDL display
 * @param void
 * @return void
 */
void init_display(void) {
    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 64 * 8, 32 * 8, 0);
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
}

/**
 * draw: draw SDL rectangle to screen
 * @param display a pointer to the display
 * @return void
 */
void draw(unsigned char* display) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // clear the current rendering target with the drawing color
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // iterating thru the display (64*32)
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (display[x + (y * 64)]) {
                SDL_Rect rect;

                rect.x = x * 8;
                rect.y = y * 8;
                rect.w = 8;
                rect.h = 8;

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // update the screen
    SDL_RenderPresent(renderer);
}

/**
 * sdl_handler: SDL Event Handler for keypress
 * @param keypad pointer to the keypad
 * @return void
 */
void sdl_ehandler(unsigned char* keypad) {
    SDL_Event event;

    // check for event
    if (SDL_PollEvent(&event)) {
        // get snapshot of current state of the keyboard
        const Uint8* state = SDL_GetKeyboardState(NULL);

        switch (event.type) {
            case SDL_QUIT:
                QUIT = 1;
                break;
            default:
                if (state[SDL_SCANCODE_ESCAPE]) {
                    QUIT = 1;
                }

                // updating the keypad with the current state
                for (int keycode = 0; keycode < 16; keycode++) {
                    keypad[keycode] = state[keymappings[keycode]];
                }

                break;
        }
    }
}

/**
 * should_quit: return QUIT status
 * @param void
 * @return QUIT flag
 */
int should_quit(void) { return QUIT; }

/**
 * stop_display: destroy SDL window and quit
 * @param void
 * @return void
 */
void stop_display(void) {
    SDL_DestroyWindow(screen);
    SDL_Quit();
}
