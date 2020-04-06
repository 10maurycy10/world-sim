#include <SDL.h>
#include <stdio.h>
#include "toolkit.h"
#include "./ui.c"
#include "curses.c"

#undef main

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    C_init();

    while (1) {
        pollIo(&ioHandeler);
    }

    SDL_Quit();
    _Exit(0);
}