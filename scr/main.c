#include <stdio.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED
#include "game.c"
int main (int argc, char *argv[]) {
    SDL_RWops* config = SDL_RWFromFile( "./data/config.txt", "r+b" );
    game(config);
    return 0;
}
