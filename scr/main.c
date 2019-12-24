#include <stdio.h>
#include <unistd.h>
#include "game.c"
int main () {
    SDL_RWops* config = SDL_RWFromFile( "./data/config.txt", "r+b" );
    game(config);
    return 0;
}
