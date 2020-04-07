#include <SDL.h>
#include <stdio.h>

#include "toolkit.h"

#include "curses.c"
#include "ui.c"
#include "game.c"

#undef main

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    C_init();
    C_loadFont("./data/art/font.bmp");


    int state = MENUE_MAIN;
    while (state != MENUE_EXIT) {
        switch (state) {
            AUTO_CASE(MENUE_MAIN, state = mainMenue());
            AUTO_CASE(MENUE_GAME, state = gameLoop());
        }
    }

    SDL_Quit();
    _Exit(0);
}